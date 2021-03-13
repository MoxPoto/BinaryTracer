
#ifndef RAY_LOADED
#define RAY_LOADED

#include "Object.h"
#include "Triangle.h"
#include "Vector3.h"

#include <memory>

namespace Tracer {

    extern Vector3 SUN_DIR;
    constexpr double BIAS = 0.001;

    struct TraceResult {
        bool Hit = false;
        std::shared_ptr<Object> Object;
        double t = 0;
        double u = 0;
        double v = 0;
        Vector3 HitColor = Vector3(1, 1, 1);
        Vector3 HitPos = Vector3(0, 0, 0);
        Vector3 HitNormal = Vector3(0, 0, 0);
    };

	class Ray {
		public:
            Vector3 orig;
            Vector3 dir;
            int ignoreID = -1;

            TraceResult* cast();

            Ray();
	};
}


#endif