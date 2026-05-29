#pragma once
#include "RendererTypes.h"
#include <string>
#include <array>


class Texture
{
private:
	unsigned int m_RendererID = 0;
	unsigned char* m_LocalBuffer = nullptr;
	int m_Width, m_Height;
public:
	Texture(const std::string& path);
	Texture(unsigned int glId, int w, int h) : m_RendererID(glId), m_Width(w), m_Height(h) {};
	~Texture();

	unsigned int GetId() const;
	void Bind(TextureSlot slot) const;
	void Unbind() const;

	inline int GetWidth() const{ return m_Width; };
	inline int GetHeight() const{ return m_Height; };
};

