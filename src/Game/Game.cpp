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

    scenes["Test"] = std::make_unique<TestScene>();
    scenes["Ocean"] = std::make_unique<OceanScene>();
    scenes["Terrain"] = std::make_unique<TerrainScene>();
    scenes["Field"] = std::make_unique<FieldScene>();
    scenes["Solar"] = std::make_unique<SolarScene>();
}

void Game::OnUpdate(float deltaTime) {
    Renderer::Get().SetViewport(windowSize.width, windowSize.height);
    //Renderer::Get().Clear();
    if (currentScene) {
        if (currentScene->camera)
            currentScene->camera->GetComponent<CameraComponent>()->aspect = static_cast<float>(windowSize.width) / windowSize.height;

        currentScene->OnUpdate(deltaTime);
        currentScene->debugUI = debugUI;
    }

    if (Input::IsKeyDown(Key::GraveAccent))
        debugUI = !debugUI;

    if (debugUI) {
        ImGui::Begin("Debug");
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::Text("DeltaTime: %.5f seconds", deltaTime);
        if (ImGui::Checkbox("Wireframe Mode", &wireframeMode))
        {
            if (wireframeMode) {
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            }
            else {
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            }
        }
        ImGui::Checkbox("Shadow Pass", &shadowPass);

        auto stats = Renderer::Get().rendererStats;
        ImGui::Text("Vertices on scene: %i", stats.vertexCount);
        ImGui::Text("Draw calls: %i", stats.drawCalls);

        if (ImGui::CollapsingHeader("Scenes", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::Indent();

            for (auto& v : scenes) {
                if (ImGui::Button(v.first.c_str())) {
                    if (currentScene) currentScene->Destroy();
                    currentScene = v.second.get();
                    currentScene->Init();
                }
            }

            ImGui::Unindent();
        }

        ImGui::End();
    }

    if (currentScene)
        currentScene->OnRender(shadowPass);
}

void Game::Close()
{
    Assets::Close();
}