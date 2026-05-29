#include "Entity.h"

Entity::Entity(const Entity& other)
{
	for (const auto& pair : other.components) {
		this->components[pair.first] = pair.second->Clone();
		this->components[pair.first]->parent = this;
	}
}

Entity& Entity::operator=(const Entity& other)
{
	if (this != &other) {
		components.clear();
		for (const auto& pair : other.components) {
			components[pair.first] = pair.second->Clone();
			components[pair.first]->parent = this;
		}
	}
	return *this;
}

void Entity::Update(float deltaTime)
{
	for (auto& [id, component] : components)
	{
		component->OnUpdate(deltaTime);
	}
}
