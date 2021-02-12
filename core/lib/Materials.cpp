#include "Materials.h"
#include "../classes/Ray.h"
#include "../classes/Vector3.h"

namespace Tracer {
	namespace Materials {
		Material* GetMat(TraceResult* theResult) {
			Vector3 theColor = theResult->HitColor;

			Material* theMat = new Material;

			theMat->Emittance = theColor * 0.8;
			theMat->Reflectance = theColor; // no idea why i did this in SF
			theMat->ReflectPower = 0;

			return theMat;
		}
	}
}