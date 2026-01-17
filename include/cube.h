#pragma once
#include <vector>
#include "attribute.h"

class Cube {
public:
    Cube(float size = 1.0f);

    const std::vector<Vertex>& vertices() const { return vertices_; }
    const std::vector<unsigned int>& indices() const { return indices_; }

private:
    std::vector<Vertex> vertices_;
    std::vector<unsigned int> indices_;

    void build(float size);
};
