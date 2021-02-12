#ifndef MAT_LAB
#define MAT_LAB
// IDK WHY I PUT MAT_LAB?????

#include "../classes/Vector3.h"
#include "../classes/Ray.h"

namespace Tracer {
	namespace Materials {
		struct Material {
			Vector3 Emittance;
			Vector3 Reflectance;
			double ReflectPower;
		};

		extern Material* GetMat(TraceResult* theRay);
	}
}

#endif // !MAT_LAB
