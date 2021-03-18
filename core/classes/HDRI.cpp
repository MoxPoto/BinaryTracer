#define _USE_MATH_DEFINES
#include <cmath>

#define STB_IMAGE_IMPLEMENTATION
#include "../vendor/stb_image.h"

#include <string>
#include "Vector3.h"
#include "HDRI.h"
#include "../MainTracer.h"

#include <iostream>

double sign(const double& num) // https://github.com/100PXSquared/gmod-binary-tracer/blob/56f482c041909494497d22dcf5c45d4f507aa022/Binary%20Module/pathtracer.cpp#L52
{
	if (num < 0.) return -1.;
	return 1.;
}

namespace Tracer {
	HDRI::HDRI(double rX, double rY, std::string filePath) {
		resX = rX;
		resY = rY;
		brightness = 2;
		comps = 3;

		filePath = filePath;
		radianceData = NULL;
	}

	bool HDRI::load() {
		int rX = static_cast<int>(resX);
		int rY = static_cast<int>(resY);

		radianceData = stbi_load(filePath.c_str(), &rX, &rY, &comps, 3);

		if (radianceData == NULL) {
			return false; // didnt load, dont use this
		}

		return true;
	}

	Vector3 HDRI::getPixel(const int& x, const int& y) {
		int rX = static_cast<int>(resX);

		if (radianceData != NULL) {
			const stbi_uc* pixel = radianceData + (3 * (y * rX + x));

			return Vector3(pixel[0] / 255.0, pixel[1] / 255.0, pixel[2] / 255.0);
		}

		return Vector3(1, 0, 0);
	}

	double HDRI::getPitch(const Vector3& N) { // https://github.com/100PXSquared/gmod-binary-tracer/blob/56f482c041909494497d22dcf5c45d4f507aa022/Binary%20Module/pathtracer.cpp#L97
		if (N.x == 0 && N.y == 0) return M_PI / 2. * sign(N.z);
		return asin(abs(N.z)) * sign(N.z);
	}

	double HDRI::getYaw(const Vector3& N) { // https://github.com/100PXSquared/gmod-binary-tracer/blob/56f482c041909494497d22dcf5c45d4f507aa022/Binary%20Module/pathtracer.cpp#L103
		if (N.y == 0) return N.x >= 0 ? 0 : M_PI;
		return acos((N * Vector3(1., 1., 0)).getNormalized().dot(Vector3(1., 0, 0))) * sign(N.y);
	}

	Vector3 HDRI::getPixelFromRay(const Vector3& N) { // https://github.com/100PXSquared/gmod-binary-tracer/blob/56f482c041909494497d22dcf5c45d4f507aa022/Binary%20Module/pathtracer.cpp#L109
		if (radianceData == NULL) return Vector3(0, 0, 0);

		double y = resY / 2. - ((getPitch(N) * 2.) / M_PI) * resY / 2.;
		double x = resX / 2. + (getYaw(N) / M_PI) * resX / 2.;

		y = (fmod(floor(y), resY - 1.));
		x = (fmod(floor(x), resX - 1.));

		// std::cout << std::to_string(y) << " : " << std::to_string(x) << "\nWith the dir: " << vectorAsAString(N) << std::endl;

		Vector3 color = getPixel(x, y);

		color = color * (color * brightness);

		return color;
	}
}