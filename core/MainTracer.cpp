
#include "MainTracer.h"
#include "classes/Ray.h"
#include "lib/Settings.h"
#include "lib/PathLib.h"

#include "omp.h"

#include "GarrysMod/Lua/Interface.h"

#include <thread>

#include <string>

#include <iostream>
#include <fstream>
#include <sstream>

#define STB_IMAGE_IMPLEMENTATION
#include "vendor/stb_image.h"

#define STBI_MSC_SECURE_CRT
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "vendor/stb_image_write.h"

#include <math.h>

#include "vendor/Matrix.h"

#define M_PI 3.14159

#define RES 512

std::string makeFileName(int QUALITY, int SAMPLES) {
	char theBuffer[400];

	snprintf(theBuffer, sizeof(theBuffer), "C:\\pathtracer\\%ix%i_%iSPP_Render.png", QUALITY, QUALITY, SAMPLES);

	std::string newStr = theBuffer; 

	return newStr;
}

void _renderScene(const double& QUALITY, Tracer::Vector3* pixels) {
	using namespace Tracer;

	for (int y = 0; y < (int)QUALITY; ++y) {
	#pragma omp parallel for num_threads(8) 
		for (int x = 0; x < (int)QUALITY; ++x) {

			// [x][y] is rewritten as x*sizeY+y

			Vector3* rayDir = new Vector3(0, 0, 0);
			Vector3* rayPos = new Vector3(0, 0, 0);

			calculateViewPlane(x + 1, y + 1, *rayPos, *rayDir);

			Ray* theRay = new Ray;
			theRay->orig = *rayPos;
			theRay->dir = *rayDir;

			TraceResult* traceResult = theRay->cast();

			PathLib::PathResult pathResult = PathLib::PathTrace(traceResult, SAMPLES, MAX_DEPTH);

			pixels[x * (int)QUALITY + y] = pathResult.Color;


			delete rayPos;
			delete rayDir;
			delete theRay;
			delete traceResult; // Clean up ALL the memory
		}

	}

	finishedTrace = true;
	// let the lua side turn off tracing mode

	// IMAGE RENDERING \\

	uint8_t* pixelsFrame = new uint8_t[QUALITY * QUALITY * 3];

	int index = 0;
	for (int y = QUALITY - 1; y >= 0; --y)
	{
		for (int x = 0; x < QUALITY; ++x)
		{
			int fakeY = (QUALITY - 1) - y; // get inverse so the image does not flip..
			int fakeX = (QUALITY - 1) - x;

			Vector3 colorStruct = pixels[fakeX * (int)QUALITY + fakeY].toColor();

			int ir = colorStruct.x;
			int ig = colorStruct.y;
			int ib = colorStruct.z;

			pixelsFrame[index++] = (int)ir;
			pixelsFrame[index++] = (int)ig;
			pixelsFrame[index++] = (int)ib;
		}
	}


	stbi_write_png(fileName.c_str(), QUALITY, QUALITY, 3, pixelsFrame, QUALITY * 3);

	delete[] pixelsFrame;
	delete[] pixels; // Kill the pixels off, we r done
}


namespace Tracer {

	double QUALITY = RES;
	int SAMPLES = 50;
	int MAX_DEPTH = 4;

	double DISTANCE = 1;
	double FOV = 65;

	const bool NORMAL_DEBUG = false;

	GarrysMod::Lua::ILuaBase* LUA_STATE = nullptr;

	Vector3 CAMERA_POS = Vector3(1, 1, 1);
	Matrix<double> CAMERA_DIR(3, 3);

	Vector3 CAM_FORWARD;
	Vector3 CAM_RIGHT;
	Vector3 CAM_UP;

	std::string fileName = makeFileName(QUALITY, SAMPLES);

	bool finishedTrace = true;
	bool tracing = false;

	// todo: add duration again

	void luaPrint(const std::string& message) {
		if (LUA_STATE != nullptr) {
			/*
			GarrysMod::Lua::ILuaBase* LUA = LUA_STATE; // dont ask

			LUA->PushSpecial(GarrysMod::Lua::SPECIAL_GLOB);
			LUA->GetField(-1, "print");
			LUA->PushString(message.c_str());

			LUA->Call(1, 0);
			LUA->Pop();
			*/

			std::cout << message << std::endl;
		}
	}

	std::string vectorAsAString(const Tracer::Vector3& vec) {
		return std::to_string(vec.x) + ", " + std::to_string(vec.y) + ", " + std::to_string(vec.z);
	}

	void calculateViewPlane(const int& x, const int& y, Vector3& origin, Vector3& direction) {
		double coeff = DISTANCE * tan((FOV / 2) * (M_PI / 180)) * 2;
		origin = Vector3(
			DISTANCE,
			(static_cast<double>(QUALITY - x) / static_cast<double>(QUALITY - 1) - 0.5) * coeff,
			(coeff / static_cast<double>(QUALITY)) * static_cast<double>(QUALITY - y) - 0.5 * (coeff / static_cast<double>(QUALITY)) * static_cast<double>(QUALITY - 1)
		).getRotated(CAMERA_DIR);
		direction = origin.getNormalized();
		origin += CAMERA_POS; // NOT MY CODE!! https://github.com/100PXSquared/public-starfalls/tree/master/raytracer
	}

	void StartRender(GarrysMod::Lua::ILuaBase* theLua) {
		tracing = true;
		finishedTrace = false;

		Vector3* pixels = new Vector3[RES * RES]; // i had to move the giant pixel array to da heap, also deleted in _renderScene, so don't count on it later
		
		omp_set_dynamic(0); // removes dynamic teams cuz fuck them

		std::thread renderThread(_renderScene, QUALITY, pixels);

		renderThread.detach(); // Run in a seperate hardware thread to not interrupt gmod
	}

	void ChangeCamera(const Vector3& newPos, const Matrix<double>& newDir) {
		CAMERA_DIR = newDir;
		CAMERA_POS = newPos;
	}

	void ChangeSettings(int QUALITY, int SAMPLES, int MAX_DEPTH) {
		QUALITY = QUALITY;
		SAMPLES = SAMPLES;
		MAX_DEPTH = MAX_DEPTH;
	}

	void LUACameraChange(const Vector3& newPos, const Vector3& forward, const Vector3& right, const Vector3& up) {
		CAMERA_POS = newPos;

		CAM_FORWARD = forward;
		CAM_RIGHT = right;
		CAM_UP = up;

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
		daMatrix[0][2] = up.x;
		daMatrix[1][2] = up.y;
		daMatrix[2][2] = up.z;

		Tracer::CAMERA_DIR = daMatrix;
	}

	Settings GetSettings() {
		Settings settings;
		settings.SAMPLES = SAMPLES;
		settings.QUALITY = QUALITY; // I DID NOT JUST MAK AN ENTIERYG HEINJDSF
		settings.MAX_DEPTH = MAX_DEPTH;

		return settings;
	}
}