#include "Mesh.h"

Mesh::Mesh(const VertexData& vertexData)
{
	va = std::make_unique<VertexArray>();

	vb = std::make_unique<VertexBuffer>(vertexData.vertices_byte.data(), vertexData.vertices_byte.size(), GL_STATIC_DRAW);
	va->AddBuffer(*vb, vertexData.layout);
	vertexCount = vertexData.vertices_byte.size() / vertexData.layout.GetStride();

	if (!vertexData.indices.empty())
		ib = std::make_unique<IndexBuffer>(vertexData.indices.data(), vertexData.indices.size());

	this->Unbind();
}

size_t Mesh::Count()
{
	if (UseIndexBuffer())
		return ib->GetCount();
	else
		return vertexCount;
}

unsigned int Mesh::GetId() const
{
	return va->GetId();
}

bool Mesh::UseIndexBuffer() const
{
	return (ib != nullptr);
}

void Mesh::Bind()
{
	va->Bind();
}

void Mesh::Unbind()
{
	if (va) va->Unbind();
	if (vb) vb->Unbind();
	if (ib) ib->Unbind();
}
