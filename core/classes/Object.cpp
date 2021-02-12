#include "Vector3.h"

#include "Object.h"
#include "Triangle.h"

#include <memory>

namespace Tracer {

	int _objectIDAlloc = 0;

	std::vector<std::shared_ptr<Object>> AllObjects;

	Object::Object() {
		objectID = ++_objectIDAlloc;
	}

	void Object::InsertTri(const Vector3& v0, const Vector3& v1, const Vector3& v2, const Vector3& n1, const Vector3& n2, const Vector3& n3) {
		Triangle newTri = CreateTri(v0, v1, v2, n1, n2, n3, mainColor);

		tris.push_back(newTri);
	}

	void ClearObjects() {
		AllObjects.clear();
	}

}