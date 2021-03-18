#ifndef HDRI_LOAD
#define HDRI_LOAD

#include "../vendor/stb_image.h"
#include "Vector3.h"

namespace Tracer {
	class HDRI {
	public:
		unsigned char* radianceData;
		std::string filePath;

		double resX;
		double resY;
		int comps;
		double brightness = 0.05;

		HDRI(double rX, double rY, std::string filePath);
		bool load();
		double getPitch(const Vector3& N);
		double getYaw(const Vector3& N);
		Vector3 getPixel(const int& x, const int& y);
		Vector3 getPixelFromRay(const Vector3& vec);
	};
}

#endif