#include "Texture.h"

#include "stb_image/stb_image.h"

static std::array<unsigned int, Config::MAX_TEXTURE_SLOTS> s_BoundTextures = { 0 };

Texture::Texture(const std::string& path) : 
	m_LocalBuffer(nullptr), m_Width(0), m_Height(0)
{
	int m_BPP = 0;
	stbi_set_flip_vertically_on_load(true);
	m_LocalBuffer = stbi_load(path.c_str(), &m_Width, &m_Height, &m_BPP, 4);
	if (m_LocalBuffer) {
		glGenTextures(1, &m_RendererID);
		glBindTexture(GL_TEXTURE_2D, m_RendererID);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_LocalBuffer);
		Unbind();

		stbi_image_free(m_LocalBuffer);
	}	
}

Texture::Texture(int width, int height, unsigned char* data, GLenum internalFormat, GLenum format)
	: m_Width(width), m_Height(height), m_LocalBuffer(data)
{
	glGenTextures(1, &m_RendererID);
	glBindTexture(GL_TEXTURE_2D, m_RendererID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_Width, m_Height, 0, format, GL_UNSIGNED_BYTE, data);
	Unbind();
}

Texture::~Texture()
{
	glDeleteTextures(1, &m_RendererID);
}

unsigned int Texture::GetId() const {
	return m_RendererID;
}

void Texture::Bind(TextureSlot slot) const
{
	int slotIndex = static_cast<int>(slot);

	if (s_BoundTextures[slotIndex] != m_RendererID)
	{
		glActiveTexture(GL_TEXTURE0 + slotIndex);
		glBindTexture(GL_TEXTURE_2D, m_RendererID);

		s_BoundTextures[slotIndex] = m_RendererID;
	}
}

void Texture::Unbind() const
{
	glBindTexture(GL_TEXTURE_2D, 0);
}
