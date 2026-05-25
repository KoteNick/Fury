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
        .AddUBO(UBOslot::Camera, sizeof(glm::mat4));

    Assets::Init();

    camera
        .AddComponent(TransformComponent(0, 0, 3))
        .AddComponent(CameraComponent());

    triangle
        .AddComponent(TransformComponent(-0.5, 0.0, 0.0))
        .AddComponent(RenderableComponent(Assets::GetMesh("Triangle"), Assets::GetMaterial("Basic2D")));

    cube
        .AddComponent(TransformComponent(0, 0, 0))
        .AddComponent(RenderableComponent(Assets::GetMesh("Cube"), Assets::GetMaterial("Basic3D")));

    triangle2 = triangle;
    triangle2.GetComponent<TransformComponent>()->pos.z = 1;
    triangle2.GetComponent<TransformComponent>()->rot = { 0.2, 0.5, 0.7 };

    triangle.GetComponent<RenderableComponent>()->material.uniform<glm::vec4>("u_Color") = glm::vec4(1, 0, 1, 0.5);
}

void Game::OnUpdate(float deltaTime) {
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

    auto tr = cube.Transform();
    if (tr->pos.x >= 1.5)
        tr->pos.x = -1.5;
    tr->pos.x += 1 * deltaTime;
    tr->rot += glm::vec3(45) * deltaTime;

    ImGui::Begin("Test");
    ImGui::DragFloat3("Camera pos", &camera.Transform()->pos.x);
    ImGui::DragFloat3("Camera rot", &camera.Transform()->rot.x);
    ImGui::End();

    Renderer::Get().Flush();
}

void Game::Close()
{
    Assets::Close();
}
