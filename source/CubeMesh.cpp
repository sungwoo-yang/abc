#include "CubeMesh.h"

int CubeMesh::VertexCount(void)
{
    return 8;
}

Point CubeMesh::GetVertex(int i)
{
    return vertices[i];
}

Vector CubeMesh::Dimensions(void)
{
    float min_x = vertices[0].x;
    float max_x = vertices[0].x;
    float min_y = vertices[0].y;
    float max_y = vertices[0].y;
    float min_z = vertices[0].z;
    float max_z = vertices[0].z;

    for (int i = 1; i < 8; i++)
    {
        min_x = std::min(min_x, vertices[i].x);
        max_x = std::max(max_x, vertices[i].x);
        min_y = std::min(min_y, vertices[i].y);
        max_y = std::max(max_y, vertices[i].y);
        min_z = std::min(min_z, vertices[i].z);
        max_z = std::max(max_z, vertices[i].z);
    }

    return Vector(max_x - min_x, max_y - min_y, max_z - min_z);
}

Point CubeMesh::Center(void)
{
    float min_x = vertices[0].x;
    float max_x = vertices[0].x;
    float min_y = vertices[0].y;
    float max_y = vertices[0].y;
    float min_z = vertices[0].z;
    float max_z = vertices[0].z;

    for (int i = 1; i < 8; i++)
    {
        min_x = std::min(min_x, vertices[i].x);
        max_x = std::max(max_x, vertices[i].x);
        min_y = std::min(min_y, vertices[i].y);
        max_y = std::max(max_y, vertices[i].y);
        min_z = std::min(min_z, vertices[i].z);
        max_z = std::max(max_z, vertices[i].z);
    }

    return Point((max_x + min_x) * 0.5f, (max_y + min_y) * 0.5f, (max_z + min_z) * 0.5f);
}

int CubeMesh::FaceCount(void)
{
    return 12;
}

CubeMesh::Face CubeMesh::GetFace(int i)
{
    return faces[i];
}

int CubeMesh::EdgeCount(void)
{
    return 12;
}

CubeMesh::Edge CubeMesh::GetEdge(int i)
{
    return edges[i];
}
