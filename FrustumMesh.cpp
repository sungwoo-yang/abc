#include "FrustumMesh.h"

const Mesh::Edge FrustumMesh::edges[16] =
    {
        Edge(0, 1), Edge(0, 2), Edge(0, 3), Edge(0, 4), // E to near
        Edge(1, 2), Edge(2, 3), Edge(3, 4), Edge(4, 1), // near to near
        Edge(1, 5), Edge(2, 6), Edge(3, 7), Edge(4, 8), // near to far
        Edge(5, 6), Edge(6, 7), Edge(7, 8), Edge(8, 5)  // far to far
};

const Mesh::Face FrustumMesh::faces[12] =
    {
        Face(0, 1, 2), Face(0, 2, 3), Face(0, 3, 4), Face(0, 4, 1), // E to near
        Face(1, 5, 6), Face(1, 6, 2), Face(2, 6, 7), Face(2, 7, 3), // top and left of near to far
        Face(3, 7, 8), Face(3, 8, 4), Face(4, 8, 5), Face(4, 5, 1)  // bottom and top of near to far
};

FrustumMesh::FrustumMesh(float fov, float a, float n, float f)
{
    vertices[0] = Point(0, 0, 0);

    float W_near = std::tan(fov / 2.0f) * n * 2.0f;
    float H_near = W_near / a;

    vertices[1] = Point(W_near / 2, H_near / 2, -n);
    vertices[2] = Point(-W_near / 2, H_near / 2, -n);
    vertices[3] = Point(-W_near / 2, -H_near / 2, -n);
    vertices[4] = Point(W_near / 2, -H_near / 2, -n);

    float W_far = std::tan(fov / 2.0f) * f * 2.0f;
    float H_far = W_far / a;

    vertices[5] = Point(W_far / 2, H_far / 2, -f);
    vertices[6] = Point(-W_far / 2, H_far / 2, -f);
    vertices[7] = Point(-W_far / 2, -H_far / 2, -f);
    vertices[8] = Point(W_far / 2, -H_far / 2, -f);
}

int FrustumMesh::VertexCount(void)
{
    return 9;
}

Point FrustumMesh::GetVertex(int i)
{
    return vertices[i];
}

Vector FrustumMesh::Dimensions(void)
{
    float min_x = vertices[0].x;
    float max_x = vertices[0].x;
    float min_y = vertices[0].y;
    float max_y = vertices[0].y;
    float min_z = vertices[0].z;
    float max_z = vertices[0].z;

    for (int i = 1; i < 9; i++)
    {
        min_x = (vertices[i].x < min_x) ? vertices[i].x : min_x;
        max_x = (vertices[i].x > max_x) ? vertices[i].x : max_x;

        min_y = (vertices[i].y < min_y) ? vertices[i].y : min_y;
        max_y = (vertices[i].y > max_y) ? vertices[i].y : max_y;

        min_z = (vertices[i].z < min_z) ? vertices[i].z : min_z;
        max_z = (vertices[i].z > max_z) ? vertices[i].z : max_z;
    }

    return Vector(max_x - min_x, max_y - min_y, max_z - min_z);
}

Point FrustumMesh::Center(void)
{
    float min_x = vertices[0].x;
    float max_x = vertices[0].x;
    float min_y = vertices[0].y;
    float max_y = vertices[0].y;
    float min_z = vertices[0].z;
    float max_z = vertices[0].z;

    for (int i = 1; i < 9; i++)
    {
        min_x = (vertices[i].x < min_x) ? vertices[i].x : min_x;
        max_x = (vertices[i].x > max_x) ? vertices[i].x : max_x;

        min_y = (vertices[i].y < min_y) ? vertices[i].y : min_y;
        max_y = (vertices[i].y > max_y) ? vertices[i].y : max_y;

        min_z = (vertices[i].z < min_z) ? vertices[i].z : min_z;
        max_z = (vertices[i].z > max_z) ? vertices[i].z : max_z;
    }

    return Point((max_x + min_x) * 0.5f, (max_y + min_y) * 0.5f, (max_z + min_z) * 0.5f);
}

int FrustumMesh::FaceCount(void)
{
    return 12;
}

Mesh::Face FrustumMesh::GetFace(int i)
{
    return faces[i];
}

int FrustumMesh::EdgeCount(void)
{
    return 16;
}

Mesh::Edge FrustumMesh::GetEdge(int i)
{
    return edges[i];
}
