#define _USE_MATH_DEFINES
#include <cmath>

#include "GarrysMod/Lua/Interface.h"
#include "Lambert.h"

#include "../../MainTracer.h"

#include "../../classes/Vector3.h"
#include "../../classes/Ray.h"
#include "../../classes/Object.h"

#include "../../vendor/Matrix.h"

#include <algorithm>

namespace Tracer {
	namespace BRDF {

		Vector3 Lambert::Sampler(double r1, double r2) {
			float sinTheta = sqrt(1 - r1 * r1);
			float phi = 2 * M_PI * r2;
			float x = sinTheta * cos(phi);
			float z = sinTheta * sin(phi);

			return Vector3(x, r1, z);
		}

		void Lambert::CreateCoordinateSystem(const Vector3& N, Vector3& Nt, Vector3& Nb) {
			if (std::abs(N.x) > std::abs(N.y))
				Nt = Vector3(N.z, 0, -N.x) / sqrt(N.x * N.x + N.z * N.z);
			else
				Nt = Vector3(0, -N.z, N.y) / sqrt(N.y * N.y + N.z * N.z);
			Nb = N.cross(Nt);
		}

		Vector3 Lambert::SampleWorld(const Vector3& sample, const Vector3& hitnormal, const Vector3& Nt, const Vector3& Nb) {
			Vector3 sampleWorld(
				sample.x * Nt.x + sample.y * Nb.x + sample.z * hitnormal.x,
				sample.x * Nt.y + sample.y * Nb.y + sample.z * hitnormal.y,
				sample.x * Nt.z + sample.y * Nb.z + sample.z * hitnormal.z);

			return sampleWorld;
		}
	}
}