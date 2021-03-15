#include "SerialHandler.h"
#include "../MainTracer.h"
#include "GarrysMod/Lua/Interface.h"

namespace Tracer {
	namespace Serial { // Serial as in relating to actually normal lua execution since we're unable to do that in parallel
		int notificationRef = 0; // notification reference to notifications.AddLegacy
		const std::string doneMsg = "[C++ Tracer]: Completed a trace!!";
		const double notifLength = 6;

		// EXECUTE ONLY WHEN THERE IS NOTHING ON THE STACK!
		void Initialize(GarrysMod::Lua::ILuaBase* LUA) {
			LUA->PushSpecial(GarrysMod::Lua::SPECIAL_GLOB);
			LUA->GetField(-1, "notification");
			LUA->GetField(-1, "AddLegacy");

			notificationRef = LUA->ReferenceCreate();

			LUA->Pop(3); // we pushed special, notification table and a function - so pop all these off
		}

		void Deinitialize(GarrysMod::Lua::ILuaBase* LUA) {
			LUA->ReferenceFree(notificationRef);
			notificationRef = 0;
		}

		// POLL IN THINK HOOK!
		void DoPolling(GarrysMod::Lua::ILuaBase* LUA) { // todo: add lua function support whenever it ends
			if (finishedTrace && tracing && notificationRef != 0) {
				LUA->ReferencePush(notificationRef);
					LUA->PushString(doneMsg.c_str());
					LUA->PushNumber(3);
					LUA->PushNumber(notifLength);
				LUA->Call(3, 0);

				tracing = false;
			}
		}
	}
}