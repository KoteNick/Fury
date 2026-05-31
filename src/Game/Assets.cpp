#include "Assets.h"
#include "Core/Primitives.h"
#include "Renderer/Vertex.h"

std::unordered_map<std::string, std::unique_ptr<Mesh>> Assets::meshes;
std::unordered_map<std::string, std::unique_ptr<Shader>> Assets::shaders;
std::unordered_map<std::string, Material> Assets::materials;

void Assets::Init() {
    std::vector<Vertex2D> v = {
            {+0.0, +0.5, 1.0, 1.0, 1.0},
            {-0.5, -0.5, 1.0, 1.0, 0.0},
            {+0.5, -0.5, 0.0, 1.0, 1.0}
    };
    VertexData vd(v);
    vd.layout.Push<float>(2).Push<float>(3);
    AddMesh("Triangle", vd);

    AddShader("Shader2D")
        .AddProgram("assets/shaders/Basic2D.frag", ShaderType::FRAGMENT)
        .AddProgram("assets/shaders/Basic2D.vert", ShaderType::VERTEX)
        .Build();
    AddMaterial("Basic2D", Material(GetShader("Shader2D")));

    AddShader("Shader3D")
        .AddProgram("assets/shaders/Basic3D.frag", ShaderType::FRAGMENT)
        .AddProgram("assets/shaders/Basic3D.vert", ShaderType::VERTEX)
        .Build();
    AddMaterial("Basic3D", Material(GetShader("Shader3D")));

    AddShader("blinn_phong")
        .AddProgram("assets/shaders/blinn_phong.frag", ShaderType::FRAGMENT)
        .AddProgram("assets/shaders/blinn_phong.vert", ShaderType::VERTEX)
        .Build();
    AddMaterial("light3d", Material(GetShader("blinn_phong")));

    AddShader("shadow_map")
        .AddProgram("assets/shaders/shadow_map.vert", ShaderType::VERTEX)
        .Build();

    AddShader("sky")
        .AddProgram("assets/shaders/sky.vert", ShaderType::VERTEX)
        .AddProgram("assets/shaders/sky.frag", ShaderType::FRAGMENT)
        .Build();
    AddMaterial("sky", Material(GetShader("sky")));

    AddShader("plains")
        .AddProgram("assets/shaders/plains.vert", ShaderType::VERTEX)
        .AddProgram("assets/shaders/plains.frag", ShaderType::FRAGMENT)
        .Build();
    AddMaterial("plains", Material(GetShader("plains")));

    AddShader("terrain")
        .AddProgram("assets/shaders/terrain.vert", ShaderType::VERTEX)
        .AddProgram("assets/shaders/terrain.frag", ShaderType::FRAGMENT)
        .Build();
    AddMaterial("terrain", Material(GetShader("terrain")));

    AddShader("ocean")
        .AddProgram("assets/shaders/ocean.vert", ShaderType::VERTEX)
        .AddProgram("assets/shaders/ocean.frag", ShaderType::FRAGMENT)
        .Build();
    AddMaterial("ocean", Material(GetShader("ocean")));

    AddMesh("Plane", Primitives::CreatePlane());
    AddMesh("Plane100", Primitives::CreatePlane(100, 100, 100, 100));
    AddMesh("Plane100250", Primitives::CreatePlane(100, 100, 250, 250));
    AddMesh("Plane100500", Primitives::CreatePlane(100, 100, 500, 500));
    AddMesh("Plane50500", Primitives::CreatePlane(100, 100, 500, 500));
    AddMesh("Plane200x600", Primitives::CreatePlane(200, 200, 600, 600));
    AddMesh("Cube", Primitives::CreateCube());
    AddMesh("Sphere", Primitives::CreateSphere());
}

void Assets::Close() {
	meshes.clear();
	shaders.clear();
}