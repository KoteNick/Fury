#include "Component.h"
#include "Entity.h"

void RenderableComponent::Submit()
{
    if (TransformComponent* tc = parent->GetComponent<TransformComponent>()) {
        RenderCall call = { mesh, &material, tc->GetModelMatrix() };
        Renderer::Get().Submit(call);
    }
}

std::unique_ptr<Component> RenderableComponent::Clone() const
{
	return std::make_unique<RenderableComponent>(*this);
}

glm::mat4 TransformComponent::GetRotationMatrix() const {
    glm::mat4 rotation(1.0f);
    rotation = glm::rotate(rotation, glm::radians(rot.y), { 0, 1, 0 });
    rotation = glm::rotate(rotation, glm::radians(rot.x), { 1, 0, 0 });
    rotation = glm::rotate(rotation, glm::radians(rot.z), { 0, 0, 1 });
    return rotation;
}

glm::mat4 TransformComponent::GetModelMatrix() const {
    glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos);
    transform *= GetRotationMatrix();
    transform = glm::scale(transform, scale);
    return transform;
}

glm::vec3 TransformComponent::GetForward() const {
    return glm::normalize(glm::vec3(GetRotationMatrix()[2])) * -1.0f;
}

glm::vec3 TransformComponent::GetRight() const {
    return glm::normalize(glm::vec3(GetRotationMatrix()[0]));
}

glm::vec3 TransformComponent::GetUp() const {
    return glm::normalize(glm::vec3(GetRotationMatrix()[1]));
}

void TransformComponent::Move(const glm::vec3& offset) {
    pos += GetRight() * offset.x + GetUp() * offset.y + GetForward() * offset.z;
}

void TransformComponent::Rotate(const glm::vec3& rotation)
{
    rot += rotation;
}

void TransformComponent::OnUpdate(float deltaTime)
{
    rot.x = std::fmod(rot.x, 360.0f);
    rot.y = std::fmod(rot.y, 360.0f);
    rot.z = std::fmod(rot.z, 360.0f);
}

std::unique_ptr<Component> TransformComponent::Clone() const
{
	return std::make_unique<TransformComponent>(*this);
}

void CameraComponent::Submit() {
    glm::mat4 view, proj, VP;
    proj = glm::perspective(glm::radians(fov), aspect, nearClip, farClip);
    if (TransformComponent* tc = parent->GetComponent<TransformComponent>())
        view = glm::inverse(tc->GetModelMatrix());
    else
        view = glm::mat4(1.0f);
    VP = proj * view;
    Renderer::Get().SetUBO(UBOslot::Camera, VP);
}

std::unique_ptr<Component> CameraComponent::Clone() const
{
    return std::make_unique<CameraComponent>(*this);
}