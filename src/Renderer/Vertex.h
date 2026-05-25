#pragma once
#include "glm/glm.hpp"
#include "VertexBufferLayout.h"

struct Vertex2D
{
	float pos[2];
	float color[3];
};

struct Vertex3D {
    glm::vec3 pos;
    glm::vec3 normal;
    glm::vec2 uv;

    static VertexBufferLayout GetLayout() {
        VertexBufferLayout layout;
        layout
            .Push<float>(3)
            .Push<float>(3)
            .Push<float>(2);
        return layout;
    }
};