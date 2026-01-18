#pragma once
#include "glm.hpp"
#include <glad/glad.h>

class Light {
public:
    Light();
    ~Light();

    void setPoint(const glm::vec3& pos, const glm::vec3& color);

    void setupShadowCube(int size, float nearPlane, float farPlane);

    void beginDepthPass();
    void endDepthPass();

    GLuint depthCubeTexture() const;
    const glm::vec3& position() const;
    const glm::vec3& color() const;
    float farPlane() const;
    int shadowSize() const;

private:
    glm::vec3 position_;
    glm::vec3 color_;

    GLuint depthMapFBO_;
    GLuint depthCubemap_;
    int shadowSize_;
    float nearPlane_;
    float farPlane_;
};

