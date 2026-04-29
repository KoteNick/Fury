#pragma once
#include <string>
#include <unordered_map>
#include "glm/glm.hpp"

enum class ShaderType;



class Shader
{
	struct ShaderTuple
	{
		std::string fp;
		ShaderType type;
	};
private:
	unsigned int m_RendererID;
	std::string m_fp;
	std::unordered_map<std::string, int> m_uniLocCache;
public:
	Shader();
	//Shader(std::initializer_list<ShaderTuple> list);
	~Shader();

	void Bind() const;
	void Unbind() const;

	void SetUniform1i(const std::string& name, int v);
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
	void AttachShader(ShaderType type, const std::string& source);
	int GetUnifromLocation(const std::string& name);
};

