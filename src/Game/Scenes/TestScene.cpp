#include "TestScene.h"

void TestScene::Init()
{
    camera = &CreateEntity("Camera")
        .AddComponent(TransformComponent(0, 0, 3))
        .AddComponent(CameraComponent());

    Entity& triangle = CreateEntity("Triangle")
        .AddComponent(TransformComponent(-0.5, 0.0, 0.0))
        .AddComponent(RenderableComponent(Assets::GetMesh("Triangle"), Assets::GetMaterial("Basic2D")));

    Entity& cube = CreateEntity("Cube")
        .AddComponent(TransformComponent(0, 0, 0))
        .AddComponent(RenderableComponent(Assets::GetMesh("Cube"), Assets::GetMaterial("light3d")));
    cube.GetComponent<RenderableComponent>()->material.AddTexture(Assets::GetTexture("white"), TextureSlot::Diffuse);

    CreateEntity("Plane")
        .AddComponent(TransformComponent(0, -2, 0))
        .AddComponent(RenderableComponent(Assets::GetMesh("Plane100"), Assets::GetMaterial("light3d")));

    CreateEntity("Sphere")
        .AddComponent(TransformComponent(0, 2, -1))
        .AddComponent(RenderableComponent(Assets::GetMesh("Sphere"), Assets::GetMaterial("light3d")));

    CreateEntity("Light")
        .AddComponent(TransformComponent(0, 3, 0))
        .AddComponent(PointLightComponent())
        .AddComponent(RenderableComponent(Assets::GetMesh("Sphere"), Assets::GetMaterial("Basic3D")))
    .Transform()->scale = glm::vec3(0.1);

    CreateEntity("Sun")
        .AddComponent(TransformComponent(5, 3, 0))
        .AddComponent(DirectionalLightComponent())
        .AddComponent(RenderableComponent(Assets::GetMesh("Sphere"), Assets::GetMaterial("Basic3D")))
    .GetComponent<RenderableComponent>()->material.uniform<glm::vec4>("u_Color") = { 1.0, 0.87, 0.13, 1. };

    Entity& triangle2 = CreateEntity("Triangle2") = triangle;
    triangle2.GetComponent<TransformComponent>()->pos.z = 1;
    triangle2.GetComponent<TransformComponent>()->rot = { 0.2, 0.5, 0.7 };

    triangle.GetComponent<RenderableComponent>()->material.uniform<glm::vec4>("u_Color") = glm::vec4(1, 0, 1, 0.5);

    GetEntity("Sphere")->GetComponent<RenderableComponent>()->material.uniform<float>("u_Shininess") = 128;
}

void TestScene::OnUpdate(float deltaTime)
{
    UpdateEntities(deltaTime);

    auto& sun = *GetEntity("Sun");
    auto& light = *GetEntity("Light");
    auto& cube = *GetEntity("Cube");

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

    float speed = 5.0f * deltaTime;

    if (!ImGui::GetIO().WantCaptureKeyboard) {
        glm::vec3 move(0);

        if (Input::IsKeyPressed(Key::LeftShift)) speed = 10.f * deltaTime;

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

    auto tr = cube.Transform();
    if (tr->pos.x >= 1.5)
        tr->pos.x = -1.5;
    tr->pos.x += 1 * deltaTime;
    tr->rot += glm::vec3(45) * deltaTime;
}
