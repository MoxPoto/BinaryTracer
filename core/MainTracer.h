#pragma once

// the main tracer namespace \\
// loads shit in too \\

#ifndef MAIN_LOAD

#define MAIN_LOAD

#include "classes/Vector3.h"
#include "vendor/Matrix.h"
#include "GarrysMod/Lua/Interface.h"

#include <string>


namespace Tracer {
	extern double QUALITY;
	extern int SAMPLES;
	extern int MAX_DEPTH;

	extern double DISTANCE;

	extern double FOV;

	extern GarrysMod::Lua::ILuaBase* LUA_STATE;

	// SETTINGS STRUCT WHY DID I MAKE THIS HELPPPP
	extern struct Settings {
		int QUALITY;
		int SAMPLES;
		int MAX_DEPTH;
	};

	extern Vector3 CAMERA_POS;
	extern Matrix<double> CAMERA_DIR;

	extern bool finishedTrace;
	extern bool tracing;

	extern void calculateViewPlane(const int& x, const int& y, Vector3& origin, Vector3& direction);

	extern void luaPrint(const std::string& message);
	extern std::string vectorAsAString(const Tracer::Vector3& vec);

	extern std::string fileName;

	extern void StartRender(GarrysMod::Lua::ILuaBase* LUA);
	extern void ChangeCamera(const Vector3& newPos, const Matrix<double>& newDir);

	extern void LUACameraChange(const Vector3& newPos, const Vector3& forward, const Vector3& right, const Vector3& up);

	extern void ChangeSettings(int SAMPLES, int MAX_DEPTH);
	extern Settings GetSettings();
}

#endif