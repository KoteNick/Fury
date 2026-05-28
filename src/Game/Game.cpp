#include "Game.h"
#include "Assets.h"

#include "imgui/imgui.h"

#include "Renderer/Renderer.h"
#include "Renderer/Vertex.h"

#include "Core/Entity.h"
#include "Core/Input.h"

Game* Game::instance = nullptr;

void Game::Init() {
    Renderer::Get()
        .AddUBO(UBOslot::Camera, sizeof(CameraUBOData))
        .AddUBO(UBOslot::Ambient, sizeof(glm::vec4))
        .AddUBO(UBOslot::Lights, sizeof(LightsUBOData));

    Assets::Init();

    camera
        .AddComponent(TransformComponent(0, 0, 3))
        .AddComponent(CameraComponent());

    triangle
        .AddComponent(TransformComponent(-0.5, 0.0, 0.0))
        .AddComponent(RenderableComponent(Assets::GetMesh("Triangle"), Assets::GetMaterial("Basic2D")));

    cube
        .AddComponent(TransformComponent(0, 0, 0))
        .AddComponent(RenderableComponent(Assets::GetMesh("Cube"), Assets::GetMaterial("light3d")));

    plane
        .AddComponent(TransformComponent(0, -2, 0))
        .AddComponent(RenderableComponent(Assets::GetMesh("Plane"), Assets::GetMaterial("light3d")));
    plane.Transform()->scale *= 10;

    sphere
        .AddComponent(TransformComponent(0, 2, -1))
        .AddComponent(RenderableComponent(Assets::GetMesh("Sphere"), Assets::GetMaterial("light3d")));

    light
        .AddComponent(TransformComponent(0, 3, 0))
        .AddComponent(PointLightComponent())
        .AddComponent(RenderableComponent(Assets::GetMesh("Sphere"), Assets::GetMaterial("Basic3D")));
    light.Transform()->scale = glm::vec3(0.1);

    sun
        .AddComponent(TransformComponent(5, 3, 0))
        .AddComponent(DirectionalLightComponent())
        .AddComponent(RenderableComponent(Assets::GetMesh("Sphere"), Assets::GetMaterial("Basic3D")));
    sun.GetComponent<RenderableComponent>()->material.uniform<glm::vec4>("u_Color") = { 1.0, 0.87, 0.13, 1. };

    triangle2 = triangle;
    triangle2.GetComponent<TransformComponent>()->pos.z = 1;
    triangle2.GetComponent<TransformComponent>()->rot = { 0.2, 0.5, 0.7 };

    triangle.GetComponent<RenderableComponent>()->material.uniform<glm::vec4>("u_Color") = glm::vec4(1, 0, 1, 0.5);

    sphere.GetComponent<RenderableComponent>()->material.uniform<float>("u_Shininess") = 128;

    ambient = { 0.05f, 0.05f, 0.1f, 1.0f };
}

void Game::OnUpdate(float deltaTime) {
    Renderer::Get().SetViewport(windowSize.width, windowSize.height);
    Renderer::Get().SetUBO(UBOslot::Ambient, ambient);
    camera.GetComponent<CameraComponent>()->aspect = static_cast<float>(windowSize.width) / windowSize.height;

    static float orbitTime = 0.0f;
    orbitTime += deltaTime * 0.5f;

    float radius = 6.0f;
    float height = 5.0f;

    glm::vec3 newPos;
    newPos.x = cos(orbitTime) * radius;
    newPos.y = height;
    newPos.z = sin(orbitTime) * radius;

    sun.Transform()->pos = newPos;
    sun.Transform()->LookAt(glm::vec3(0.0f, 0.0f, 0.0f));
    sun.Transform()->OnUpdate(deltaTime);

    LightsUBOData lightData;
    lightData.count = 1;
    //lightData.lights[0] = light.GetComponent<PointLightComponent>()->GetLightData();
    lightData.lights[0] = sun.GetComponent<DirectionalLightComponent>()->GetLightData();
    Renderer::Get().SetUBO(UBOslot::Lights, lightData);

    float speed = 5.0f * deltaTime;

    if (!ImGui::GetIO().WantCaptureKeyboard) {
        glm::vec3 move(0);

        if (Input::IsKeyPressed(Key::W)) move.z += speed;
        if (Input::IsKeyPressed(Key::S)) move.z -= speed;
        if (Input::IsKeyPressed(Key::A)) move.x -= speed;
        if (Input::IsKeyPressed(Key::D)) move.x += speed;

        if (Input::IsKeyPressed(Key::Space)) camera.Transform()->pos.y += speed;
        if (Input::IsKeyPressed(Key::C)) camera.Transform()->pos.y -= speed;

        camera.Transform()->Move(move);
    }

    if (Input::IsMouseDown(MouseButton::Right) && !ImGui::GetIO().WantCaptureMouse) {
        glm::vec2 delta = Input::GetMouseDelta();

        float sensitivity = 0.25f;

        camera.Transform()->rot.y -= delta.x * sensitivity;
        camera.Transform()->rot.x -= delta.y * sensitivity;

        if (camera.Transform()->rot.x > 90.f)  camera.Transform()->rot.x = 90.f;
        if (camera.Transform()->rot.x < -90.f) camera.Transform()->rot.x = -90.f;

        float scroll = Input::GetScrollDelta();
        if (scroll != 0.0f) {
            camera.GetComponent<CameraComponent>()->fov -= scroll * 5.0f;
        }
    }

    camera.GetComponent<CameraComponent>()->Submit();
    triangle2.GetComponent<RenderableComponent>()->Submit();
    triangle.GetComponent<RenderableComponent>()->Submit();
    cube.GetComponent<RenderableComponent>()->Submit();
    plane.GetComponent<RenderableComponent>()->Submit();
    sphere.GetComponent<RenderableComponent>()->Submit();
    light.GetComponent<RenderableComponent>()->Submit();
    sun.GetComponent<RenderableComponent>()->Submit();

    auto tr = cube.Transform();
    if (tr->pos.x >= 1.5)
        tr->pos.x = -1.5;
    tr->pos.x += 1 * deltaTime;
    tr->rot += glm::vec3(45) * deltaTime;

    ImGui::Begin("Test");
    ImGui::DragFloat3("Camera pos", &camera.Transform()->pos.x);
    ImGui::DragFloat3("Camera rot", &camera.Transform()->rot.x);
    ImGui::DragFloat3("LightPos", &light.Transform()->pos.x, 0.1);
    ImGui::ColorPicker3("AmbientColor", &ambient.r);
    ImGui::ColorPicker3("LightColor", &sun.GetComponent<DirectionalLightComponent>()->color.x);
    ImGui::End();

    Renderer::Get().Flush(Assets::GetShader("shadow_map"), sun.GetComponent<DirectionalLightComponent>()->GetLightSpaceMatrix());
}

void Game::Close()
{
    Assets::Close();
}
