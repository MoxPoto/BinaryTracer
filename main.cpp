#include <Windows.h>

#include "GarrysMod/Lua/Interface.h"
#include "core/MainTracer.h"
#include "core/classes/Vector3.h"
#include "core/classes/Object.h"
#include "core/lib/Lighting.h"

#include "core/lib/SerialHandler.h"

#include <chrono>
#include <iostream>

#include <memory>

#define VERSION "1.0.1"

using namespace GarrysMod::Lua;

void luaPrint(ILuaBase* LUA, const std::string& message) {
    LUA->PushSpecial(GarrysMod::Lua::SPECIAL_GLOB);
    LUA->GetField(-1, "print");
    LUA->PushString(message.c_str());

    LUA->Call(1, 0);
    LUA->Pop();
}

std::string vectorAsAString(const Tracer::Vector3& vec) {
    return std::to_string(vec.x) + ", " + std::to_string(vec.y) + ", " + std::to_string(vec.z);
}

LUA_FUNCTION(PT_StartRender) {
    if (Tracer::finishedTrace == false && Tracer::tracing) {
        LUA->ThrowError("The current trace isnt done!");
    }

    luaPrint(LUA, "Current number of objects in virtual scene: " + std::to_string(Tracer::AllObjects.size()));

	Tracer::StartRender(LUA);
    
    // This doesn't yield anymore

	return 0;
}

LUA_FUNCTION(PT_CameraChange) {
	Tracer::Vector3 posChange = Tracer::ConvertVector(LUA->GetVector()); // This quickly converts the lua vector into our vector just for the sake of simplicity
    LUA->Pop(1);
    Tracer::Vector3 up = Tracer::ConvertVector(LUA->GetVector());
    LUA->Pop(1);
    Tracer::Vector3 right = Tracer::ConvertVector(LUA->GetVector());
    LUA->Pop(1);
    Tracer::Vector3 forward = Tracer::ConvertVector(LUA->GetVector());

    luaPrint(LUA, "CameraPos: " + Tracer::vectorAsAString(posChange));

    luaPrint(LUA, "Forward: " + Tracer::vectorAsAString(forward));
    luaPrint(LUA, "Right: " + Tracer::vectorAsAString(right));
    luaPrint(LUA, "Up: " + Tracer::vectorAsAString(up));

	Tracer::LUACameraChange(posChange, forward, right, up);

    return 0;
}

LUA_FUNCTION(PT_CreateEnt) {
    Tracer::Vector3 objColor = Tracer::ConvertVector(LUA->GetVector());

    LUA->Pop(1);

    bool isLight = LUA->GetBool();

    LUA->Pop(1);

    Tracer::Vector3 objPos = Tracer::ConvertVector(LUA->GetVector());

    LUA->Pop(1);

    luaPrint(LUA, "Got the object color fine, " + Tracer::vectorAsAString(objColor));

    // also dis shit not mine: https://dav3.co/blog/looping-through-lua-table-in-c/
    size_t len = LUA->ObjLen();

    std::vector<Tracer::Vector3> verts;
    std::vector<Tracer::Vector3> normals;

    for (int index = 0; index <= len; index++) {
        // Our actual index will be +1 because Lua 1 indexes tables.
        int actualIndex = index + 1;
        // Push our target index to the stack.
        LUA->PushNumber(actualIndex);
        // Get the table data at this index (and not get the table, which is what I thought this did.)
        LUA->GetTable(-2);
        // Check for the sentinel nil element.
        if (LUA->GetType(-1) == GarrysMod::Lua::Type::Nil) break;
        // Get it's value.
        verts.push_back(Tracer::ConvertVector(LUA->GetVector()));
        // Pop it off again.
        LUA->Pop(1);
    }

    LUA->Pop(2); // Pop it off for the normal table

    len = LUA->ObjLen();

    for (int index = 0; index <= len; index++) {
        // Our actual index will be +1 because Lua 1 indexes tables.
        int actualIndex = index + 1;
        // Push our target index to the stack.
        LUA->PushNumber(actualIndex);
        // Get the table data at this index (and not get the table, which is what I thought this did.)
        LUA->GetTable(-2);
        // Check for the sentinel nil element.
        if (LUA->GetType(-1) == GarrysMod::Lua::Type::Nil) break;
        // Get it's value.
        normals.push_back(Tracer::ConvertVector(LUA->GetVector()));
        // Pop it off again.
        LUA->Pop(1);
    }

    luaPrint(LUA, "Received an object with " + std::to_string(len) + " vertices");

    // Tracer::Object obj;

    std::shared_ptr<Tracer::Object> obj = std::make_shared<Tracer::Object>();

    obj->mainColor = objColor;
    obj->isLight = isLight;
    obj->position = objPos;

    size_t vertIdx = 0; // iterate and form every triangle

    luaPrint(LUA, "Processing triangles");

    while (vertIdx < len) {
        Tracer::Vector3 p0;
        Tracer::Vector3 p1;
        Tracer::Vector3 p2;

        Tracer::Vector3 n1;
        Tracer::Vector3 n2;
        Tracer::Vector3 n3;

        p0 = verts[vertIdx];
        p1 = verts[vertIdx + 1]; // intellisense is stupid im litrally using a 8-byte value
        p2 = verts[vertIdx + 2];

        n1 = normals[vertIdx];
        n2 = normals[vertIdx + 1];
        n3 = normals[vertIdx + 2];

        Tracer::luaPrint("Processing normal that is located at (p0): " + Tracer::vectorAsAString(n1) + "!!");
        Tracer::luaPrint("Processing normal that is located at (p1): " + Tracer::vectorAsAString(n2) + "!!");
        Tracer::luaPrint("Processing normal that is located at (p2): " + Tracer::vectorAsAString(n3) + "!!");

        obj->InsertTri(p0, p1, p2, n1, n2, n3);

        vertIdx += 3; // move onto the next tri
    }

    luaPrint(LUA, "Added object successfully, added " + std::to_string(obj->tris.size()) + " triangles!");

    Tracer::AllObjects.push_back(obj);

    if (isLight) {
        Tracer::Lighting::LightObjects.push_back(obj);
    }

    return 0; // done
}

LUA_FUNCTION(PT_ClearAllEnts) {
    Tracer::ClearObjects();
    Tracer::Lighting::LightObjects.clear(); // meh

    return 0;
}

LUA_FUNCTION(PT_ChangeFOV) {
    Tracer::FOV = LUA->GetNumber();

    return 0;
}

LUA_FUNCTION(PT_ChangeDist) {
    Tracer::DISTANCE = LUA->GetNumber();

    return 0;
}

LUA_FUNCTION(InternalPoller) {
    // Runs in Think hook
    
    Tracer::Serial::DoPolling(LUA);

    return 0;
}

// Called when the module is loaded
GMOD_MODULE_OPEN()
{
    AllocConsole();

    FILE* pFile = nullptr;

    freopen_s(&pFile, "CONOUT$", "w", stdout);


    luaPrint(LUA, "mox's c++ tracer, version " + std::string(VERSION));

    Tracer::LUA_STATE = LUA;

	LUA->PushSpecial(GarrysMod::Lua::SPECIAL_GLOB);

	LUA->PushString("PT_StartRender");
	LUA->PushCFunction(PT_StartRender);
	LUA->SetTable(-3); // `_G.TestFunction = MyExampleFunction`

	LUA->PushString("PT_CameraChange");
	LUA->PushCFunction(PT_CameraChange);
	LUA->SetTable(-3); // `_G.TestFunction = MyExampleFunction`

    LUA->PushString("PT_CreateEnt");
    LUA->PushCFunction(PT_CreateEnt);
    LUA->SetTable(-3); // `_G.TestFunction = MyExampleFunction`

    LUA->PushString("PT_ClearAllEnts");
    LUA->PushCFunction(PT_ClearAllEnts);
    LUA->SetTable(-3); // `_G.TestFunction = MyExampleFunction`

    LUA->PushString("PT_ChangeFOV");
    LUA->PushCFunction(PT_ChangeFOV);
    LUA->SetTable(-3); // `_G.TestFunction = MyExampleFunction`

    LUA->PushString("PT_ChangeDist");
    LUA->PushCFunction(PT_ChangeDist);
    LUA->SetTable(-3); // `_G.TestFunction = MyExampleFunction`

    LUA->Pop(1); // Pop off global

    Tracer::Serial::Initialize(LUA);
    // Add poller onto think hook
    
    LUA->PushSpecial(GarrysMod::Lua::SPECIAL_GLOB);
    LUA->GetField(-1, "hook");
    LUA->GetField(-1, "Add");

    LUA->PushString("Think");
    LUA->PushString("pt_internal_poller");
    LUA->PushCFunction(InternalPoller);

    LUA->Call(3, 0);
    // Call removed the arguments and the Add function, so we need to pop 2 off the stack to leave it squeaky clean

    LUA->Pop(2);

	return 0;
}

// Called when the module is unloaded
GMOD_MODULE_CLOSE()
{
    FreeConsole();

    Tracer::Serial::Deinitialize(LUA);
    
    LUA->PushSpecial(GarrysMod::Lua::SPECIAL_GLOB);
    LUA->GetField(-1, "hook");
    LUA->GetField(-1, "Remove");

    LUA->PushString("Think");
    LUA->PushString("pt_internal_poller");

    LUA->Call(2, 0);
    // Call removed the arguments and the Remove function, so we need to pop 2 off the stack to leave it squeaky clean

    LUA->Pop(2);

    luaPrint(LUA, "mox's c++ tracer, version " + std::string(VERSION) + " - unloaded");

	return 0;
}