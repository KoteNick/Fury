#include "SolarScene.h"

void SolarScene::Init()
{
    camera = &CreateEntity("camera")
        .AddComponent(TransformComponent())
        .AddComponent(CameraComponent());

    CreateEntity("Earth")
        .AddComponent(TransformComponent(0, 0, -3))
        .AddComponent(RenderableComponent(Assets::GetMesh("Sphere"), Assets::GetMaterial("planet")))
        .GetComponent<RenderableComponent>()->material
            .AddTexture(Assets::GetTexture("Earth"), TextureSlot::Diffuse)
            .AddTexture(Assets::GetTexture("EarthNight"), TextureSlot::Emission)
            .AddTexture(Assets::GetTexture("EarthSpec"), TextureSlot::Specular)
            .AddTexture(Assets::GetTexture("EarthNormal"), TextureSlot::Normal)
            .uniform<float>("u_Shininess") = 128;

    auto& sun = CreateEntity("Sun")
        .AddComponent(TransformComponent(0, 0, 0))
        .AddComponent(PointLightComponent())
        .AddComponent(RenderableComponent(Assets::GetMesh("Sphere"), Assets::GetMaterial("Basic3D")));
    sun.GetComponent<RenderableComponent>()->material.uniform<glm::vec4>("u_Color") = { 1.0, 0.87, 0.13, 1. };
    sun.GetComponent<PointLightComponent>()->radius = 9999.f;
}

void SolarScene::OnUpdate(float deltaTime)
{

    if (debugUI) {
        ImGui::Begin("Solar");
        ImGui::DragFloat3("Earth Pos", &GetEntity("Earth")->Transform()->pos[0], 0.1f);
        ImGui::End();
    }

    GetEntity("Earth")->Transform()->Rotate({ 0, 45.f * deltaTime, 0 } );

    float speed = 15.0f * deltaTime;


    if (!ImGui::GetIO().WantCaptureKeyboard) {
        glm::vec3 move(0);

        if (Input::IsKeyPressed(Key::LeftShift)) speed = 50.f * deltaTime;

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

        float sensitivity = 0.25f;

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