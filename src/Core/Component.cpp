#include "Component.h"
#include "Entity.h"

void RenderableComponent::Submit()
{
    if (TransformComponent* tc = parent->GetComponent<TransformComponent>()) {
        Renderer::Get().Submit({ mesh, &material, tc->GetModelMatrix() });
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

void TransformComponent::LookAt(const glm::vec3& target)
{
    glm::vec3 direction = glm::normalize(target - pos);

    float pitch = asin(direction.y);

    float yaw = atan2(-direction.x, -direction.z);

    rot.x = glm::degrees(pitch);
    rot.y = glm::degrees(yaw);

    rot.z = 0.0f;
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
    CameraUBOData data;
    glm::mat4 view, proj;
    proj = glm::perspective(glm::radians(fov), aspect, nearClip, farClip);
    if (TransformComponent* tc = parent->GetComponent<TransformComponent>()) {
        view = glm::inverse(tc->GetModelMatrix());
        data.viewPos = glm::vec4(tc->pos, 0);
    }
    else {
        view = glm::mat4(1.0f);
    }
    data.projectionView = proj * view;
    Renderer::Get().SetUBO(UBOslot::Camera, data);
}

std::unique_ptr<Component> CameraComponent::Clone() const
{
    return std::make_unique<CameraComponent>(*this);
}

LightData PointLightComponent::GetLightData()
{
    return { glm::vec4(parent->Transform()->pos, radius), glm::vec4(color, intensity) };
}

std::unique_ptr<Component> PointLightComponent::Clone() const
{
    return std::make_unique<PointLightComponent>(*this);
}

LightData DirectionalLightComponent::GetLightData() const
{
    return { glm::vec4(parent->Transform()->GetForward(), -1.f), glm::vec4(color, intensity)};
}

glm::vec3 DirectionalLightComponent::GetDirection() const
{
    return parent ? parent->Transform()->GetForward() : glm::vec3(0, -1, 0);
}

glm::mat4 DirectionalLightComponent::GetLightSpaceMatrix(glm::vec3 target) const
{
    if (TransformComponent* tc = parent->GetComponent<TransformComponent>())
        return 
            glm::ortho(-orthoSize, orthoSize, -orthoSize, orthoSize, nearPlane, farPlane)
            * 
            glm::lookAt(tc->pos, target, tc->GetUp());
    return glm::mat4();
}

SunUBOData DirectionalLightComponent::GetSunUBO(unsigned int size) const
{
    SunUBOData ubo = {};
    float safeSize = (size == 0) ? 1.0f : (float)size;
    float sunCosThreshold = 1.0f - 0.5f / (10.0f * safeSize);
    ubo.DirectionIntensity = glm::vec4(GetDirection(), intensity);
    ubo.DiskColorAndSize = glm::vec4(color, sunCosThreshold); 
    ubo.LightColor = ubo.SpecularColor = glm::vec4(color, 1.0f);

    return ubo;
}

std::unique_ptr<Component> DirectionalLightComponent::Clone() const
{
    return std::make_unique<DirectionalLightComponent>();
}
