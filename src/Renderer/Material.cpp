#include "Material.h"

Material::Material(Shader* shader) : shader(shader)
{
	if (shader->HasUniform("u_Color"))
		vec4s["u_Color"] = glm::vec4(1.0);
	if (shader->HasUniform("u_Shininess"))
		floats["u_Shininess"] = 64.f;
}

Material& Material::AddTexture(Texture* texture, TextureSlot slot)
{
	textures[static_cast<int>(slot)] = texture;

	return *this;
}

void Material::Bind() {
	for (int i = 0; i < textures.size(); i++) {
		if (textures[i] != nullptr)
			textures[i]->Bind(static_cast<TextureSlot>(i));
	}
}

void Material::ApplyUniforms()
{
	if (!isDirty || shader == nullptr)
		return;

	for (int i = 0; i < textures.size(); i++) {
		if (textures[i] != nullptr) {
			TextureSlot slot = static_cast<TextureSlot>(i);
			shader->SetUniformSampler2D(ToString(slot), slot);
		}
	}

	// Apply all uniforms from each type
	for (auto& v : ints)
		shader->SetUniform1i(v.first, v.second);

	for (auto& v : floats)
		shader->SetUniform1f(v.first, v.second);

	for (auto& v : vec3s)
		shader->SetUniform3f(v.first, v.second.x, v.second.y, v.second.z);

	for (auto& v : vec4s)
		shader->SetUniform4f(v.first, v.second.x, v.second.y, v.second.z, v.second.w);

	//isDirty = false;
}
