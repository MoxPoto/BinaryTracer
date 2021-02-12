#include "Triangle.h"

namespace Tracer {
	Triangle CreateTri(const Vector3& v0, const Vector3& v1, const Vector3& v2, const Vector3& n1, const Vector3& n2, const Vector3& n3, const Vector3& col) {
		Triangle newTri;
		newTri.v0 = v0;
		newTri.v1 = v1;
		newTri.v2 = v2; 
		
		newTri.n1 = n1;
		newTri.n2 = n2;
		newTri.n3 = n3;

		newTri.color = col;

		return newTri;
	}
}