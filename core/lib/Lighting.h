#ifndef LIGHT_LIB
#define LIGHT_LIB

#include "../classes/Vector3.h"
#include "../classes/Ray.h"
#include "../classes/Object.h"

#include <vector>

namespace Tracer {
	namespace Lighting {
		extern double LIGHT_RANGE;
		extern double LIGHT_BRIGHTNESS;

		extern std::vector<std::shared_ptr<Object>> LightObjects;

		extern bool IsLight(std::shared_ptr<Object> light);
		extern bool IsReceivingLight(TraceResult* rayResult);

		extern Vector3 CalculateLighting(TraceResult* rayResult);

	}
}

#endif