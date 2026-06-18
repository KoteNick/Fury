#include "Assets.h"
#include "Core/Primitives.h"
#include "Renderer/Vertex.h"

std::unordered_map<std::string, std::unique_ptr<Mesh>> Assets::meshes;
std::unordered_map<std::string, std::unique_ptr<Shader>> Assets::shaders;
std::unordered_map<std::string, std::unique_ptr<Texture>> Assets::textures;
std::unordered_map<std::string, Material> Assets::materials;

void Assets::Init() {
    std::vector<unsigned char> pixel = { 255, 255, 255, 255 };
    AddTexture("white", Texture(1, 1, pixel.data()));
    pixel = { 0, 0, 0, 0 };
    AddTexture("black", Texture(1, 1, pixel.data()));
    pixel = { 128, 128, 255, 255 };
    AddTexture("normal", Texture(1, 1, pixel.data()));

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
    AddMaterial("light3d", Material(GetShader("blinn_phong")))
        .AddTexture(GetTexture("white"), TextureSlot::Diffuse);

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

    AddShader("planet")
        .AddProgram("assets/shaders/planet.frag", ShaderType::FRAGMENT)
        .AddProgram("assets/shaders/blinn_phong.vert", ShaderType::VERTEX)
        .Build();
    AddMaterial("planet", Material(GetShader("planet")))
        .AddTexture(GetTexture("white"), TextureSlot::Diffuse)
        .AddTexture(GetTexture("normal"), TextureSlot::Normal)
        .AddTexture(GetTexture("black"), TextureSlot::Specular)
        .AddTexture(GetTexture("black"), TextureSlot::Emission);

    AddTexture("Earth", Texture("assets/textures/8k_earth_daymap.jpg"));
    AddTexture("EarthNight", Texture("assets/textures/8k_earth_nightmap.jpg"));
    AddTexture("EarthSpec", Texture("assets/textures/8k_earth_specular_map.jpg"));
    AddTexture("EarthNormal", Texture("assets/textures/8k_earth_normal_map.jpg"));
}

void Assets::Close() {
	meshes.clear();
	shaders.clear();
}