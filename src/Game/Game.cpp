#include "Game.h"
#include "Assets.h"

#include "imgui/imgui.h"

#include "Renderer/Renderer.h"
#include "Renderer/Vertex.h"

#include "Core/Entity.h"
#include "Core/Input.h"

Game* Game::instance = nullptr;
static bool wireframeMode = false;

void Game::Init() {
    Renderer::Get()
        .AddUBO(UBOslot::Camera, sizeof(CameraUBOData))
        .AddUBO(UBOslot::Ambient, sizeof(glm::vec4))
        .AddUBO(UBOslot::Lights, sizeof(LightsUBOData))
        .AddUBO(UBOslot::Sun, sizeof(SunUBOData));

    Assets::Init();

    currentScene = new OceanScene();
    //currentScene = new TerrainScene();
    currentScene->Init();
}

void Game::OnUpdate(float deltaTime) {
    Renderer::Get().SetViewport(windowSize.width, windowSize.height);
    if (currentScene) {
        if (currentScene->camera)
            currentScene->camera->GetComponent<CameraComponent>()->aspect = static_cast<float>(windowSize.width) / windowSize.height;

        currentScene->OnUpdate(deltaTime);
    }

    ImGui::Begin("Test");
    if (ImGui::Checkbox("Wireframe Mode", &wireframeMode))
    {
        if (wireframeMode) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
        else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
    }
    /*ImGui::DragFloat3("Camera pos", &camera.Transform()->pos.x);
    ImGui::DragFloat3("Camera rot", &camera.Transform()->rot.x);
    ImGui::DragFloat3("LightPos", &light.Transform()->pos.x, 0.1);
    ImGui::ColorPicker3("AmbientColor", &ambient.r);
    ImGui::ColorPicker3("LightColor", &sun.GetComponent<DirectionalLightComponent>()->color.x);*/
    ImGui::End();

    if (currentScene)
        currentScene->OnRender();
}

void Game::Close()
{
    Assets::Close();
}
