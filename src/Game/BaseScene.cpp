#include "BaseScene.h"

Entity& BaseScene::CreateEntity(const std::string& name)
{
    if (name.empty()) {
        entities.emplace_back("Entity");
        return entities.back();
    }

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

void BaseScene::OnRender(bool shadowPass)
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
    
    if (shadowPass)
        Renderer::Get().Flush(Assets::GetShader("shadow_map"), sunLightSpaceMatrix);
    else
        Renderer::Get().Flush();
}

void BaseScene::DebugCameraMovement(float deltaTime, float speed, float speedBoostMultiplier, float sensitivity)
{
    speed *= deltaTime;

    if (!ImGui::GetIO().WantCaptureKeyboard) {
        glm::vec3 move(0);

        if (Input::IsKeyPressed(Key::LeftShift)) speed*=speedBoostMultiplier;

        if (Input::IsKeyPressed(Key::W)) move.z += speed;
        if (Input::IsKeyPressed(Key::S)) move.z -= speed;
        if (Input::IsKeyPressed(Key::A)) move.x -= speed;
        if (Input::IsKeyPressed(Key::D)) move.x += speed;

        if (Input::IsKeyPressed(Key::Space)) camera->Transform()->pos.y += speed;
        if (Input::IsKeyPressed(Key::C)) camera->Transform()->pos.y -= speed;

        camera->Transform()->Move(move);
    }

    if (Input::IsMouseDown(MouseButton::Right) && !ImGui::GetIO().WantCaptureMouse) {
        glm::vec2 delta = Input::GetMouseDelta();

        camera->Transform()->rot.y -= delta.x * sensitivity;
        camera->Transform()->rot.x -= delta.y * sensitivity;

        if (camera->Transform()->rot.x > 90.f)  camera->Transform()->rot.x = 90.f;
        if (camera->Transform()->rot.x < -90.f) camera->Transform()->rot.x = -90.f;

        float scroll = Input::GetScrollDelta();
        if (scroll != 0.0f) {
            camera->GetComponent<CameraComponent>()->fov -= scroll * 5.0f;
        }
    }
}

void BaseScene::Init() {

}

void BaseScene::OnUpdate(float deltaTime) {
	UpdateEntities(deltaTime);
}

void BaseScene::Destroy()
{
    entities.clear();
    entityCache.clear();
    camera = nullptr;
}
