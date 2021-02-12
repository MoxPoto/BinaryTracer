#include <Windows.h>

#include "GarrysMod/Lua/Interface.h"
#include "core/MainTracer.h"
#include "core/classes/Vector3.h"
#include "core/classes/Object.h"

#include "core/lib/Lighting.h"

#include <chrono>
#include <iostream>

#include <memory>

using namespace GarrysMod::Lua;

void luaPrint(ILuaBase* LUA, const std::string& message) {
    LUA->PushSpecial(GarrysMod::Lua::SPECIAL_GLOB);
    LUA->GetField(-1, "print");
    LUA->PushString(message.c_str());

    LUA->Call(1, 0);
    LUA->Pop();
}


void dumpStack(ILuaBase* inst)
{
    std::string toPrint = "";

    int max = inst->Top();
    for (int i = 1; i <= max; i++) {
        toPrint += "[" + std::to_string(i) + "] ";
        switch (inst->GetType(i)) {
        case Type::Angle:
            toPrint += "Angle: (" + std::to_string((int)inst->GetAngle(i).x) + ", " + std::to_string((int)inst->GetAngle(i).y) + ", " + std::to_string((int)inst->GetAngle(i).z) + ")";
            break;
        case Type::Bool:
            toPrint += "Bool: " + inst->GetBool(i);
            break;
        case Type::Function:
            toPrint += "Function";
            break;
        case Type::Nil:
            toPrint += "nil";
            break;
        case Type::Number:
            toPrint += "Number: " + std::to_string(inst->GetNumber(i));
            break;
        case Type::String:
            toPrint += "String: " + (std::string)inst->GetString(i);
            break;
        case Type::Table:
            toPrint += "Table";
            break;
        default:
            toPrint += "Unknown";
            break;
        }
        toPrint += "\n";
    }

    std::cout << toPrint; // Output to c++ console
}

std::string vectorAsAString(const Tracer::Vector3& vec) {
    return std::to_string(vec.x) + ", " + std::to_string(vec.y) + ", " + std::to_string(vec.z);
}

LUA_FUNCTION(PT_StartRender) {
    luaPrint(LUA, "Current number of objects in virtual scene: " + std::to_string(Tracer::AllObjects.size()));

    auto start = std::chrono::high_resolution_clock::now();

	Tracer::StartRender(LUA);

    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> diff = end - start; 

    luaPrint(LUA, "Time spent: " + std::to_string(diff.count()));
    
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

    dumpStack(LUA);

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

    dumpStack(LUA);

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

// Called when the module is loaded
GMOD_MODULE_OPEN()
{
    AllocConsole();

    FILE* pFile = nullptr;

    freopen_s(&pFile, "CONOUT$", "w", stdout);


    luaPrint(LUA, "mox's c++ tracer, version 0.4");

    Tracer::LUA_STATE = LUA;


	LUA->PushSpecial(GarrysMod::Lua::SPECIAL_GLOB);
	LUA->PushString("PT_StartRender");
	LUA->PushCFunction(PT_StartRender);
	LUA->SetTable(-3); // `_G.TestFunction = MyExampleFunction`

	LUA->PushSpecial(GarrysMod::Lua::SPECIAL_GLOB);
	LUA->PushString("PT_CameraChange");
	LUA->PushCFunction(PT_CameraChange);
	LUA->SetTable(-3); // `_G.TestFunction = MyExampleFunction`

    LUA->PushSpecial(GarrysMod::Lua::SPECIAL_GLOB);
    LUA->PushString("PT_CreateEnt");
    LUA->PushCFunction(PT_CreateEnt);
    LUA->SetTable(-3); // `_G.TestFunction = MyExampleFunction`

    LUA->PushSpecial(GarrysMod::Lua::SPECIAL_GLOB);
    LUA->PushString("PT_ClearAllEnts");
    LUA->PushCFunction(PT_ClearAllEnts);
    LUA->SetTable(-3); // `_G.TestFunction = MyExampleFunction`

    LUA->PushSpecial(GarrysMod::Lua::SPECIAL_GLOB);
    LUA->PushString("PT_ChangeFOV");
    LUA->PushCFunction(PT_ChangeFOV);
    LUA->SetTable(-3); // `_G.TestFunction = MyExampleFunction`

    LUA->PushSpecial(GarrysMod::Lua::SPECIAL_GLOB);
    LUA->PushString("PT_ChangeDist");
    LUA->PushCFunction(PT_ChangeDist);
    LUA->SetTable(-3); // `_G.TestFunction = MyExampleFunction`

	return 0;
}

// Called when the module is unloaded
GMOD_MODULE_CLOSE()
{
    FreeConsole();

    luaPrint(LUA, "mox's c++ tracer, version 0.3 - unloaded");

	return 0;
}