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

Renderer& Renderer::AddUBO(unsigned int slot, unsigned int size)
{
    uniformBuffers[slot] = std::make_unique<UniformBuffer>(size, slot);
    idUBO = uniformBuffers[slot]->GetID();

    return *this;
}

void Renderer::Submit(RenderCall& call)
{
    renderQueue.push_back(call);
}

void Renderer::Flush()
{
    // TODO: add sorting
    for (const RenderCall& r : renderQueue) {
        Draw(r);
    }
    renderQueue.clear();
}

void Renderer::Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const
{
    shader.Bind();
    va.Bind();
    ib.Bind();

    glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr);
}

void Renderer::Draw(RenderCall call)
{
    if (idShader != call.material->shader->GetId()) {
        idShader = call.material->shader->GetId();
        call.material->shader->Bind();
    }

    if (idMesh != call.mesh->GetId()) {
        idMesh = call.mesh->GetId();
        call.mesh->Bind();
    }

    call.material->shader->SetUniformMat4f("u_Model", call.model);
    call.material->ApplyUniforms();

    if (call.mesh->UseIndexBuffer())
        glDrawElements(GL_TRIANGLES, call.mesh->Count(), GL_UNSIGNED_INT, nullptr);
    else
        glDrawArrays(GL_TRIANGLES, 0, call.mesh->Count());
}

void Renderer::Clear() const
{
   glClear(GL_COLOR_BUFFER_BIT);
   glClear(GL_DEPTH_BUFFER_BIT);
}
