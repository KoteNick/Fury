#include "OceanScene.h"

void OceanScene::Init()
{
    oceanData = {};

    Renderer::Get().AddUBO(UBOslot::Slot8, sizeof(OceanUBOData));

    camera = &CreateEntity("camera")
        .AddComponent(TransformComponent())
        .AddComponent(CameraComponent());

    int size = 9;
    float start = -100 * (size / 2);
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            auto& En = CreateEntity("Ocean")
                .AddComponent(TransformComponent(start + 100 * j, 0, start + 100 * i))
                .AddComponent(RenderableComponent(Assets::GetMesh("Plane100500"), Assets::GetMaterial("ocean")))
                .AddComponent(LevelOfDetailComponent(camera)
                    .AddLOD(75, Assets::GetMesh("Plane100250"))
                    .AddLOD(120, Assets::GetMesh("Plane100")));
        }
    }
	

    sun = &CreateEntity("Sun")
        .AddComponent(TransformComponent(0.0f, 50.0f, -100.0f))
        .AddComponent(DirectionalLightComponent());;

    CreateEntity("Light")
        .AddComponent(TransformComponent(20, 3, 0))
        .AddComponent(PointLightComponent())
        .AddComponent(RenderableComponent(Assets::GetMesh("Sphere"), Assets::GetMaterial("Basic3D")))
        .Transform()->scale *= 0.5;

    sky = &CreateEntity("Sky")
        .AddComponent(TransformComponent())
        .AddComponent(RenderableComponent(Assets::GetMesh("Sphere"), Assets::GetMaterial("sky")));
    sky->Transform()->scale *= 1000;
}

void OceanScene::OnUpdate(float deltaTime)
{
    UpdateEntities(deltaTime);

    sky->Transform()->pos = camera->Transform()->pos;
    sun->Transform()->LookAt({ 0,0,0 });
    SunUBOData sunData = sun->GetComponent<DirectionalLightComponent>()->GetSunUBO(sunSize);
    Renderer::Get().SetUBO(UBOslot::Sun, sunData);

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

    ImGui::Begin("Ocean Settings (FBM)");

    if (ImGui::CollapsingHeader("Colors & Light", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::ColorEdit3("Ambient", &oceanData.Ambient[0]);
        ImGui::ColorEdit3("Diffuse", &oceanData.DiffuseReflectance[0]);
        ImGui::ColorEdit3("Fresnel (Sky)", &oceanData.FresnelColor[0]);
        ImGui::ColorEdit3("Tip Color (Foam)", &oceanData.TipColor[0]);
    }

    if (ImGui::CollapsingHeader("Wave Geometry", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::SliderInt("Octaves (Quality)", &oceanData.WaveCount, 1, 16);
        ImGui::SliderFloat("Global Height", &oceanData.Height, 0.0f, 5.0f);
        ImGui::SliderFloat("Global Speed", &oceanData.InitialSpeed, 0.0f, 10.0f);

        ImGui::SliderFloat("Base Frequency", &oceanData.Frequency, 0.01f, 1.0f);
        ImGui::SliderFloat("Freq Multiplier", &oceanData.FrequencyMult, 1.0f, 3.0f);

        ImGui::SliderFloat("Base Amplitude", &oceanData.Amplitude, 0.1f, 5.0f);
        ImGui::SliderFloat("Amp Multiplier", &oceanData.AmplitudeMult, 0.1f, 1.0f);

        // Это ключевые параметры для "остроты" волн
        ImGui::SliderFloat("Max Peak (Sharpness)", &oceanData.MaxPeak, 0.1f, 3.0f);
        ImGui::SliderFloat("Peak Offset", &oceanData.PeakOffset, 0.0f, 2.0f);
        ImGui::SliderFloat("Drag (Choppiness)", &oceanData.Drag, 0.0f, 0.2f);
    }

    if (ImGui::CollapsingHeader("Material")) {
        ImGui::SliderFloat("Shininess", &oceanData.Shininess, 10.0f, 512.0f);
        ImGui::SliderFloat("Fresnel Power", &oceanData.FresnelShininess, 1.0f, 20.0f);
        ImGui::SliderFloat("Foam Attenuation", &oceanData.TipAttenuation, 1.0f, 20.0f);
        ImGui::SliderFloat("Normal Strength", &oceanData.NormalStrength, 0.0f, 2.0f);
        ImGui::SliderFloat("Glint Size", &glint, 0.2, 1);
    }

    if (ImGui::CollapsingHeader("Athmosphere")) {
        ImGui::ColorEdit3("Sun Color", &sun->GetComponent<DirectionalLightComponent>()->color[0]);
        ImGui::DragFloat3("Sun Pos", &sun->Transform()->pos[0]);
        ImGui::SliderInt("Sun Size", &sunSize, 2, 15);
        ImGui::ColorEdit3("Sky Top", &skyTop[0]);
        ImGui::ColorEdit3("Sky Bottom", &skyBot[0]);
    }
    GetEntity("Sky")->GetComponent<RenderableComponent>()->material.uniform<glm::vec3>("u_TopColor") = skyTop;
    GetEntity("Sky")->GetComponent<RenderableComponent>()->material.uniform<glm::vec3>("u_BottomColor") = skyBot;

    ImGui::End();

    GetEntity("Ocean")->GetComponent<RenderableComponent>()->material.uniform<float>("u_Time") = time;
    GetEntity("Ocean")->GetComponent<RenderableComponent>()->material.uniform<float>("u_GlintSize") = glint;
    Renderer::Get().SetUBO(UBOslot::Slot8, oceanData);

    time += deltaTime;
}