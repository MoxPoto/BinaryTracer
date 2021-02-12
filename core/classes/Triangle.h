
#ifndef TRI_LOADED
#define TRI_LOADED

#include "Vector3.h"

namespace Tracer {
	extern struct Triangle {
		Vector3 v0 = Vector3(1, 1, 1);
		Vector3 v1 = Vector3(1, 1, 1);
		Vector3 v2 = Vector3(1, 1, 1);

		Vector3 n1 = Vector3(1, 0, 1);
		Vector3 n2 = Vector3(1, 0, 1);
		Vector3 n3 = Vector3(1, 0, 1);

		Vector3 color = Vector3(1, 1, 1);
	};

	extern Triangle CreateTri(const Vector3& v0, const Vector3& v1, const Vector3& v2, const Vector3& n1, const Vector3& n2, const Vector3& n3, const Vector3& col);
}


#endif 