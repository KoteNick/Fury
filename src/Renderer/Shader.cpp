#include "Shader.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "Renderer.h"

Shader::Shader()
{
    m_RendererID = glCreateProgram();
}

Shader::~Shader()
{
    glDeleteProgram(m_RendererID);
}

void Shader::Bind() const
{
    glUseProgram(m_RendererID);
}

void Shader::Unbind() const
{
    glUseProgram(0);
}

void Shader::SetUniform1i(const std::string& name, int v)
{
    glUniform1i(GetUnifromLocation(name), v);
}

void Shader::SetUniform1f(const std::string& name, float v)
{
    glUniform1f(GetUnifromLocation(name), v);
}

void Shader::SetUniform1iv(const std::string& name, unsigned int count, int* v)
{
    glUniform1iv(GetUnifromLocation(name), count, v);
}

void Shader::SetUniform3f(const std::string& name, float v0, float v1, float v2)
{
    glUniform3f(GetUnifromLocation(name), v0, v1, v2);
}

void Shader::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3)
{
    glUniform4f(GetUnifromLocation(name), v0, v1, v2, v3);
}

void Shader::SetUniformMat4f(const std::string& name, const glm::mat4& matrix)
{
    glUniformMatrix4fv(GetUnifromLocation(name), 1, GL_FALSE, &matrix[0][0]);
}

Shader& Shader::AddProgram(const std::string& filepath, ShaderType type)
{
    ParseShader(filepath, type);
    return *this;
}

Shader& Shader::Build() {
    glLinkProgram(m_RendererID);
    glValidateProgram(m_RendererID);
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
    
    AttachShader(type, ss.str());

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
        AttachShader(kv.first, kv.second);
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

void Shader::AttachShader(ShaderType type, const std::string& source)
{
    unsigned int id = CompileShader(type, source);
    glAttachShader(m_RendererID, id);
    glDeleteShader(id);
}

int Shader::GetUnifromLocation(const std::string& name)
{
    if (m_uniLocCache.find(name) != m_uniLocCache.end())
        return m_uniLocCache[name];

    int location = glGetUniformLocation(m_RendererID, name.c_str());
    if (location == -1)
        std::cout << "uniform " << name << " doesnt exist" << std::endl;
    m_uniLocCache[name] = location;
    return location;
}
