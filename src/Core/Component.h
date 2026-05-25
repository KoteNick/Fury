#pragma once
#include "Renderer/Renderer.h"

#include <memory>
#include <glm/ext/matrix_transform.hpp>

class Entity;

struct Component
{
    Entity* parent = nullptr;

    virtual ~Component() = default;
    virtual void OnUpdate(float deltaTime) {};
    virtual std::unique_ptr<Component> Clone() const = 0;
};


class ComponentID {
private:
    static int GetNextId() {
        static int counter = 0;
        return counter++;
    }
public:
    template <typename T>
    static int Get() {
        static_assert(std::is_base_of_v<Component, T>, "T should be child of Component");

        static const int id = GetNextId();
        return id;
    }
};

struct TransformComponent : Component {
    glm::vec3 pos = { 0.0f, 0.0f, 0.0f };
    glm::vec3 rot = { 0.0f, 0.0f, 0.0f };
    glm::vec3 scale = { 1.0f, 1.0f, 1.0f };

    TransformComponent() = default;
    TransformComponent(float x, float y, float z) : pos(x, y, z) {};

    glm::mat4 GetRotationMatrix() const;
    glm::mat4 GetModelMatrix() const;

    glm::vec3 GetForward() const;
    glm::vec3 GetRight() const;
    glm::vec3 GetUp() const;

    void Move(const glm::vec3& offset);
    void Rotate(const glm::vec3& rotation);

    void OnUpdate(float deltaTime) override;

    std::unique_ptr<Component> Clone() const override;
};

struct RenderableComponent : Component {
    Mesh* mesh = nullptr;
    Material material = nullptr;

    RenderableComponent() = default;
    RenderableComponent(Mesh* mesh, Material& material) : mesh(mesh), material(material) {};

    void Submit();

    std::unique_ptr<Component> Clone() const override;
};

struct CameraComponent : Component {
    float fov = 45.0f;
    float aspect = 16.0f / 9.0f;
    float nearClip = 0.1f;
    float farClip = 100.0f;

    CameraComponent() = default;
    CameraComponent(float fov, float aspect, float nearClip, float farClip): fov(fov), aspect(aspect), nearClip(nearClip), farClip(farClip) {};

    void Submit();

    std::unique_ptr<Component> Clone() const override;

};