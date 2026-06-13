#pragma once
#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Shader.h"
#include "Material.h"
#include "Mesh.h"
#include "UniformBuffer.h"
#include "FrameBuffer.h"

#include "RendererTypes.h"

#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>

#define ASSERT(x) if (!(x)) __debugbreak();
#define GLCall(x) GLClearError();\
        x;\
        ASSERT(GLLogCall(#x, __FILE__, __LINE__))

void GLClearError();

bool GLLogCall(const char* function, const char* file, int line);

struct CameraUBOData {
    glm::mat4 projectionView;
    glm::vec4 viewPos = glm::vec4(0);
};

struct RendererStats {
    unsigned int drawCalls = 0;
    unsigned int vertexCount = 0;
};

struct RenderCall
{
    Mesh* mesh;
    Material* material;
    glm::mat4 model;
};

struct LightData {
    glm::vec4 positionRadius; // x,y,z - pos, w - radius
    glm::vec4 colorIntensity; // r,g,b - color, a(w) - intensity
};

struct SunUBOData {
    // x,y,z = Direction, w = Intensity
    glm::vec4 DirectionIntensity = { 0.5f, -0.5f, 0.5f, 1.0f };

    // x,y,z = Red, Green, Blue, w = Sun Size
    glm::vec4 DiskColorAndSize = { 1, 1, 1, 0.995 };

    // x,y,z = Light RGB, w = 1.0
    glm::vec4 LightColor = glm::vec4(1.0);

    // x,y,z = Specular RGB, w = 1.0
    glm::vec4 SpecularColor = glm::vec4(1.0);
};

struct LightsUBOData
{
    int count = 0;
    int padding[3];
    LightData lights[Config::MAX_LIGHTS] = {};
};

class Renderer {
private:
    Renderer() {}
    ~Renderer() {}
private:
    unsigned int idMesh = 0, idTex = 0, idShader = 0, idUBO = 0;
    std::unordered_map<UBOslot, std::unique_ptr<UniformBuffer>> uniformBuffers;
public:
    static Renderer& Get() {
        static Renderer instance;
        return instance;
    }
    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;

public:
    std::vector<RenderCall> renderQueue;
    std::unique_ptr<FrameBuffer> shadowBuffer;
    glm::vec2 viewport;
    RendererStats rendererStats;
    
    void Submit(const RenderCall& call);
    void Flush(class Shader* shadowShader = nullptr, const glm::mat4& lightSpaceMatrix = glm::mat4(1.0f));
    void Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const;
    void Draw(RenderCall call);
    void SetViewport(int width, int height);
    void Clear() const;


    Renderer& AddUBO(UBOslot slot, unsigned int size);

    template<typename T>
    void SetUBO(UBOslot slot, const T& data, unsigned int offset = 0) {
        auto it = uniformBuffers.find(slot);

        if (it != uniformBuffers.end()) {
            if (it->second->GetID() != idUBO) {
                idUBO = it->second->GetID();
                it->second->Bind();
            }
            it->second->SetData(static_cast<const void*>(&data), sizeof(T), offset);
        }
    }

};