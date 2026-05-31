#include "Shader.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "Renderer.h"

Shader::Shader()
{
    wasCompiled = false;
    m_RendererID = glCreateProgram();
}

Shader::Shader(std::string vertexShaderLocation, std::string fragmentShaderLocation) : Shader()
{
    AddProgram(vertexShaderLocation, ShaderType::VERTEX);
    AddProgram(fragmentShaderLocation, ShaderType::FRAGMENT);
}

Shader::~Shader()
{
    glDeleteProgram(m_RendererID);
}

unsigned int Shader::GetId() const {
    return m_RendererID;
};

void Shader::Bind() const
{
    assert(wasCompiled && "Shader was not compiled");
    glUseProgram(m_RendererID);
}

void Shader::Unbind() const
{
    glUseProgram(0);
}

bool Shader::HasUniform(const std::string& name)
{
    auto it = m_uniLocCache.find(name);
    if (it != m_uniLocCache.end())
        return true;
    return false;
}

void Shader::SetUniform1i(const std::string& name, int v)
{
    if (int location = GetUniformLocation(name, GL_INT); location != -1)
        glUniform1i(location, v);
}

void Shader::SetUniformSampler2D(const std::string& name, TextureSlot slot)
{
    if (int location = GetUniformLocation(name, GL_SAMPLER_2D); location != -1)
        glUniform1i(location, static_cast<int>(slot));
}

void Shader::SetUniform1f(const std::string& name, float v)
{
    if (int location = GetUniformLocation(name, GL_FLOAT); location != -1)
        glUniform1f(location, v);
}

void Shader::SetUniform1iv(const std::string& name, unsigned int count, int* v)
{
    if (int location = GetUniformLocation(name, GL_INT); location != -1)
        glUniform1iv(location, count, v);
}

void Shader::SetUniform3f(const std::string& name, float v0, float v1, float v2)
{
    if (int location = GetUniformLocation(name, GL_FLOAT_VEC3); location != -1)
        glUniform3f(location, v0, v1, v2);
}

void Shader::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3)
{
    if (int location = GetUniformLocation(name, GL_FLOAT_VEC4); location != -1)
        glUniform4f(location, v0, v1, v2, v3);
}

void Shader::SetUniformMat4f(const std::string& name, const glm::mat4& matrix)
{
    if (int location = GetUniformLocation(name, GL_FLOAT_MAT4); location != -1)
        glUniformMatrix4fv(location, 1, GL_FALSE, &matrix[0][0]);
}

Shader& Shader::AddProgram(const std::string& filepath, ShaderType type)
{
    ParseShader(filepath, type);
    return *this;
}

Shader& Shader::Build() {
    glLinkProgram(m_RendererID);
    glValidateProgram(m_RendererID);
    CacheActiveUniforms();
    BindUniformBuffers();
    wasCompiled = true;
    return *this;
}

void Shader::ParseShader(const std::string& filepath, ShaderType type)
{
    if (type == ShaderType::NONE) {
        ParseShaderAuto(filepath);
        return;
    }
    std::ifstream fs(filepath);

    std::string line;
    std::stringstream ss;
    while (std::getline(fs, line)) {
        ss << line << '\n';
    }
    
    AttachCompiledShader(type, ss.str());

    return;
}

void Shader::ParseShaderAuto(const std::string& filepath) {
    std::ifstream fs(filepath);
    std::string line;

    std::unordered_map<ShaderType, std::string> shaderSources;
    ShaderType currentType = ShaderType::NONE;

    while (std::getline(fs, line)) {
        if (line.find("#shader") != std::string::npos) {
            if (line.find("vertex") != std::string::npos)
                currentType = ShaderType::VERTEX;
            else if (line.find("fragment") != std::string::npos)
                currentType = ShaderType::FRAGMENT;
        }
        else if (currentType != ShaderType::NONE) {
            shaderSources[currentType] += line + '\n';
        }
    }

    for (auto& kv : shaderSources) {
        AttachCompiledShader(kv.first, kv.second);
    }
}

unsigned int Shader::CompileShader(ShaderType type, const std::string& source) {
    unsigned int id = glCreateShader((unsigned int)type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE) {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        std::cout << message << std::endl;
        glDeleteShader(id);
        return 0;
    }

    return id;
}

void Shader::AttachCompiledShader(ShaderType type, const std::string& source)
{
    unsigned int id = CompileShader(type, source);
    glAttachShader(m_RendererID, id);
    glDeleteShader(id);
}

void Shader::BindUniformBuffers()
{
    for (UBOslot slot : Config::STANDART_UBO_SLOTS) {
        const char* blockName = ToString(slot);

        if (blockName[0] == '\0') continue;

        GLuint blockIndex = glGetUniformBlockIndex(m_RendererID, blockName);

        if (blockIndex != GL_INVALID_INDEX) {
            glUniformBlockBinding(m_RendererID, blockIndex, static_cast<GLuint>(slot));
        }
    }
}

void Shader::CacheActiveUniforms()
{
    int count;
    glGetProgramiv(m_RendererID, GL_ACTIVE_UNIFORMS, &count);

    const unsigned short bufSize = 256;
    char name[bufSize];
    int length;
    int size;
    unsigned int type;

    for (int i = 0; i < count; i++) {
        glGetActiveUniform(m_RendererID, (unsigned int)i, bufSize, &length, &size, &type, name);

        int location = glGetUniformLocation(m_RendererID, name);

        if (location != -1) {
            m_uniLocCache[std::string(name)] = { location, type };
        }
    }
}

int Shader::GetUniformLocation(const std::string& name, unsigned int dataType)
{
    auto it = m_uniLocCache.find(name);
    if (it != m_uniLocCache.end()) {
        if (dataType == it->second.type) {
            return it->second.location;
        }
        else {
            std::cout << "Wrong uniform type! " << name << std::endl;
            return -1;
        }
    }
    std::cout << "uniform " << name << " doesnt exist" << std::endl;
    return -1;
}