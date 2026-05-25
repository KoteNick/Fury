#include "Primitives.h";
#include "Renderer/Mesh.h"
#include "Renderer/Vertex.h"

#include <glm/glm.hpp>

constexpr float PI = 3.14159265359f;

VertexData Primitives::CreatePlane(float width, float depth, unsigned int segmentsX, unsigned int segmentsZ)
{
    if (segmentsX < 1) segmentsX = 1;
    if (segmentsZ < 1) segmentsZ = 1;

    float halfW = width / 2.0f;
    float halfD = depth / 2.0f;
    float stepX = width / static_cast<float>(segmentsX);
    float stepZ = depth / static_cast<float>(segmentsZ);

    glm::vec3 normal(0.0f, 1.0f, 0.0f);

    std::vector<Vertex3D> verts;

    for (unsigned int z = 0; z <= segmentsZ; ++z) {
        float zPos = -halfD + z * stepZ;
        float v = static_cast<float>(z) / segmentsZ;

        for (unsigned int x = 0; x <= segmentsX; ++x) {
            float xPos = -halfW + x * stepX;
            float u = static_cast<float>(x) / segmentsX;

            verts.push_back({ {xPos, 0.0f, zPos}, normal, {u, v} });
        }
    }

    VertexData vd(verts);
    vd.layout = Vertex3D::GetLayout();

    for (unsigned int z = 0; z < segmentsZ; ++z) {
        for (unsigned int x = 0; x < segmentsX; ++x) {
            unsigned int row1 = z * (segmentsX + 1);
            unsigned int row2 = (z + 1) * (segmentsX + 1);

            vd.indices.push_back(row1 + x);
            vd.indices.push_back(row2 + x);
            vd.indices.push_back(row1 + x + 1);

            vd.indices.push_back(row1 + x + 1);
            vd.indices.push_back(row2 + x);
            vd.indices.push_back(row2 + x + 1);
        }
    }

    return vd;
}

VertexData Primitives::CreateCube(float size)
{
    float hs = size / 2.0f;

    glm::vec3 nBack(0, 0, -1), nFront(0, 0, 1), nLeft(-1, 0, 0), nRight(1, 0, 0), nBottom(0, -1, 0), nTop(0, 1, 0);

    std::vector<Vertex3D> verts = {
        { {-hs, -hs, -hs}, nBack, {0, 0} }, { { hs, -hs, -hs}, nBack, {1, 0} },
        { { hs,  hs, -hs}, nBack, {1, 1} }, { {-hs,  hs, -hs}, nBack, {0, 1} },

        { {-hs, -hs,  hs}, nFront, {0, 0} }, { { hs, -hs,  hs}, nFront, {1, 0} },
        { { hs,  hs,  hs}, nFront, {1, 1} }, { {-hs,  hs,  hs}, nFront, {0, 1} },

        { {-hs,  hs,  hs}, nLeft, {1, 0} }, { {-hs,  hs, -hs}, nLeft, {1, 1} },
        { {-hs, -hs, -hs}, nLeft, {0, 1} }, { {-hs, -hs,  hs}, nLeft, {0, 0} },

        { { hs,  hs,  hs}, nRight, {0, 0} }, { { hs, -hs,  hs}, nRight, {0, 1} },
        { { hs, -hs, -hs}, nRight, {1, 1} }, { { hs,  hs, -hs}, nRight, {1, 0} },

        { {-hs, -hs, -hs}, nBottom, {0, 1} }, { { hs, -hs, -hs}, nBottom, {1, 1} },
        { { hs, -hs,  hs}, nBottom, {1, 0} }, { {-hs, -hs,  hs}, nBottom, {0, 0} },

        { {-hs,  hs, -hs}, nTop, {0, 1} }, { {-hs,  hs,  hs}, nTop, {0, 0} },
        { { hs,  hs,  hs}, nTop, {1, 0} }, { { hs,  hs, -hs}, nTop, {1, 1} }
    };

    VertexData vd(verts);

    for (unsigned int i = 0; i < 6; ++i) {
        unsigned int offset = i * 4;
        vd.indices.push_back(offset + 0); vd.indices.push_back(offset + 1); vd.indices.push_back(offset + 2);
        vd.indices.push_back(offset + 2); vd.indices.push_back(offset + 3); vd.indices.push_back(offset + 0);
    }
    vd.layout = Vertex3D::GetLayout();

    return vd;
}

VertexData Primitives::CreateSphere(float radius, unsigned int sectorCount, unsigned int stackCount)
{
    float x, y, z, xy;
    float nx, ny, nz, lengthInv = 1.0f / radius;
    float s, t;

    float sectorStep = 2 * PI / sectorCount;
    float stackStep = PI / stackCount;
    float sectorAngle, stackAngle;

    std::vector<Vertex3D> verts;

    for (unsigned int i = 0; i <= stackCount; ++i) {
        stackAngle = PI / 2 - i * stackStep;
        xy = radius * cosf(stackAngle);
        z = radius * sinf(stackAngle); 

        for (unsigned int j = 0; j <= sectorCount; ++j) {
            sectorAngle = j * sectorStep;

            x = xy * cosf(sectorAngle);
            y = xy * sinf(sectorAngle);
            glm::vec3 pos(x, z, y);

            glm::vec3 normal(pos.x * lengthInv, pos.y * lengthInv, pos.z * lengthInv);

            s = (float)j / sectorCount;
            t = (float)i / stackCount;

            verts.push_back({ pos, normal, {s, t} });
        }
    }

    VertexData vd(verts);
    vd.layout = Vertex3D::GetLayout();

    unsigned int k1, k2;
    for (unsigned int i = 0; i < stackCount; ++i) {
        k1 = i * (sectorCount + 1);
        k2 = k1 + sectorCount + 1;

        for (unsigned int j = 0; j < sectorCount; ++j, ++k1, ++k2) {
            if (i != 0) {
                vd.indices.push_back(k1);
                vd.indices.push_back(k2);
                vd.indices.push_back(k1 + 1);
            }
            if (i != (stackCount - 1)) {
                vd.indices.push_back(k1 + 1);
                vd.indices.push_back(k2);
                vd.indices.push_back(k2 + 1);
            }
        }
    }

    return vd;
}
