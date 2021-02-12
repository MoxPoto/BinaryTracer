#ifndef PATHLIB
#define PATHLIB

#include "../classes/Vector3.h"
#include "../classes/Ray.h"

namespace Tracer {
	namespace PathLib {

		// Describes the color of the trace and describes what got the trace there with the TraceResult
		struct PathResult {
			Vector3 Color;
			TraceResult* Trace;
		};

		// Performs a path-trace
		PathResult PathTrace(TraceResult* result, int samples, int maxDepth);
	}
}

#endif 