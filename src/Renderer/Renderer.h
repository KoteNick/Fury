#pragma once
#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Shader.h"
#include "Material.h"
#include "Mesh.h"
#include "UniformBuffer.h"

#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>

#define ASSERT(x) if (!(x)) __debugbreak();
#define GLCall(x) GLClearError();\
        x;\
        ASSERT(GLLogCall(#x, __FILE__, __LINE__))

void GLClearError();

bool GLLogCall(const char* function, const char* file, int line);

enum class LightType { Point, Directional };

enum class UBOslot : unsigned int {
    Camera = 0, // Expected to be CameraUBOData
    Ambient = 1, // Expected to be vec4
    Lights = 2, // Expected to be LightsUBOData
};

struct CameraUBOData {
    glm::mat4 projectionView;
    glm::vec4 viewPos = glm::vec4(0);
};

struct RenderCall
{
    Mesh* mesh;
    Material* material;
    glm::mat4 model;
};

struct PointLightData {
    glm::vec4 positionRadius; // x,y,z - pos, w - radius
    glm::vec4 colorIntensity; // r,g,b - color, a(w) - intensity
};

#define MAX_LIGHTS 8

struct LightsUBOData
{
    int count;
    int padding[3];
    PointLightData lights[MAX_LIGHTS] = {};
};

class Renderer {
private:
    Renderer() {}
    ~Renderer() {}
private:
    unsigned int idMesh = 0, idTex = 0, idShader = 0, idUBO = 0;
    std::unordered_map<unsigned int, std::unique_ptr<UniformBuffer>> uniformBuffers;
public:
    static Renderer& Get() {
        static Renderer instance;
        return instance;
    }
    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;

public:
    std::vector<RenderCall> renderQueue;
    
    void Submit(RenderCall& call);
    void Flush();
    void Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const;
    void Draw(RenderCall call);
    void Clear() const;


    Renderer& AddUBO(unsigned int slot, unsigned int size);
    Renderer& AddUBO(UBOslot slot, unsigned int size) {
        AddUBO(static_cast<unsigned int>(slot), size);
        return *this;
    };

    template<typename T>
    void SetUBO(unsigned int slot, const T& data, unsigned int offset = 0) {
        auto it = uniformBuffers.find(slot);

        if (it != uniformBuffers.end()) {
            if (it->second->GetID() != idUBO) {
                idUBO = it->second->GetID();
                it->second->Bind();
            }
            it->second->SetData(static_cast<const void*>(&data), sizeof(T), offset);
        }
    }
    template<typename T>
    void SetUBO(UBOslot slot, const T& data, unsigned int offset = 0) {
        SetUBO(static_cast<unsigned int>(slot), data, offset);
    }

};