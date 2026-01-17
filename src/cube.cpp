#include "cube.h"

Cube::Cube(float size) {
    build(size);
}

void Cube::build(float size) {
    float h = size * 0.5f;

    struct Face {
        float nx, ny, nz;
        float v[4][3];
    };

    Face faces[6] = {
        {  0,  0,  1, {{-h,-h, h},{ h,-h, h},{ h, h, h},{-h, h, h}}}, // front
        {  0,  0, -1, {{ h,-h,-h},{-h,-h,-h},{-h, h,-h},{ h, h,-h}}}, // back
        { -1,  0,  0, {{-h,-h,-h},{-h,-h, h},{-h, h, h},{-h, h,-h}}}, // left
        {  1,  0,  0, {{ h,-h, h},{ h,-h,-h},{ h, h,-h},{ h, h, h}}}, // right
        {  0,  1,  0, {{-h, h, h},{ h, h, h},{ h, h,-h},{-h, h,-h}}}, // top
        {  0, -1,  0, {{-h,-h,-h},{ h,-h,-h},{ h,-h, h},{-h,-h, h}}}, // bottom
    };

    for (int f = 0; f < 6; ++f) {
        int base = (int)vertices_.size();

        for (int i = 0; i < 4; ++i) {
            Vertex v{};

            v.position[0] = faces[f].v[i][0];
            v.position[1] = faces[f].v[i][1];
            v.position[2] = faces[f].v[i][2];

            v.normal[0] = faces[f].nx;
            v.normal[1] = faces[f].ny;
            v.normal[2] = faces[f].nz;

            v.uv0[0] = (i == 1 || i == 2) ? 1.0f : 0.0f;
            v.uv0[1] = (i >= 2) ? 1.0f : 0.0f;

            v.uv1[0] = v.uv1[1] = 0.0f;
            v.uv2[0] = v.uv2[1] = 0.0f;

            vertices_.push_back(v);
        }

        indices_.push_back(base + 0);
        indices_.push_back(base + 1);
        indices_.push_back(base + 2);
        indices_.push_back(base + 2);
        indices_.push_back(base + 3);
        indices_.push_back(base + 0);
    }
}
