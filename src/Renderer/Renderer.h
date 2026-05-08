#pragma once
#include <GL/glew.h>
#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Shader.h"
#include "Material.h"
#include "Mesh.h"

#define ASSERT(x) if (!(x)) __debugbreak();
#define GLCall(x) GLClearError();\
        x;\
        ASSERT(GLLogCall(#x, __FILE__, __LINE__))

void GLClearError();

bool GLLogCall(const char* function, const char* file, int line);

struct RenderCall
{
    Mesh* mesh;
    Material* material;
    RenderCall(Mesh& mesh, Material& material) {
        this->mesh = &mesh;
        this->material = &material;
    }
};


class Renderer {
private:
    Renderer() {}
    ~Renderer() {}
public:
    static Renderer& Instance() {
        static Renderer instance;
        return instance;
    }
    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;
    void Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const;
    void Draw(RenderCall call);
    void Clear() const;
};

static Renderer& renderer = Renderer::Instance();