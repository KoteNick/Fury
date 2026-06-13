#include "Input.h"
#include <GLFW/glfw3.h>
#include <bitset>
#include <unordered_map>
#include <vector>

namespace {
    GLFWwindow* window = nullptr;

    std::bitset<GLFW_KEY_LAST + 1> keyStates;
    std::bitset<GLFW_KEY_LAST + 1> keyStatesPrev;

    bool mouseBtnStates[GLFW_MOUSE_BUTTON_LAST + 1] = { false };

    std::unordered_map<std::string, std::vector<Key>> actionMap;

    glm::vec2 mouseLast(0.0f);
    glm::vec2 mouseDelta(0.0f);

    GLFWkeyfun prevKeyCallback = nullptr;
    GLFWmousebuttonfun prevMouseBtnCallback = nullptr;
    GLFWscrollfun prevScrollCallback = nullptr;

    float scrollAccum = 0.0f;
    float scrollDelta = 0.0f;
}

void Input::Init(GLFWwindow* glfwWindow)
{
    window = glfwWindow;
    if (!window)
        return;

    prevKeyCallback = glfwSetKeyCallback(window, [](GLFWwindow* w, int key, int scancode, int action, int mods){
        if (key >= 0 && key <= GLFW_KEY_LAST)
            keyStates[key] = (action != GLFW_RELEASE);

        if (prevKeyCallback)
            prevKeyCallback(w, key, scancode, action, mods);
    });

    prevMouseBtnCallback = glfwSetMouseButtonCallback(window, [](GLFWwindow* w, int button, int action, int mods) {
        if (button >= 0 && button <= GLFW_MOUSE_BUTTON_LAST)
            mouseBtnStates[button] = (action == GLFW_PRESS);

        if (prevMouseBtnCallback)
            prevMouseBtnCallback(w, button, action, mods);
    });

    prevScrollCallback = glfwSetScrollCallback(window, [](GLFWwindow* w, double xoffset, double yoffset) {
        scrollAccum += static_cast<float>(yoffset);

        if (prevScrollCallback)
            prevScrollCallback(w, xoffset, yoffset);
    });

    double x, y;
    glfwGetCursorPos(window, &x, &y);
    mouseLast = glm::vec2(static_cast<float>(x), static_cast<float>(y));
}

void Input::Update()
{
    if (!window) return;

    double currentX, currentY;
    glfwGetCursorPos(window, &currentX, &currentY);

    mouseDelta.x = static_cast<float>(currentX) - mouseLast.x;
    mouseDelta.y = static_cast<float>(currentY) - mouseLast.y;

    mouseLast.x = currentX;
    mouseLast.y = currentY;

    scrollDelta = scrollAccum;
    scrollAccum = 0.0f;

    keyStatesPrev = keyStates;
}

bool Input::IsKeyPressed(Key key)
{
    int code = static_cast<int>(key);
    return keyStates[code];
}

bool Input::IsKeyDown(Key key)
{
    int code = static_cast<int>(key);
    return (!keyStatesPrev[code] && keyStates[code]);
}

bool Input::IsKeyUp(Key key)
{
    int code = static_cast<int>(key);
    return (keyStatesPrev[code] && !keyStates[code]);
}

void Input::BindAction(const std::string& action, Key key)
{
    actionMap[action].push_back(key);
}

bool Input::IsActionActive(const std::string& action)
{
    auto it = actionMap.find(action);
    if (it != actionMap.end()) {
        for (Key key : it->second) {
            if (IsKeyPressed(key)) return true;
        }
    }
    return false;
}

bool Input::IsMouseDown(MouseButton button)
{
    int code = static_cast<int>(button);
    return (code >= 0 && code <= GLFW_MOUSE_BUTTON_LAST) ? mouseBtnStates[code] : false;
}

glm::vec2 Input::GetMousePos() {
    return mouseLast;
}

glm::vec2 Input::GetMouseDelta()
{
    return mouseDelta;
}

float Input::GetScrollDelta()
{
    return scrollDelta;
}
