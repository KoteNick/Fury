#pragma once
#include "Core/Entity.h"
#include "Core/Input.h"
#include "Assets.h"

#include <deque>

class BaseScene
{
	std::deque<Entity> entities;
	std::unordered_map<std::string, Entity*> entityCache;
public:
	Entity* camera = nullptr;
	glm::vec3 ambient = {0.05f, 0.05f, 0.1f};

	Entity& CreateEntity(const std::string& name);
	Entity* GetEntity(const std::string& name);

	void UpdateEntities(float deltaTime);
	void OnRender();

	virtual void Init();
	virtual void OnUpdate(float deltaTime);
};

