#pragma once
#include <string>
#include <vector>

struct Vertex {
    float position[3];
    float normal[3];
    float uv0[2];
    float uv1[2];
    float uv2[2];
};

struct TextureDesc {
    std::string image_uuid;
    std::vector<unsigned char> image_bytes;
    int width;
    int height;
    bool srgb;
    bool generate_mipmaps;
};

struct MaterialDesc {
    float color[3];
    std::string map_uuid;
};
