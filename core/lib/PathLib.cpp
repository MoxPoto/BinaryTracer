#define _USE_MATH_DEFINES

#include <cmath>

#include "PathLib.h"
#include "Materials.h"
#include "Settings.h"
#include "Lighting.h"
#include "brdfs/Lambert.h"

#include "GarrysMod/Lua/Interface.h"

#include "../MainTracer.h"

#include "../classes/Vector3.h"
#include "../classes/Ray.h"
#include "../classes/Object.h"

#include "../vendor/Matrix.h"

#include <algorithm>

#include <random>
#include <chrono>

#include <iostream>

#include <memory>

// this bitch the REAL big ass library

double PI = M_PI;

std::default_random_engine randEngine;
std::uniform_real_distribution<double> unif(0.0, 1.0);

/*
local function uniformSampling(r1, r2) -- SO MUCH THX TO http://www.rorydriscoll.com/2009/01/07/better-sampling/
	local r = math.sqrt(r1)
	local theta = 2 * PI * r2

	local x = r * math.cos(theta)
	local y = r * math.sin(theta)

	return Angle(x, y, math.sqrt(math.max(0, 1 - r1)))
end
*/

// thx https://github.com/100PXSquared/gmod-binary-tracer/blob/master/Binary%20Module/shaders.cpp
Tracer::Vector3 transformToWorld(const double& x, const double& y, const double& z, const Tracer::Vector3& normal)
{
	using namespace Tracer;

	// Find an axis that is not parallel to normal
	Vector3 majorAxis;
	if (abs(normal.x) < 0.57735026919 /* 1 / sqrt(3) */) {
		majorAxis = Vector3(1, 0, 0);
	}
	else if (abs(normal.y) < 0.57735026919 /* 1 / sqrt(3) */) {
		majorAxis = Vector3(0, 1, 0);
	}
	else {
		majorAxis = Vector3(0, 0, 1);
	}

	// Use majorAxis to create a coordinate system relative to world space
	Vector3 u = normal.cross(majorAxis).getNormalized();
	Vector3 v = normal.cross(u);
	Vector3 w = normal;

	// Transform from local coordinates to world coordinates
	return u * x + v * y + w * z;
}


Tracer::Vector3 uniformSampling(const double& r1, const double& r2, const Tracer::Vector3& normal) { // this isn't a constant PDF btw
	using namespace Tracer;

	double r = sqrt(r1);
	double theta = r2 * 2. * M_PI;

	double x = r * cos(theta);
	double y = r * sin(theta);

	// Project z up to the unit hemisphere
	double z = sqrt(1.0 - x * x - y * y);

	return Vector3(x, y, z);
		
}


// https://github.com/100PXSquared/gmod-binary-tracer/blob/56f482c041909494497d22dcf5c45d4f507aa022/Binary%20Module/shaders.cpp#L57 th an kyou 
Tracer::Vector3 lambertEvaluate(Tracer::Vector3 pathCol, Tracer::Vector3 dir, Tracer::Vector3 normal) {
	return pathCol * (M_1_PI * dir.dot(normal));
}


int oneDebug = 1;

Tracer::Vector3 doTrace(Tracer::TraceResult* ray, int depth, int maxDepth, int samples) {
	using namespace Tracer;

	if (depth > maxDepth) {
		return Vector3(0, 0, 0);
	}
	if (ray->Hit == false) {
		return mainHDRI->getPixelFromRay(ray->Direction);
	}
	if (Lighting::IsLight(ray->Object)) {
		return ray->Object->mainColor;
	}

	// initialize a uniform distribution between 0 and 1

	// HI ITS ME NOT SOME STACKOVERFLOW GUY OK THAT CODE ABOVE COMES FROM: https://stackoverflow.com/a/31091422

	// TODO: Implement water again like last time

	Materials::Material* mat = Materials::GetMat(ray);

	Vector3 emittance = mat->Emittance;
	Vector3 reflectance = mat->Reflectance;
	double reflectPower = mat->ReflectPower;

	Vector3 directLighting(0.0, 0.0, 0.0);
	Vector3 indirectLighting(0.0, 0.0, 0.0);

	// THE MEAT OF THE ENTIRE TRACER \\

	directLighting = Lighting::CalculateLighting(ray);

	if (depth + 1 <= maxDepth) {
		Vector3 hitNormal = -ray->HitNormal; // cache it
		Vector3 biased = ray->HitPos;

		Vector3 Nt, Nb;

		BRDF::Lambert::CreateCoordinateSystem(hitNormal, Nt, Nb);
		

		double pdf = 1 / (2 * M_PI);

		for (int i = 0; i < samples; i++) { // ayo i++ or ++i
			double r1 = unif(randEngine);
			double r2 = unif(randEngine);

			Vector3 theUnitVec = BRDF::Lambert::Sampler(r1, r2);
			Vector3 theUnit = BRDF::Lambert::SampleWorld(theUnitVec, hitNormal, Nt, Nb);

			if (isnan(theUnit.x)) {
				i = i - 1;
				continue;
			}

			Ray* newRay = new Ray;
			newRay->orig = biased;
			newRay->dir = theUnit;
			// newRay->ignoreID = ray->Object->objectID;

			TraceResult* theResult = newRay->cast();
			// double pdf = BRDF::Lambert::GetPDF(theUnit, hitNormal);
			// double cos_theta = theUnit.dot(hitNormal);

			Vector3 theIndirectColor = ((doTrace(theResult, depth + 1, maxDepth, 1)) * r1) / pdf;

			indirectLighting += theIndirectColor;

			if (oneDebug < 5) {
				luaPrint("THE ONE SINGULAR DEBUG.\nThe indirect color: " + vectorAsAString(theIndirectColor));
				luaPrint("theUnit: " + vectorAsAString(theUnit));
				luaPrint("theUnitVec: " + vectorAsAString(theUnitVec));
				luaPrint("cos_theta: " + std::to_string(r1));
				luaPrint("direct lighting: " + vectorAsAString(directLighting));
				luaPrint("pdf: " + std::to_string(pdf));
				 
				luaPrint("randomDist1: " + std::to_string(unif(randEngine)));
				luaPrint("randomDist2: " + std::to_string(unif(randEngine)));

				oneDebug++;
			}

			delete newRay; 
			delete theResult;
		}

		indirectLighting = indirectLighting / static_cast<double>(samples);
	}

	// TODO: implement the reflecting shit from sf

	Vector3 finalHitColor = (directLighting / PI + 2.0 * indirectLighting) * (emittance / PI); // All dis shit from scratchapixel 
	//Vector3 finalHitColor = (indirectLighting) * emittance;
	
	delete mat;

	return finalHitColor;

}

namespace Tracer {
	namespace PathLib {
		PathResult PathTrace(TraceResult* result, int samples, int maxDepth) {
			PathResult theResult;

			if (result->Hit == false) {
				theResult.Color = mainHDRI->getPixelFromRay(result->Direction);
				theResult.Trace = result;

			}
			else {
				theResult.Color = doTrace(result, 0, maxDepth, samples);
				theResult.Trace = result;
			}

			return theResult;
		}
	}
}