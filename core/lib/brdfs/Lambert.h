#ifndef LAMBERT_BRDF
#define LAMBERT_BRDF

#include "GarrysMod/Lua/Interface.h"

#include "../../MainTracer.h"

#include "../../classes/Vector3.h"
#include "../../classes/Ray.h"
#include "../../classes/Object.h"

#include "../../vendor/Matrix.h"

#include <algorithm>

#include <random>
#include <chrono>

namespace Tracer {
	namespace BRDF {
		namespace Lambert {
			Vector3 Sampler(double r1, double r2);
			void CreateCoordinateSystem(const Vector3& hitnormal, Vector3& Nt, Vector3& Nb);
			Vector3 SampleWorld(const Vector3& sample, const Vector3& hitnormal, const Vector3& Nt, const Vector3& Nb);
			double GetPDF(const Vector3& sample, const Vector3& normal);
		};
	}
}

#endif