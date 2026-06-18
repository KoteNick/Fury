#include "SolarScene.h"
#include "Core/ScopeTimer.h"

void SolarScene::LoadAssets()
{
    if (!assetsLoaded) {
        ScopeTimer t("SolarScene textures loading");

        Assets::AddTexture("Stars", Texture("assets/textures/8k_stars_milky_way.jpg"));
        Assets::AddTexture("Earth", Texture("assets/textures/8k_earth_daymap.jpg"));
        Assets::AddTexture("EarthNight", Texture("assets/textures/8k_earth_nightmap.jpg"));
        Assets::AddTexture("EarthSpec", Texture("assets/textures/8k_earth_specular_map.jpg"));
        Assets::AddTexture("EarthNormal", Texture("assets/textures/8k_earth_normal_map.jpg"));
        Assets::AddTexture("EarthClouds", Texture("assets/textures/8k_earth_clouds.jpg"));
        Assets::AddTexture("Sun", Texture("assets/textures/8k_sun.jpg"));
        Assets::AddTexture("Moon", Texture("assets/textures/8k_moon.jpg"));
        Assets::AddTexture("Mars", Texture("assets/textures/2k_mars.jpg"));
        Assets::AddTexture("Venus", Texture("assets/textures/2k_venus_surface.jpg"));
        Assets::AddTexture("VenusAtmosphere", Texture("assets/textures/2k_venus_atmosphere.jpg"));
        Assets::AddTexture("Jupiter", Texture("assets/textures/2k_jupiter.jpg"));
        Assets::AddTexture("Mercury", Texture("assets/textures/2k_mercury.jpg"));
        Assets::AddTexture("Uranus", Texture("assets/textures/2k_uranus.jpg"));
        Assets::AddTexture("Neptune", Texture("assets/textures/2k_neptune.jpg"));
        Assets::AddTexture("Saturn", Texture("assets/textures/2k_saturn.jpg"));
        Assets::AddTexture("SaturnRing", Texture("assets/textures/2k_saturn_ring_alpha.png"));
    }
    assetsLoaded = true;
}

SolarObject& SolarScene::CreateSolarObject(const std::string& name)
{
    objects.push_back(SolarObject());
    SolarObject& obj = objects.back();
    obj.name = name;
    return obj;
}

SolarObject& SolarScene::CreateSolarObject(const std::string& name, SolarObject* parent, float distance, float orbitSpeed, float rotationSpeed, float tilt, float scale)
{
    objects.push_back(SolarObject());
    SolarObject& obj = objects.back();

    obj.name = name;
    obj.parent = parent;
    obj.orbitDistance = distance;
    obj.orbitSpeed = orbitSpeed;
    obj.rotationSpeed = rotationSpeed;
    obj.axialTilt = tilt;

    obj.entity = &CreateEntity(name).AddComponent(TransformComponent());
    obj.entity->GetComponent<TransformComponent>()->scale = glm::vec3(scale);

    return obj;
}

void SolarScene::Init()
{
    LoadAssets();
    objects.reserve(20);

    Entity& stars = CreateEntity("Stars")
        .AddComponent(TransformComponent())
        .AddComponent(RenderableComponent(Assets::GetMesh("Sphere"), Assets::GetMaterial("Basic3D")));
    stars.Transform()->scale *= 900.f;
    stars.GetComponent<RenderableComponent>()->material.AddTexture(Assets::GetTexture("Stars"), TextureSlot::Diffuse);

    camera = &CreateEntity("camera")
        .AddComponent(TransformComponent())
        .AddComponent(CameraComponent());

    SolarObject& sun = CreateSolarObject("Sun", nullptr, 0.0f, 0.0f, 14.0f, 0.0f, 8.0f);
    sun.entity->AddComponent(PointLightComponent())
        .GetComponent<PointLightComponent>()->radius = 99999.f;
    sun.entity->AddComponent(RenderableComponent(Assets::GetMesh("Sphere"), Assets::GetMaterial("Basic3D")))
        .GetComponent<RenderableComponent>()->material
            .AddTexture(Assets::GetTexture("Sun"), TextureSlot::Diffuse);

    SolarObject& earth = CreateSolarObject("Earth", &sun, 24.0f, 1.0f, 360.0f, 23.5f, 1.0f);
    earth.entity->AddComponent(RenderableComponent(Assets::GetMesh("Sphere"), Assets::GetMaterial("planet")));
    earth.entity->GetComponent<RenderableComponent>()->material
        .AddTexture(Assets::GetTexture("Earth"), TextureSlot::Diffuse)
        .AddTexture(Assets::GetTexture("EarthNight"), TextureSlot::Emission)
        .AddTexture(Assets::GetTexture("EarthSpec"), TextureSlot::Specular)
        .AddTexture(Assets::GetTexture("EarthNormal"), TextureSlot::Normal)
        .uniform<float>("u_Shininess") = 64.0f;

    auto AddTexture = [&](SolarObject& obj, const std::string& texName) {
        obj.entity->AddComponent(RenderableComponent(Assets::GetMesh("Sphere"), Assets::GetMaterial("planet")));
        Texture* tex = Assets::GetTexture(texName);
        if (!tex)
            tex = Assets::GetTexture("white");
        obj.entity->GetComponent<RenderableComponent>()->material
            .AddTexture(tex, TextureSlot::Diffuse)
            .uniform<float>("u_Shininess") = 8.0f;
        };

    SolarObject& earthClouds = CreateSolarObject("EarthClouds", &earth, 0.0f, 0.0f, 380.0f, 23.5f, 1.02f);
    AddTexture(earthClouds, "EarthClouds");
    earthClouds.entity->GetComponent<RenderableComponent>()->material.uniform<int>("u_IsAtmosphere") = 1;
    //earthClouds.entity->GetComponent<RenderableComponent>()->material.uniform<glm::vec4>("u_Color") = { 1,1,1,0.5f };

    SolarObject& moon = CreateSolarObject("Moon", &earth, 3.5f, 13.3f, 13.3f, 1.5f, 0.27f);
    moon.orbitInclination = 5.1f;
    AddTexture(moon, "Moon");

    SolarObject& mercury = CreateSolarObject("Mercury", &sun, 10.0f, 4.1f, 6.0f, 0.0f, 0.4f);
    AddTexture(mercury, "Mercury");

    SolarObject& venus = CreateSolarObject("Venus", &sun, 16.0f, 1.6f, -1.5f, 177.3f, 0.95f);
    AddTexture(venus, "Venus");
    SolarObject& venusAtmosphere = CreateSolarObject("VenusAtmosphere", &venus, 0.0f, 0.0f, -2.0f, 177.3f, 1.02f);
    AddTexture(venusAtmosphere, "VenusAtmosphere");
    venusAtmosphere.entity->GetComponent<RenderableComponent>()->material.uniform<int>("u_IsAtmosphere") = 1;
    venusAtmosphere.entity->GetComponent<RenderableComponent>()->material.uniform<glm::vec4>("u_Color") = { 1,1,1, 0.5f };

    SolarObject& mars = CreateSolarObject("Mars", &sun, 32.0f, 0.5f, 350.0f, 25.1f, 0.53f);
    AddTexture(mars, "Mars");

    SolarObject& jupiter = CreateSolarObject("Jupiter", &sun, 50.0f, 0.2f, 860.0f, 3.1f, 3.5f);
    AddTexture(jupiter, "Jupiter");
    jupiter.entity->Transform()->scale = glm::vec3(3.5f, 3.3f, 3.5f);

    SolarObject& saturn = CreateSolarObject("Saturn", &sun, 65.0f, 0.1f, 800.0f, 26.7f, 3.0f);
    AddTexture(saturn, "Saturn");
    saturn.entity->Transform()->scale = glm::vec3(3.0f, 2.7f, 3.0f);
    SolarObject& saturnRings = CreateSolarObject("SaturnRings", &saturn, 0.0f, 0.0f, 800.0f, 26.7f, 6.5f);
    saturnRings.entity->AddComponent(RenderableComponent(Assets::GetMesh("Plane"), Assets::GetMaterial("ring")));
    saturnRings.entity->GetComponent<RenderableComponent>()->material
        .AddTexture(Assets::GetTexture("SaturnRing"), TextureSlot::Diffuse);

    SolarObject& uranus = CreateSolarObject("Uranus", &sun, 80.0f, 0.05f, -500.0f, 97.7f, 2.0f);
    AddTexture(uranus, "Uranus");

    SolarObject& neptune = CreateSolarObject("Neptune", &sun, 95.0f, 0.03f, 530.0f, 28.3f, 2.0f);
    AddTexture(neptune, "Neptune");
}

void SolarScene::OnUpdate(float deltaTime)
{
    GetEntity("Stars")->Transform()->pos = camera->Transform()->pos;
    float scaledDt = deltaTime * timeScale;

    for (auto& obj : objects)
    {
        auto* tc = obj.entity->Transform();
        if (!tc) continue;

        obj.currentOrbitAngle += obj.orbitSpeed * scaledDt;
        obj.currentRotationAngle += obj.rotationSpeed * scaledDt;

        if (obj.currentOrbitAngle > 360.0f) obj.currentOrbitAngle -= 360.0f;
        if (obj.currentRotationAngle > 360.0f) obj.currentRotationAngle -= 360.0f;

        float radOrbit = glm::radians(obj.currentOrbitAngle);
        float localX = obj.orbitDistance * cosf(radOrbit);
        float localZ = obj.orbitDistance * sinf(radOrbit);
        float localY = 0.0f;

        float radInclination = glm::radians(obj.orbitInclination);
        glm::vec3 relativePos;
        relativePos.x = localX * cosf(radInclination) - localY * sinf(radInclination);
        relativePos.y = localX * sinf(radInclination) + localY * cosf(radInclination);
        relativePos.z = localZ;

        if (obj.parent != nullptr) {
            obj.worldPosition = obj.parent->worldPosition + relativePos;
        }
        else {
            obj.worldPosition = relativePos;
        }

        tc->pos = obj.worldPosition;

        tc->rot.y = obj.currentRotationAngle;
        tc->rot.z = obj.axialTilt;
    }

    if (debugUI) {
        ImGui::Begin("Solar");
        ImGui::DragFloat3("Earth Pos", &GetEntity("Earth")->Transform()->pos[0], 0.1f);
        const char* items[] = {
            "1 Hour / sec",
            "1 Day / sec",
            "1 Week / sec",
            "1 Month / sec",
            "1 Year / sec",
            "10 Years / sec"
        };

        ImGui::Combo("Speed", &timeScaleMode, items, IM_ARRAYSIZE(items));

        switch (timeScaleMode) {
        case 0: timeScale = 1.0f / 24.0f; break;
        case 1: timeScale = 1.0f; break;
        case 2: timeScale = 7.0f; break;
        case 3: timeScale = 30.0f; break;
        case 4: timeScale = 365.25f; break;
        case 5: timeScale = 3652.5f; break;
        }

        ImGui::Text("Current Time Scale: %.2f Days/Sec", timeScale);
        ImGui::ColorEdit3("Ambient", &ambient[0]);
        if (ImGui::Button("Free Camera")) {
            trackedObj = nullptr;
        }

        ImGui::Separator();
        ImGui::Text("Track Planet:");

        for (auto& obj : objects)
        {
            bool isCurrent = (trackedObj == &obj);
            if (isCurrent) {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.7f, 0.2f, 1.0f));
            }

            if (ImGui::Button(obj.name.c_str())) {
                trackedObj = &obj;
            }

            if (isCurrent) {
                ImGui::PopStyleColor();
            }
        }

        if (trackedObj != nullptr) {
            ImGui::Separator();
            ImGui::Text("Orbital Camera Controls");

            ImGui::SliderFloat("Radius", &trackRadius, 1.5f, 50.0f);

            ImGui::SliderFloat("Yaw", &trackYaw, 0.0f, 360.0f);

            ImGui::SliderFloat("Pitch", &trackPitch, -89.0f, 89.0f);
        }

        ImGui::End();
    }

    if (trackedObj != nullptr)
    {
        if (Input::IsMouseDown(MouseButton::Right)) {
            glm::vec2 delta = Input::GetMouseDelta();

            float sensitivity = 0.5f;

            trackYaw += delta.x * sensitivity;
            trackPitch -= delta.y * sensitivity;

            if (trackPitch > 89.0f) trackPitch = 89.0f;
            if (trackPitch < -89.0f) trackPitch = -89.0f;

            trackRadius -= Input::GetScrollDelta() * 60.f * deltaTime;
        }

        auto* camTransform = camera->GetComponent<TransformComponent>();
        if (camTransform)
        {
            float sizeMultiplier = trackedObj->entity->Transform()->scale.x;
            float actualRadius = trackRadius * sizeMultiplier;

            float pitchRad = glm::radians(trackPitch);
            float yawRad = glm::radians(trackYaw);

            glm::vec3 offset;
            offset.x = actualRadius * cos(pitchRad) * sin(yawRad);
            offset.y = actualRadius * sin(pitchRad);
            offset.z = actualRadius * cos(pitchRad) * cos(yawRad);

            camTransform->pos = trackedObj->worldPosition + offset;
            camTransform->LookAt(trackedObj->worldPosition);
        }
    }
    else
        DebugCameraMovement(deltaTime);
}

void SolarScene::Destroy()
{
    objects.clear();
    trackedObj = nullptr;
    BaseScene::Destroy();
}
