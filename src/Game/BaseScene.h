#pragma once
#include "Core/Entity.h"
#include "Core/Input.h"

#include "imgui/imgui.h"

#include "Assets.h"

#include <deque>

class BaseScene
{
	std::deque<Entity> entities;
	std::unordered_map<std::string, Entity*> entityCache;
public:
	bool debugUI;
	float time = 0;
	Entity* camera = nullptr;
	glm::vec3 ambient = {0.05f, 0.05f, 0.1f};

	Entity& CreateEntity(const std::string& name = "");
	Entity* GetEntity(const std::string& name);

	void UpdateEntities(float deltaTime);
	void OnRender(bool shadowPass = true);
	void DebugCameraMovement(float deltaTime, float speed = 15.0f, float speedBoostMultiplier = 3.0f, float sensitivity = 0.25f);

	virtual void Init();
	virtual void OnUpdate(float deltaTime);
	virtual void Destroy();
};

