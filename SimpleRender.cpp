#include "SimpleRender.h"

SimpleRender::SimpleRender(Render &r) : render(r)
{
    Hcoord row0(1.0f, 0, 0, 0);
    Hcoord row1(0, 1.0f, 0, 0);
    Hcoord row2(0, 0, 1.0f / 11.0f, -5.0f / 11.0f);
    Hcoord row3(0, 0, -2.0f / 11.0f, 10.0f / 11.0f);

    PersProj = {row0, row1, row2, row3};
}

SimpleRender::~SimpleRender(void)
{
}

void SimpleRender::DisplayEdges(Mesh &m, const Affine &A, const Vector &color)
{
    world_vertices.clear();
    proj_vertices.clear();

    render.SetColor(color);

    for (int i = 0; i < m.VertexCount(); ++i)
    {
        Point world_pt = A * m.GetVertex(i);
        world_vertices.push_back(world_pt);

        Hcoord proj_pt = PersProj * world_pt;

        proj_pt.x /= proj_pt.w;
        proj_pt.y /= proj_pt.w;
        proj_pt.z /= proj_pt.w;

        proj_vertices.push_back(Point(proj_pt.x, proj_pt.y, proj_pt.z));
    }

    for (int i = 0; i < m.EdgeCount(); ++i)
    {
        Mesh::Edge e = m.GetEdge(i);

        int index1 = e.index1;
        int index2 = e.index2;

        Point p1 = proj_vertices[index1];
        Point p2 = proj_vertices[index2];

        render.DrawLine(p1, p2);
    }
}

void SimpleRender::DisplayFaces(Mesh &m, const Affine &A, const Vector &color)
{
    world_vertices.clear();
    proj_vertices.clear();

    render.SetColor(color);

    for (int i = 0; i < m.VertexCount(); ++i)
    {
        Point world_pt = A * m.GetVertex(i);
        world_vertices.push_back(world_pt);

        Hcoord proj_pt = PersProj * world_pt;

        proj_pt.x /= proj_pt.w;
        proj_pt.y /= proj_pt.w;
        proj_pt.z /= proj_pt.w;

        proj_vertices.push_back(Point(proj_pt.x, proj_pt.y, proj_pt.z));
    }

    Point E(0, 0, 5);
    Vector L(0, 0, 1);

    for (int i = 0; i < m.FaceCount(); ++i)
    {
        Mesh::Face f = m.GetFace(i);

        int index1 = f.index1;
        int index2 = f.index2;
        int index3 = f.index3;

        Point w1 = world_vertices[index1];
        Point w2 = world_vertices[index2];
        Point w3 = world_vertices[index3];

        Vector edge1(w2 - w1);
        Vector edge2(w3 - w1);
        Vector m_normal = cross(edge1, edge2);

        Vector PE(E - w1);

        if (dot(m_normal, PE) <= 0)
        {
            continue;
        }

        float dot_Lm = dot(L, m_normal);
        float len_L = abs(L);
        float len_m = abs(m_normal);

        float mu = std::abs(dot_Lm) / (len_L * len_m);

        Vector shaded_color(mu * color);

        render.SetColor(shaded_color);

        Point p1 = proj_vertices[index1];
        Point p2 = proj_vertices[index2];
        Point p3 = proj_vertices[index3];

        render.FillTriangle(p1, p2, p3);
    }
}
