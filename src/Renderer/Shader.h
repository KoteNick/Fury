#pragma once
#include <GL/glew.h>
#include <string>
#include <unordered_map>
#include "glm/glm.hpp"

#include "RendererTypes.h"

enum class ShaderType {
	NONE = -1, VERTEX = GL_VERTEX_SHADER, FRAGMENT = GL_FRAGMENT_SHADER
};

class Shader
{
public:
	struct ShaderTuple
	{
		std::string fp;
		ShaderType type;
	};
	struct UniformInfo {
		int location;
		unsigned int type;
	};
private:
	bool wasCompiled;
	unsigned int m_RendererID;
	std::unordered_map<std::string, UniformInfo> m_uniLocCache;
public:
	Shader();
	Shader(std::string vertexShaderLocation, std::string fragmentShaderLocation);
	//Shader(std::initializer_list<ShaderTuple> list);
	~Shader();

	unsigned int GetId() const;
	void Bind() const;
	void Unbind() const;
	bool HasUniform(const std::string& name);

	void SetUniform1i(const std::string& name, int v);
	void SetUniformSampler2D(const std::string& name, TextureSlot slot);
	void SetUniform1f(const std::string& name, float v);
	void SetUniform1iv(const std::string& name, unsigned int count, int* v);
	void SetUniform3f(const std::string& name, float v0, float v1, float v2);
	void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
	void SetUniformMat4f(const std::string& name, const glm::mat4& matrix);

	Shader& AddProgram(const std::string& filepath, ShaderType type = ShaderType::NONE);
	Shader& Build();
private:
	void ParseShader(const std::string& filepath, ShaderType type);
	void ParseShaderAuto(const std::string& filepath);
	unsigned int CompileShader(ShaderType type, const std::string& source);
	void AttachCompiledShader(ShaderType type, const std::string& source);

	void BindUniformBuffers();
	void CacheActiveUniforms();
	int GetUniformLocation(const std::string& name, unsigned int dataType);
};

