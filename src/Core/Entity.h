#pragma once
#include "Component.h"
#include <glm/glm.hpp>
#include <unordered_map>
#include <memory>

class Entity
{
	std::unordered_map<int, std::unique_ptr<Component>> components;
public:
	std::string name;

	Entity() = default;
	Entity(const std::string& name) : name(name) {};

	Entity(const Entity& other);

	// MAKES A COPY!!!
	Entity& operator=(const Entity& other);

	void Update(float deltaTime);

	template <typename T>
	Entity& AddComponent(T component) {
		int id = ComponentID::Get<T>();

		components[id] = std::make_unique<T>(std::move(component));
		components[id]->parent = this;

		return *this;
	}

	template <typename T>
	T* GetComponent() {
		int id = ComponentID::Get<T>();
		auto it = components.find(id);
		if (it != components.end()) {
			return static_cast<T*>(it->second.get());
		}
		return nullptr;
	}

	template <typename T>
	bool HasComponent() const {
		int id = ComponentID::Get<T>();
		return components.count(id) > 0;
	}

	TransformComponent* Transform() {
		return GetComponent<TransformComponent>();
	}
};

