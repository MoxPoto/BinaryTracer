-- Load everything --

if gmod13_close == nil then 
    CompileFile("autorun/client/gm_binary_util.lua")()
end 

if PT_StartRender == nil then 
    require("pathtracer")
end 

local function rotateVecMatrix(vec, matrix)
    local x, y, z = vec[1], vec[2], vec[3]

    return Vector( 
        x * matrix:GetField(1, 1) + y * matrix:GetField(1, 2) + z * matrix:GetField(1, 3),
        x * matrix:GetField(2, 1) + y * matrix:GetField(2, 2) + z * matrix:GetField(2, 3),
        x * matrix:GetField(3, 1) + y * matrix:GetField(3, 2) + z * matrix:GetField(3, 3)
    )
end 

function PTLUA_AddObject(ent, isLight)
    isLight = isLight or false

    local modelName = ent:GetModel()

    local vismeshes = util.GetModelMeshes(modelName);

    if (!vismeshes) then return end

    local theMesh = vismeshes[1]

    local matrixTransform = ent:GetWorldTransformMatrix()

    local theVertsTbl = {}
    local theNormTbl = {} -- each normal corresponds to each vertex in the tbl above

    for k , v in ipairs(theMesh.triangles) do 
        local theNewVec = v.pos
        theNewVec = ent:LocalToWorld(theNewVec)
        // theNewVec:Rotate(Angle(0,90,0))

        theVertsTbl[#theVertsTbl+1] = theNewVec
    end

    for k , v in ipairs(theMesh.triangles) do
        local norm = v.normal 
        // norm:Rotate(Angle(0, -180, 0))

        norm = rotateVecMatrix(norm, matrixTransform):GetNormalized()

        print("Normal (LUA): " .. tostring(norm))

        theNormTbl[k] = norm
    end

    local convertedColor = Vector(0,0,0)
    local shittyColor = ent:GetColor() 

    convertedColor = Vector(shittyColor.r / 255, shittyColor.g / 255, shittyColor.b / 255)
    
    print("Creating new entity with color: ")
    print(tostring(convertedColor))

    PT_CreateEnt(theNormTbl, theVertsTbl, ent:GetPos(), isLight, convertedColor)
end

function PTLUA_SetCamera()
    print("Setting new camera pos and dir - updated")

    print("LUA SIDE: EyePos: " .. tostring(LocalPlayer():EyePos()))

    local matrix = Matrix()
    matrix:SetAngles(LocalPlayer():EyeAngles() + Angle(0,0,0))

    print("Updated setcamera")
    //matrix:GetForward(), matrix:GetRight(), matrix:GetUp(),

    local ourEyeAngles = LocalPlayer():EyeAngles() + Angle(0,0,0)


    PT_CameraChange(ourEyeAngles:Forward(), ourEyeAngles:Right(), ourEyeAngles:Up(), LocalPlayer():EyePos())
end

function PTLUA_Clear()
    print("Clearing all entities")
    PT_ClearAllEnts()
end

function PTLUA_StartRender() 
    print("HERE WE GO.. RENDERING")
    PT_StartRender()
end 

function PTLUA_AddAllObjects()
    for k , v in pairs(ents.FindByClass("prop_physics")) do 
        if (CPPI and v:CPPIGetOwner() == LocalPlayer()) or (game.SinglePlayer()) then 
            PTLUA_AddObject(v, ((v:GetMaterial() == "lights/white") and true or false))
        end
    end
end

function PTLUA_SetSamples(amt)
    PT_SetSamples(amt)
end

function PTLUA_SetMaxDepth(amt)
    PTLUA_SetMaxDepth(amt)
end

PTLUA_Clear() -- Clear firstly