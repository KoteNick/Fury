#include "FieldScene.h"

#include "FastNoiseLite.h"

void FieldScene::GenerateHeightmapTexture(int width, int height, int seed)
{
    FastNoiseLite noise;
    noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    noise.SetFrequency(0.01f);
    noise.SetFractalType(FastNoiseLite::FractalType_FBm);
    noise.SetFractalOctaves(2);
    noise.SetSeed(seed);

    std::vector<unsigned char> pixels(width * height);

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            float noiseVal = noise.GetNoise((float)x, (float)y);

            unsigned char color = static_cast<unsigned char>((noiseVal + 1.0f) * 0.5f * 255.0f);

            pixels[y * width + x] = color;
        }
    }

    heightmap = std::make_unique<Texture>(512, 512, pixels.data(), GL_RED, GL_RED);
}

void FieldScene::Init()
{
    GenerateHeightmapTexture(512, 512, static_cast<int>(time));

    camera = &CreateEntity("camera")
        .AddComponent(TransformComponent(0, 1, 3))
        .AddComponent(CameraComponent());

    auto& field = CreateEntity("field")
        .AddComponent(TransformComponent(0, 0, 0))
        .AddComponent(RenderableComponent(Assets::GetMesh("Plane100250"), Assets::GetMaterial("plains")));
    field.GetComponent<RenderableComponent>()->material.AddTexture(heightmap.get(), TextureSlot::HeightMap);
    field.GetComponent<RenderableComponent>()->material.uniform<float>("u_HeightScale") = 5;

    auto& sun = CreateEntity("Sun")
        .AddComponent(TransformComponent(50, 50, 0))
        .AddComponent(DirectionalLightComponent())
        .AddComponent(RenderableComponent(Assets::GetMesh("Sphere"), Assets::GetMaterial("Basic3D")));
    sun.GetComponent<RenderableComponent>()->material.uniform<glm::vec4>("u_Color") = { 1.0, 0.87, 0.13, 1. };
    sun.Transform()->LookAt(glm::vec3(0.0f, 0.0f, 0.0f));

    /*CreateEntity("Light")
        .AddComponent(TransformComponent(0, 3, 0))
        .AddComponent(PointLightComponent())
        .AddComponent(RenderableComponent(Assets::GetMesh("Sphere"), Assets::GetMaterial("Basic3D")))
        .Transform()->scale = glm::vec3(0.1);*/
}

void FieldScene::OnUpdate(float deltaTime)
{
    time += deltaTime;
    UpdateEntities(deltaTime);

    GetEntity("field")->GetComponent<RenderableComponent>()->material.uniform<glm::vec4>("u_Color") = glm::vec4(fieldColor, 1);

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
}
