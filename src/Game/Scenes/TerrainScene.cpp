#include "TerrainScene.h"

void TerrainScene::Init()
{
    terrainSettings = {};
    sunData = {};
    sunData.DiskColorAndSize.w = 0.995;

    camera = &CreateEntity("camera")
        .AddComponent(TransformComponent(0, 1, 3))
        .AddComponent(CameraComponent());

    auto& terr = CreateEntity("terrain")
        .AddComponent(TransformComponent(0, 0, 0))
        .AddComponent(RenderableComponent(Assets::GetMesh("Plane200x600"), Assets::GetMaterial("terrain")));

    Renderer::Get().AddUBO(UBOslot::Slot8, sizeof(TerrainUBOData));

    sky = &CreateEntity("Sky")
        .AddComponent(TransformComponent())
        .AddComponent(RenderableComponent(Assets::GetMesh("Sphere"), Assets::GetMaterial("sky")));
    sky->Transform()->scale *= 500;
}

void TerrainScene::OnUpdate(float deltaTime)
{
    UpdateEntities(deltaTime);

    Renderer::Get().SetUBO(UBOslot::Sun, sunData);
    Renderer::Get().SetUBO(UBOslot::Slot8, terrainSettings);

    GetEntity("Sky")->Transform()->pos = camera->Transform()->pos;

    DebugCameraMovement(deltaTime, 25, 2);

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
        ImGui::SliderFloat("Amplitude Decay", &terrainSettings.AmplitudeDecay, 0.01f, 0.5f);
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

    if (ImGui::CollapsingHeader("Lighting")) {
        ImGui::SliderFloat3("Light Dir", &sunData.DirectionIntensity[0], -1.0f, 1.0f);
        ImGui::ColorEdit3("Ambient", &terrainSettings.AmbientLight[0]);
    }

    ImGui::End();
}
