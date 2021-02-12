

#ifndef VECTOR_LOADED
#define VECTOR_LOADED

#include <math.h>
#include <vector>
#include "GarrysMod/Lua/SourceCompat.h"
#include "../vendor/Matrix.h"

namespace Tracer {
	class Vector3 {
		public:
			double x;
			double y;
			double z;
			Vector3(double x = 0, double y = 0, double z = 0);


			double getDistance(const Vector3& otherVec) const;
			Vector3 cross(const Vector3& otherVec) const;
			double dot(const Vector3& otherVec) const;
			Vector3 getNormalized();
			Vector3 getRotated(Matrix<double>& matrix);
			Vector3 rotateByAngle(const Vector3& otherVec);
			Vector3 getForward();
			void clamp(const double min, const double max);
			Vector3 toColor();
			Matrix<double> getRotationMatrix();

			double getLength();

			friend Vector3 operator+(const Vector3&, const Vector3&);
			friend Vector3 operator-(const Vector3&, const Vector3&);
			friend Vector3 operator/(const Vector3&, const Vector3&);
			friend Vector3 operator/(const Vector3&, const double);
			friend Vector3 operator/(const double, const Vector3&);
			friend Vector3 operator*(const Vector3&, const Vector3&);
			friend Vector3 operator*(const Vector3&, const double);
			friend Vector3 operator*(const double, const Vector3&);

			// Assignment operators
			friend Vector3& operator+=(Vector3&, const Vector3&);
			friend Vector3& operator-=(Vector3&, const Vector3&);
			friend Vector3& operator/=(Vector3&, const Vector3&);
			friend Vector3& operator*=(Vector3&, const Vector3&);

			// Unary operators
			Vector3 operator-() const;
	};

	Vector3 operator+(const Vector3&, const Vector3&);
	Vector3 operator-(const Vector3&, const Vector3&);
	Vector3 operator/(const Vector3&, const Vector3&);
	Vector3 operator/(const Vector3&, const double);
	Vector3 operator/(const double, const Vector3&);

	Vector3 operator*(const Vector3&, const Vector3&);
	Vector3 operator*(const Vector3&, const double);
	Vector3 operator*(const double, const Vector3&);

	Vector3& operator+=(Vector3&, const Vector3&);
	Vector3& operator-=(Vector3&, const Vector3&);
	Vector3& operator/=(Vector3&, const Vector3&);
	Vector3& operator*=(Vector3&, const Vector3&);

	
	extern Vector3 ConvertVector(const Vector& theVec);
	extern Vector3 UP_DIR;
}


#endif