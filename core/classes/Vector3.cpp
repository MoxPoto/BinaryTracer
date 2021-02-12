#include <math.h>
#include <algorithm>
#include <vector>

#include "Vector3.h"
#include "../vendor/Matrix.h"

const double PI = 3.14159;

double degToRad(const double& degrees) {
	return (degrees * PI) / 180;
}

namespace Tracer {

	Vector3::Vector3(double _x, double _y, double _z)
	{
		x = _x;
		y = _y;
		z = _z;
	}
#pragma endregion

#pragma region Methods
	// Calculates cross product between this and a vector
	Vector3 Vector3::cross(const Vector3& b) const
	{
		return Vector3(
			y * b.z - z * b.y,
			z * b.x - x * b.z,
			x * b.y - y * b.x
		);
	}

	// Calculates dot product between this and a vector
	double Vector3::dot(const Vector3& b) const
	{
		return x * b.x + y * b.y + z * b.z;
	}

	// Returns this vector rotated by a rotation matrix
	
	Vector3 Vector3::getRotated(Matrix<double>& matrix)
	{
		return Vector3(
			x * matrix[0][0] + y * matrix[0][1] + z * matrix[0][2],
			x * matrix[1][0] + y * matrix[1][1] + z * matrix[1][2],
			x * matrix[2][0] + y * matrix[2][1] + z * matrix[2][2]
		);
	}
	

	// Returns a normalised version of the vector
	Vector3 Vector3::getNormalized()
	{
		return *this / sqrt(x * x + y * y + z * z);
	}

	// Gets the distance between this vector and another
	double Vector3::getDistance(const Vector3& b) const
	{
		return sqrt((b.x - x) * (b.x - x) + (b.y - y) * (b.y - y) + (b.z - z) * (b.z - z));
	}

	// Gets the vector's length
	double Vector3::getLength()
	{
		return sqrt((x * x) + (y * y) + (z * z));
	}

	void Vector3::clamp(const double min, const double max)
	{
		x = std::clamp(x, min, max);
		y = std::clamp(y, min, max);
		z = std::clamp(z, min, max);
	}

	// Converts vector into color (SELF-MODIFIES WHEN CLAMPING 0-1)!!!!!
	Vector3 Vector3::toColor() {
		this->clamp(0.0, 1.0);

		return Vector3(
			x * 255,
			y * 255,
			z * 255
		);

	}

	// Converts vector into rotation matrix (AKA ANGLES BUT THEY'RE NOT A SEPERATE DAM TYPE)!!!!!!
	Matrix<double> Vector3::getRotationMatrix() 
	{
		// do sum vector math yo



		Vector3 forward = this->getNormalized();
		Vector3 right = forward.cross(UP_DIR);
		Vector3 left = -right;
		Vector3 theCorrectUp = right.cross(forward); 

		// the rotation matrix
		Matrix<double> daMatrix(3, 3);

		// FORWARD
		daMatrix[0][0] = forward.x;
		daMatrix[1][0] = forward.y;
		daMatrix[2][0] = forward.z;

		// Right
		daMatrix[0][1] = right.x;
		daMatrix[1][1] = right.y;
		daMatrix[2][1] = right.z;

		// UP
		daMatrix[0][2] = theCorrectUp.x;
		daMatrix[1][2] = theCorrectUp.y;
		daMatrix[2][2] = theCorrectUp.z;

		// Ok we done

		return daMatrix;
	}
	
	Vector3 Vector3::rotateByAngle(const Vector3& otherVec) 
	{
		Vector3 radiant(0, 0, 0);

		radiant.x = degToRad(otherVec.z); // source engine fuckery
		radiant.y = degToRad(otherVec.x);
		radiant.z = degToRad(otherVec.y);

		double cosAlpha = cos(radiant.x);
		double sinAlpha = sin(radiant.x);

		double cosBeta = cos(radiant.y);
		double sinBeta = sin(radiant.y);

		double cosGamma = cos(radiant.z);
		double sinGamma = sin(radiant.z);

		double newX, newY, newZ;
		double cX = x;
		double cY = y;
		double cZ = z;

		newY = cosAlpha * cY - sinAlpha * cZ;
		newZ = cosAlpha * cZ - sinAlpha * cY;

		cY = newY;
		cZ = newZ;

		newX = cosBeta * cX + sinBeta * cZ;
		newZ = cosBeta * cZ - sinBeta * cX;

		cX = newX;
		cZ = newX;

		newX = cosGamma * cX - sinGamma * cY;
		newY = cosGamma * cY + sinGamma * cX;

		cX = newX;
		cY = newY;

		return Vector3(cX, cY, cZ);
	
	}

	Vector3 Vector3::getForward() 
	{
		// I hate source engine so much
		// everybody in the world: forward,up,right
		// SOURCE: forward,right,up

		double pitch = z;
		double yaw = x;
		double roll = y;

		return Vector3(cos(pitch) * cos(yaw), cos(pitch) * sin(yaw), sin(pitch));
	}

#pragma endregion

#pragma region Operators
	Vector3 operator+(const Vector3& a, const Vector3& b) { return Vector3(a.x + b.x, a.y + b.y, a.z + b.z); }
	Vector3 operator-(const Vector3& a, const Vector3& b) { return Vector3(a.x - b.x, a.y - b.y, a.z - b.z); }
	Vector3 operator/(const Vector3& a, const Vector3& b) { return Vector3(a.x / b.x, a.y / b.y, a.z / b.z); }
	Vector3 operator/(const Vector3& a, const double b) { return Vector3(a.x / b, a.y / b, a.z / b); }
	Vector3 operator/(const double b, const Vector3& a) { return Vector3(a.x / b, a.y / b, a.z / b); }

	Vector3 operator*(const Vector3& a, const Vector3& b) { return Vector3(a.x * b.x, a.y * b.y, a.z * b.z); }
	Vector3 operator*(const Vector3& a, const double b) { return Vector3(a.x * b, a.y * b, a.z * b); }
	Vector3 operator*(const double b, const Vector3& a) { return Vector3(a.x * b, a.y * b, a.z * b); }

	Vector3& operator+=(Vector3& a, const Vector3& b)
	{
		a.x += b.x;
		a.y += b.y;
		a.z += b.z;
		return a;
	}
	Vector3& operator-=(Vector3& a, const Vector3& b)
	{
		a.x -= b.x;
		a.y -= b.y;
		a.z -= b.z;
		return a;
	}
	Vector3& operator/=(Vector3& a, const Vector3& b)
	{
		a.x /= b.x;
		a.y /= b.y;
		a.z /= b.z;
		return a;
	}
	Vector3& operator*=(Vector3& a, const Vector3& b)
	{
		a.x *= b.x;
		a.y *= b.y;
		a.z *= b.z;
		return a;
	}

	Vector3 Vector3::operator-() const
	{
		return Vector3(-x, -y, -z);
	}
#pragma endregion

#pragma endregion

	Vector3 ConvertVector(const Vector& theVec) {
		return Vector3(
			static_cast<double>(theVec.x),
			static_cast<double>(theVec.y),
			static_cast<double>(theVec.z)
		);
	}

	Vector3 UP_DIR = Vector3(0, 0, 1);
}