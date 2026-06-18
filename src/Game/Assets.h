#pragma once
#include "Renderer/Mesh.h"
#include "Renderer/Material.h"


class Assets {
private:
    static std::unordered_map<std::string, std::unique_ptr<Mesh>> meshes;
    static std::unordered_map<std::string, std::unique_ptr<Shader>> shaders;
    static std::unordered_map<std::string, std::unique_ptr<Texture>> textures;
    static std::unordered_map<std::string, Material> materials;

public:
    static Mesh* GetMesh(const std::string& name) {
        auto it = meshes.find(name);
        if (it != meshes.end())
            return it->second.get();
        return nullptr;
    }

    static Mesh& AddMesh(const std::string& name, const VertexData& vertexData) {
        meshes[name] = std::make_unique<Mesh>(vertexData);
        return *meshes[name];
    }

    static Shader* GetShader(const std::string& name) {
        auto it = shaders.find(name);
        if (it != shaders.end())
            return it->second.get();
        return nullptr;
    }

    static Shader& AddShader(const std::string& name) {
        shaders[name] = std::make_unique<Shader>();
        return *shaders[name];
    }

    static Texture* GetTexture(const std::string& name) {
        auto it = textures.find(name);
        if (it != textures.end())
            return it->second.get();
        return nullptr;
    }

    static Texture& AddTexture(const std::string& name, Texture&& texture) {
        textures[name] = std::make_unique<Texture>(std::move(texture));
        return *textures[name];
    }

    static Material& GetMaterial(const std::string& name) {
        auto it = materials.find(name);
        if (it != materials.end())
            return it->second;

        static Material defaultMaterial(nullptr);
        return defaultMaterial;
    }

    static Material& AddMaterial(const std::string& name, const Material& material) {
        materials[name] = material;
        return materials[name];
    }

    static void Init();
    static void Close();
};