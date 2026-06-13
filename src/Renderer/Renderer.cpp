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

Renderer& Renderer::AddUBO(UBOslot slot, unsigned int size)
{
    uniformBuffers[slot] = std::make_unique<UniformBuffer>(size, static_cast<unsigned int>(slot));
    idUBO = uniformBuffers[slot]->GetID();

    return *this;
}

void Renderer::Submit(const RenderCall& call)
{
    renderQueue.push_back(call);
}

void Renderer::Flush(class Shader* shadowShader, const glm::mat4& lightSpaceMatrix)
{
    rendererStats = {};

    idShader = 0;
    idMesh = 0;

    if (shadowBuffer && shadowShader)
    {
        shadowBuffer->Bind();
        glClear(GL_DEPTH_BUFFER_BIT);

        shadowShader->Bind();
        shadowShader->SetUniformMat4f("u_LightSpaceMatrix", lightSpaceMatrix);

        for (const RenderCall& r : renderQueue) {

            if (idMesh != r.mesh->GetId()) {
                idMesh = r.mesh->GetId();
                r.mesh->Bind();
            }

            shadowShader->SetUniformMat4f("u_Model", r.model);

            if (r.mesh->UseIndexBuffer())
                glDrawElements(GL_TRIANGLES, r.mesh->Count(), GL_UNSIGNED_INT, nullptr);
            else
                glDrawArrays(GL_TRIANGLES, 0, r.mesh->Count());
            rendererStats.drawCalls++;
        }
        shadowBuffer->Unbind();
        glViewport(0, 0, viewport.x, viewport.y);
    }
    idShader = 0;
    idMesh = 0;

    Clear();

    if (shadowBuffer) {
        shadowBuffer->GetDepthTexture()->Bind(TextureSlot::ShadowMap);
    }

    for (const RenderCall& r : renderQueue) {
        if (idShader != r.material->shader->GetId()) {
            idShader = r.material->shader->GetId();
            r.material->shader->Bind();
        }

        r.material->Bind();

        if (idMesh != r.mesh->GetId()) {
            idMesh = r.mesh->GetId();
            r.mesh->Bind();
        }

        if (shadowBuffer && r.material->shader->HasUniform("u_ShadowMap")) {
            r.material->shader->SetUniformSampler2D("u_ShadowMap", TextureSlot::ShadowMap);
            r.material->shader->SetUniformMat4f("u_LightSpaceMatrix", lightSpaceMatrix);
        }

        r.material->shader->SetUniformMat4f("u_Model", r.model);
        r.material->ApplyUniforms();

        if (r.mesh->UseIndexBuffer())
            glDrawElements(GL_TRIANGLES, r.mesh->Count(), GL_UNSIGNED_INT, nullptr);
        else
            glDrawArrays(GL_TRIANGLES, 0, r.mesh->Count());
        rendererStats.drawCalls++;
        rendererStats.vertexCount += r.mesh->VertexCount();
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

void Renderer::SetViewport(int width, int height)
{
    viewport.x = width;
    viewport.y = height;
    glViewport(0, 0, width, height);
}

void Renderer::Clear() const
{
   glClear(GL_COLOR_BUFFER_BIT);
   glClear(GL_DEPTH_BUFFER_BIT);
}
