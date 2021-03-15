#ifndef SERIAL_H
#define SERIAL_H

#include "GarrysMod/Lua/Interface.h"
#include "../MainTracer.h"
#include <string>

namespace Tracer {
	// Serial as in relating to actually normal lua execution since we're unable to do that in parallel
	namespace Serial {
		extern int notificationRef; // notification reference to notifications.AddLegacy
		extern const std::string doneMsg;
		extern const double notifLength; // Notification length

		// EXECUTE ONLY WHEN THERE IS NOTHING ON THE STACK!
		extern void Initialize(GarrysMod::Lua::ILuaBase* LUA);

		// IDK EXECUTE WHENEVER DOESNT REALLY NEED TO BE IMPORTANT
		extern void Deinitialize(GarrysMod::Lua::ILuaBase* LUA);

		// POLL IN THINK HOOK!
		extern void DoPolling(GarrysMod::Lua::ILuaBase* LUA);
	}
}

#endif 