#pragma once
#include <vector>
#include <GL/glew.h>

struct VertexBufferElement
{
	unsigned int type;
	unsigned int count;
	unsigned char normalized;

	static unsigned int GetTypeSize(unsigned int type) {
		switch (type)
		{
			case GL_FLOAT:
				return sizeof(GLfloat);
			case GL_UNSIGNED_INT:
				return sizeof(GLuint);
			case GL_UNSIGNED_BYTE:
				return sizeof(GLubyte);
			default:
				break;
		}
		return 0;
	}
};

class VertexBufferLayout
{
private:
	std::vector<VertexBufferElement> m_Elements;
	unsigned int m_Stride;
public:
	VertexBufferLayout() : m_Stride(0) {};

	template<typename T>
	inline VertexBufferLayout& Push(unsigned int count) {
		static_assert(sizeof(T) == 0, "Unsupported type for VertexBufferLayout!");
		return *this;
	}
	template<>
	inline VertexBufferLayout& Push<float>(unsigned int count) {
		m_Elements.push_back({ GL_FLOAT, count, GL_FALSE });
		m_Stride += sizeof(GLfloat) * count;
		return *this;
	}
	template<>
	inline VertexBufferLayout& Push<unsigned int>(unsigned int count) {
		m_Elements.push_back({ GL_UNSIGNED_INT, count, GL_FALSE });
		m_Stride += sizeof(GLuint) * count;
		return *this;
	}
	template<>
	inline VertexBufferLayout& Push<unsigned char>(unsigned int count) {
		m_Elements.push_back({ GL_UNSIGNED_BYTE, count, GL_TRUE });
		m_Stride += sizeof(GLbyte) * count;
		return *this;
	}

	inline const std::vector<VertexBufferElement>& GetElements() const { return m_Elements; }
	inline unsigned int GetStride() const { return m_Stride; }
};

