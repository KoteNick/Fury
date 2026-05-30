#pragma once
#include "Shader.h"
#include "Texture.h"

#include <unordered_map>
#include <array>

class Material
{
	bool isDirty = true;
public:
	std::array<Texture*, Config::MAX_LOCAL_TEXTURES> textures = { nullptr };

	Shader* shader = nullptr;
	Material() = default;
	Material(Shader* shader);

	Material& AddTexture(Texture* texture, TextureSlot slot);
	void Bind();
	void ApplyUniforms();

	template <typename T>
	T& uniform(const std::string& name);

private:

	// Uniform types to add
	std::unordered_map<std::string, float> floats;
	std::unordered_map<std::string, glm::vec3> vec3s;
	std::unordered_map<std::string, glm::vec4> vec4s;
};


// Uniform access functions for maps of each type
template<>
inline float& Material::uniform<float>(const std::string& name) {
	isDirty = true;
	return floats[name];
}

template<>
inline glm::vec3& Material::uniform<glm::vec3>(const std::string& name) {
	isDirty = true;
	return vec3s[name];
}

template<>
inline glm::vec4& Material::uniform<glm::vec4>(const std::string& name) {
	isDirty = true;
	return vec4s[name];
}