#include "Material.h"

Material::Material(Shader* shader) : shader(shader)
{
	vec4s["u_Color"] = glm::vec4(1.0);
	floats["u_Shininess"] = 64.f;
}

void Material::ApplyUniforms()
{
	if (!isDirty || shader == nullptr)
		return;

	// Apply all uniforms from each type
	for (auto& v : floats)
		shader->SetUniform1f(v.first, v.second);

	for (auto& v : vec3s)
		shader->SetUniform3f(v.first, v.second.x, v.second.y, v.second.z);

	for (auto& v : vec4s)
		shader->SetUniform4f(v.first, v.second.x, v.second.y, v.second.z, v.second.w);

	isDirty = false;
}
