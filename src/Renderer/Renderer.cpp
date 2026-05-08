#include "Renderer.h"
#include <iostream>

void GLClearError()
{
    while (glGetError() != GL_NO_ERROR);
}

bool GLLogCall(const char* function, const char* file, int line)
{
    while (GLenum error = glGetError()) {
        std::cout << "[OpenGL Error] " << error << " | " << function << ' ' << file << ' ' << line << std::endl;
        return false;
    }
    return true;
}

void Renderer::Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const
{
    shader.Bind();
    va.Bind();
    ib.Bind();

    GLCall(glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr));
}

void Renderer::Draw(RenderCall call)
{
    call.material->shader->Bind();
    call.mesh->Bind();

    if (call.mesh->UseIndexBuffer())
        glDrawElements(GL_TRIANGLES, call.mesh->Count(), GL_UNSIGNED_INT, nullptr);
    else
        glDrawArrays(GL_TRIANGLES, 0, call.mesh->Count());
}

void Renderer::Clear() const
{
    GLCall(glClear(GL_COLOR_BUFFER_BIT));
}
