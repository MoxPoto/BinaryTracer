#include "Lighting.h"

#include "../classes/Ray.h"
#include "../classes/Triangle.h"
#include "../classes/Vector3.h"

#include "Settings.h"

#include "GarrysMod/Lua/Interface.h"

#include "../MainTracer.h"

#include <vector>
#include <math.h>
#include <algorithm>

#include <memory>

bool lightDebug = false;

// Checks if the ray provided is shadowed
bool shadowCheck(Tracer::TraceResult* result) {
	Tracer::Ray* testRay = new Tracer::Ray;
	testRay->orig = result->HitPos;
	testRay->dir = Tracer::SUN_DIR;

	Tracer::TraceResult* skyResult = testRay->cast();

	bool hitSky = skyResult->Hit;

	delete skyResult;
	delete testRay; // better to manually allocate the memory so i can directly delete them since these are so trivial for them to be kept alive

	return (hitSky == false); // If the Hit parameter isnt set to true, then we hit the sky 
}

namespace Tracer {
	namespace Lighting {
		double LIGHT_RANGE = 360;
		double LIGHT_BRIGHTNESS = 1;

		std::vector<std::shared_ptr<Object>> LightObjects;

		// Checks if the specified object is a light emitter (equal to object.isLight)
		bool IsLight(std::shared_ptr<Object> light) {
			return light->isLight; // bruh
		}

		// Checks if the ray is exposed to any of the lights in the scene
		bool IsReceivingLight(TraceResult* result) {
			if (shadowCheck(result)) {
				return true;
			}

			bool passed = false;

			for (std::shared_ptr<Object> light : LightObjects) {
				Vector3 dir = (light->position - result->HitPos).getNormalized();

				Ray* testRay = new Ray;
				testRay->orig = result->HitPos;
				testRay->dir = dir;

				TraceResult* theResult = testRay->cast();

				if (theResult->Hit == true && theResult->Object->objectID == light->objectID) {
					passed = true;
				}

			}

			return passed;

		}


		// Calculates lighting by taking the average exposed light to the ray (returns 0-1)
		Vector3 CalculateLighting(TraceResult* result) {
			double rComp = 0;
			double gComp = 0;
			double bComp = 0;

			int num = 0;

			for (std::shared_ptr<Object> light : LightObjects) {
				Vector3 dir = (light->position - result->HitPos).getNormalized();

				Ray* testRay = new Ray;
				testRay->orig = result->HitPos + (result->HitNormal);
				testRay->dir = dir;
				testRay->ignoreID = result->Object->objectID; // Dont hit the same object we JUST hit again

				TraceResult* theResult = testRay->cast();

				if (theResult->Hit == true && theResult->Object->objectID == light->objectID && theResult->t < LIGHT_RANGE) {
					double realRange = std::clamp(theResult->t, 0.0, LIGHT_RANGE);
					double fraction = (realRange / LIGHT_RANGE);

					double range = 1.0 - fraction;

					Vector3 lightCol = ((light->mainColor * range) * LIGHT_BRIGHTNESS);

					if (lightDebug == false) {
						luaPrint("[LIGHTING]: realrange = " + std::to_string(realRange));
						luaPrint("[LIGHTING]: fraction = " + std::to_string(fraction));
						luaPrint("[LIGHTING]: range = " + std::to_string(range));
						luaPrint("[LIGHTING]: orig = " + vectorAsAString(testRay->orig));
						luaPrint("[LIGHTING]: lightCol = " + vectorAsAString(lightCol));

						lightDebug = true;
					}

					num = num + 1;

					rComp += lightCol.x;
					gComp += lightCol.y;
					bComp += lightCol.z;
				}

				delete testRay;
				delete theResult;
			}

			if (num == 0) { // do not do any math if we.. well literally hit no lights..
				return Vector3(0, 0, 0);
			}

			// Cast to double first

			num = static_cast<double>(num);

			return Vector3(rComp / num, gComp / num, bComp / num);

		}
		

	}
}