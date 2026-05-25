#pragma once

struct VertexData;

class Primitives {
public:
    static VertexData CreatePlane(float width = 1.0f, float depth = 1.0f, unsigned int segmentsX = 1, unsigned int segmentsZ = 1);
    static VertexData CreateCube(float size = 1.0f);
    static VertexData CreateSphere(float radius = 0.5f, unsigned int sectorCount = 36, unsigned int stackCount = 18);
};