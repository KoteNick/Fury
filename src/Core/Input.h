#pragma once
#include "KeyCodes.h"

#include "glm/glm.hpp"

#include <string>

struct GLFWwindow;

class Input {
public:
    static void Init(GLFWwindow* window);
    static void Update();

    static bool IsKeyPressed(Key key);
    static bool IsKeyDown(Key key);
    static bool IsKeyUp(Key key);

    static void BindAction(const std::string& action, Key key);
    static bool IsActionActive(const std::string& action);

    static bool IsMouseDown(MouseButton button);

    static glm::vec2 GetMousePos();
    static glm::vec2 GetMouseDelta();
    static float GetScrollDelta();
};