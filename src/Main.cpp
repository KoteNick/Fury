#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <chrono>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "Core/Input.h"

#include "Renderer/Renderer.h"
#include "Renderer/Vertex.h"

#include "Game/Game.h"
#include "Game/Assets.h"

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error: %s\n", description);
}

void APIENTRY openglCallbackFunction(GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const GLchar* message,
    const void* userParam) {

    if (severity == GL_DEBUG_SEVERITY_NOTIFICATION) return;

    std::cerr << '[';
    switch (type) {
    case GL_DEBUG_TYPE_ERROR:               std::cerr << "ERROR"; break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cerr << "DEPRECATED"; break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cerr << "UNDEFINED"; break;
    case GL_DEBUG_TYPE_PORTABILITY:         std::cerr << "PORTABILITY"; break;
    case GL_DEBUG_TYPE_PERFORMANCE:         std::cerr << "PERFORMANCE"; break;
    default:                                std::cerr << "OTHER"; break;
    }
    std::cerr << "] ID: " << id << " | Severity: ";

    switch (severity) {
    case GL_DEBUG_SEVERITY_LOW:    std::cerr << "LOW"; break;
    case GL_DEBUG_SEVERITY_MEDIUM: std::cerr << "MEDIUM"; break;
    case GL_DEBUG_SEVERITY_HIGH:   std::cerr << "HIGH"; break;
    }
    std::cerr << message << std::endl;

    if (severity == GL_DEBUG_SEVERITY_HIGH)
        __debugbreak();
}

int main(void)
{
    glfwSetErrorCallback(error_callback);

    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(1600, 900, "Fury", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    glfwSwapInterval(1);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    float imGuiScale = 1.0f;
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    ImGui::GetIO().FontGlobalScale = imGuiScale;
    ImGui::GetStyle().ScaleAllSizes(imGuiScale);

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
    ImGui_ImplOpenGL3_Init();

    if (glewInit() != GLEW_OK)
        std::cout << "GLEW error";


    std::cout << glGetString(GL_VERSION) << std::endl;

    {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(openglCallbackFunction, nullptr);
        //glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);

        glEnable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        Input::Init(window);

        float time = 0;

        Game* game = new Game;

        game->Init();

        Renderer::Get().shadowBuffer = std::make_unique<FrameBuffer>(2048, 2048);
        glfwWindowHint(GLFW_SAMPLES, 4);
        glEnable(GL_MULTISAMPLE);

        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(window))
        {
            glfwGetWindowSize(window, &game->windowSize.width, &game->windowSize.height);

            float currentTime = (float)glfwGetTime();
            float deltaTime = currentTime - game->time;
            game->time = currentTime;

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            game->OnUpdate(deltaTime);

            Input::Update();

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            /* Swap front and back buffers */
            glfwSwapBuffers(window);

            /* Poll for and process events */
            glfwPollEvents();
        };
        game->Close();
        delete game;
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
    return 0;
}