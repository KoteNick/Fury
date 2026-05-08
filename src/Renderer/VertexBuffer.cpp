#include "VertexBuffer.h"
#include "Renderer.h"

VertexBuffer::VertexBuffer() : m_RendererID(0)
{
}

VertexBuffer::VertexBuffer(const void* data, unsigned int size, unsigned int drawType)
{
    glGenBuffers(1, &m_RendererID);
    glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
    glBufferData(GL_ARRAY_BUFFER, size, data, drawType);
}

VertexBuffer::~VertexBuffer()
{
    glDeleteBuffers(1, &m_RendererID);
}

void VertexBuffer::Bind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
}

void VertexBuffer::Unbind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

const unsigned int VertexBuffer::GetID() const
{
    return m_RendererID;
}
