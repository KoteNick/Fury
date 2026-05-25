#pragma once
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "IndexBuffer.h"

#include <memory>
#include <vector>

struct VertexData {
	std::vector<uint8_t> vertices_byte;
	VertexBufferLayout layout;
	std::vector<unsigned int> indices;

	template <typename T>
	VertexData(const std::vector<T>& vertices) {
		SetData(vertices);
	}

	template <typename T>
	VertexData& SetData(const std::vector<T>& vertices) {
		const uint8_t* raw_data = reinterpret_cast<const uint8_t*>(vertices.data());
		size_t byte_size = sizeof(T) * vertices.size();
		vertices_byte.assign(raw_data, raw_data + byte_size);
		return *this;
	};
};

class Mesh
{
private:
	size_t vertexCount;
	std::unique_ptr<VertexBuffer> vb;
	std::unique_ptr<VertexArray> va;
	std::unique_ptr<IndexBuffer> ib;
public:
	Mesh() = default;
	Mesh(const VertexData& vertexData);
	size_t Count();
	unsigned int GetId() const;
	bool UseIndexBuffer() const;
	void Bind();
	void Unbind();
};