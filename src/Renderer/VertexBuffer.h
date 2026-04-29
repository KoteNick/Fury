#pragma once
#include <GL/glew.h>

class VertexBuffer
{
private:
	unsigned int m_RendererID;
public:
	VertexBuffer();
	VertexBuffer(const void* data, unsigned int size, unsigned int drawType = GL_STATIC_DRAW);
	~VertexBuffer();

	void Bind() const;
	void Unbind() const;
	const unsigned int GetID() const;
};

