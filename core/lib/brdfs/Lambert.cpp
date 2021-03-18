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
			double z = sqrt(1.0f - x * x - y * y);

			return Vector3(x, y, z);
		}

		void Lambert::CreateCoordinateSystem(const Vector3& N, Vector3& Nt, Vector3& Nb) {
			if (std::abs(N.x) > std::abs(N.y))
				Nt = Vector3(N.z, 0, -N.x) / sqrt(N.x * N.x + N.z * N.z);
			else
				Nt = Vector3(0, -N.z, N.y) / sqrt(N.y * N.y + N.z * N.z);
			Nb = N.cross(Nt);
		}

		Vector3 Lambert::SampleWorld(const Vector3& sample, const Vector3& normal, const Vector3& Nt, const Vector3& Nb) {
			// Find an axis that is not parallel to normal
			Vector3 majorAxis;
			if (abs(normal.x) < 0.57735026919f /* 1 / sqrt(3) */) {
				majorAxis = Vector3(1, 0, 0);
			}
			else if (abs(normal.y) < 0.57735026919f /* 1 / sqrt(3) */) {
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
			return u * sample.x + v * sample.y + w * sample.z;
		}

		double Lambert::GetPDF(const Vector3& sample, const Vector3& normal) {
			return sample.dot(normal) * M_1_PI;
		}
	}
}