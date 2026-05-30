#include "TerrainScene.h"

void TerrainScene::Init()
{
    terrainSettings = {};

    camera = &CreateEntity("camera")
        .AddComponent(TransformComponent(0, 1, 3))
        .AddComponent(CameraComponent());

    auto& terr = CreateEntity("terrain")
        .AddComponent(TransformComponent(0, 0, 0))
        .AddComponent(RenderableComponent(Assets::GetMesh("Plane100500"), Assets::GetMaterial("terrain")));

    Renderer::Get().AddUBO(UBOslot::Custom1, sizeof(TerrainUBOData));

    sky = &CreateEntity("Sky")
        .AddComponent(TransformComponent())
        .AddComponent(RenderableComponent(Assets::GetMesh("Sphere"), Assets::GetMaterial("sky")));
    sky->Transform()->scale *= -500;
}

void TerrainScene::OnUpdate(float deltaTime)
{
    UpdateEntities(deltaTime);

    Renderer::Get().SetUBO(UBOslot::Custom1, terrainSettings);

    GetEntity("Sky")->Transform()->pos = camera->Transform()->pos;

    float speed = 15.0f * deltaTime;

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

    ImGui::Begin("Terrain Generator");

    if (ImGui::CollapsingHeader("Base Settings", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::SliderFloat("Scale (Zoom)", &terrainSettings.Scale, 10.0f, 400.0f);
        ImGui::SliderFloat("Height", &terrainSettings.TerrainHeight, 1.0f, 200.0f);
        ImGui::DragFloat3("Offset", &terrainSettings.Offset[0], 0.1f);
        ImGui::SliderFloat("Seed", &terrainSettings.Seed, 0.0f, 1000.0f);
    }

    if (ImGui::CollapsingHeader("Noise & FBM")) {
        ImGui::SliderFloat("Octaves", &terrainSettings.Octaves, 1.0f, 16.0f, "%.0f");
        ImGui::SliderFloat("Initial Amplitude", &terrainSettings.InitialAmplitude, 0.01f, 2.0f);
        ImGui::SliderFloat("Amplitude Decay", &terrainSettings.AmplitudeDecay, 0.01f, 1.0f);
        ImGui::SliderFloat("Lacunarity", &terrainSettings.Lacunarity, 0.5f, 3.0f);
    }

    if (ImGui::CollapsingHeader("Warping (Variance)")) {
        ImGui::SliderFloat("Gradient Rotation", &terrainSettings.GradientRotation, -180.0f, 180.0f);
        ImGui::SliderFloat("Noise Rotation", &terrainSettings.NoiseRotation, -180.0f, 180.0f);
        ImGui::DragFloat2("Angular Variance", &terrainSettings.AngularVariance[0], 0.1f, -180.0f, 180.0f);
        ImGui::DragFloat2("Freq Variance", &terrainSettings.FrequencyVarianceLowerBound, 0.01f, -2.0f, 2.0f);
    }

    if (ImGui::CollapsingHeader("Colors & Slope")) {
        ImGui::ColorEdit3("Low Slope (Grass)", &terrainSettings.LowSlopeColor[0]);
        ImGui::ColorEdit3("High Slope (Rock)", &terrainSettings.HighSlopeColor[0]);

        // x = min, y = max
        ImGui::SliderFloat2("Slope Threshold", &terrainSettings.SlopeRange[0], 0.0f, 1.0f);
        ImGui::SliderFloat("Slope Damping", &terrainSettings.SlopeDamping, 0.01f, 1.0f);
    }
    //terrainSettings.LightDirection = GetEntity("Sun")->Transform()->GetForward();
    if (ImGui::CollapsingHeader("Lighting")) {
        ImGui::SliderFloat3("Light Dir", &terrainSettings.LightDirection[0], -1.0f, 1.0f);
        ImGui::ColorEdit3("Ambient", &terrainSettings.AmbientLight[0]);
    }
    sky->GetComponent<RenderableComponent>()->material.uniform<glm::vec3>("u_LightDir") = -terrainSettings.LightDirection;

    ImGui::End();
}
