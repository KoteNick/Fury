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

    AddMesh("Plane", Primitives::CreatePlane());
    AddMesh("Cube", Primitives::CreateCube());
}

void Assets::Close() {
	meshes.clear();
	shaders.clear();
}