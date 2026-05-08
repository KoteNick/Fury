#include "Material.h"

Material::Material(Shader* shader) : shader(shader), isDirty(false)
{
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
}
