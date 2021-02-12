
#ifndef OBJECT_LOADED
#define OBJECT_LOADED

#include "Vector3.h"

#include "Triangle.h"

#include <vector>
#include <memory>

namespace Tracer {
	extern int _objectIDAlloc;

	class Object {
		public:
			std::vector<Tracer::Triangle> tris;
			
			Tracer::Vector3 mainColor = Vector3(1,1,1);

			int objectID;
			bool isLight = false;

			Tracer::Vector3 position;

			Object();
			void InsertTri(const Vector3& v0, const Vector3& v1, const Vector3& v2, const Vector3& n1, const Vector3& n2, const Vector3& n3);
	};

	extern std::vector<std::shared_ptr<Object>> AllObjects;

	extern void ClearObjects();
}


#endif