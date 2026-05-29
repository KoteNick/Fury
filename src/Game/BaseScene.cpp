#include "BaseScene.h"

Entity& BaseScene::CreateEntity(const std::string& name)
{
    std::string finalName = name;
    int counter = 1;

    while (entityCache.find(finalName) != entityCache.end()) {
        finalName = name + " (" + std::to_string(counter) + ")";
        counter++;
    }

    entities.emplace_back(finalName);
    Entity& newEntity = entities.back();

    entityCache[finalName] = &newEntity;

    return newEntity;
}

Entity* BaseScene::GetEntity(const std::string& name)
{
	auto it = entityCache.find(name);
	if (it != entityCache.end())
		return it->second;
	return nullptr;
}

void BaseScene::UpdateEntities(float deltaTime)
{
	for (size_t i = 0; i < entities.size(); i++) {
		entities[i].Update(deltaTime);
	}
}

void BaseScene::OnRender()
{
	LightsUBOData lightData;
	glm::mat4 sunLightSpaceMatrix = glm::mat4(1.0f);
    for (auto& entity : entities) {
        if (entity.HasComponent<PointLightComponent>()) {
            if (lightData.count < Config::MAX_LIGHTS) {
                lightData.lights[lightData.count++] = entity.GetComponent<PointLightComponent>()->GetLightData();
            }
        }
        if (entity.HasComponent<DirectionalLightComponent>()) {
            auto sun = entity.GetComponent<DirectionalLightComponent>();

            if (lightData.count < Config::MAX_LIGHTS) {
                lightData.lights[lightData.count++] = sun->GetLightData();
            }

            if (sun->castsShadows) {
                auto transform = entity.Transform();
                sunLightSpaceMatrix = sun->GetLightSpaceMatrix();
            }
        }
		if (entity.HasComponent<RenderableComponent>()) {
			entity.GetComponent<RenderableComponent>()->Submit();
		}

		if (!camera) {
			if (entity.HasComponent<CameraComponent>())
				camera = &entity;
		}
    }
	if (camera)
		camera->GetComponent<CameraComponent>()->Submit();

	Renderer::Get().SetUBO(UBOslot::Lights, lightData);

	Renderer::Get().SetUBO(UBOslot::Ambient, glm::vec4(ambient, 1));

	Renderer::Get().Flush(Assets::GetShader("shadow_map"), sunLightSpaceMatrix);
}

void BaseScene::Init() {

}

void BaseScene::OnUpdate(float deltaTime) {
	UpdateEntities(deltaTime);
}