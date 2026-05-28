#pragma once
#include <GL/glew.h>

class Texture;

class FrameBuffer
{
private:
    unsigned int m_RendererID = 0;
    Texture* m_DepthTexture = nullptr;
    unsigned int m_Width, m_Height;
public:
    FrameBuffer(unsigned int width, unsigned int height);
    ~FrameBuffer();

    void Bind() const;
    void Unbind() const;

    Texture* GetDepthTexture() const { return m_DepthTexture; }
    unsigned int GetWidth() const { return m_Width; }
    unsigned int GetHeight() const { return m_Height; }
};

