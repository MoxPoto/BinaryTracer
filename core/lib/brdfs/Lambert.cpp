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
			double r = sqrt(r1);
			double theta = r2 * 2. * M_PI;

			double x = r * cos(theta);
			double y = r * sin(theta);

			// Project z up to the unit hemisphere
			double z = sqrt(1.0 - x * x - y * y);

			return Vector3(x, y, z);
		}

		void Lambert::CreateCoordinateSystem(const Vector3& N, Vector3& Nt, Vector3& Nb) {
			if (std::abs(N.x) > std::abs(N.y))
				Nt = Vector3(N.z, 0.0, -N.x) / sqrt(N.x * N.x + N.z * N.z);
			else
				Nt = Vector3(0.0, -N.z, N.y) / sqrt(N.y * N.y + N.z * N.z);
			Nb = N.cross(Nt);
		}

		Vector3 Lambert::SampleWorld(const Vector3& sample, const Vector3& hitnormal, const Vector3& Nt, const Vector3& Nb) {
			Vector3 sampleWorld(
				sample.x * Nt.x + sample.y * Nb.x + sample.z * hitnormal.x,
				sample.x * Nt.y + sample.y * Nb.y + sample.z * hitnormal.y,
				sample.x * Nt.z + sample.y * Nb.z + sample.z * hitnormal.z);

			return sampleWorld;
		}

		double Lambert::GetPDF(const Vector3& sample, const Vector3& normal) {
			return sample.dot(normal) * M_1_PI;
		}
	}
}