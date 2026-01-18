#pragma once
#include <glad/glad.h>
#include "glm.hpp"
#include <vector>

class PointLightShadow {
public:
    PointLightShadow();
    ~PointLightShadow();

    bool init(int size, float nearPlane, float farPlane);

    void setLightPos(const glm::vec3& pos);
    const glm::vec3& lightPos() const;

    float nearPlane() const;
    float farPlane() const;

    GLuint depthMap() const;
    GLuint fbo() const;
    int size() const;

    void computeMatrices(std::vector<glm::mat4>& out) const;

private:
    GLuint depthMapFBO_;
    GLuint depthCubemap_;
    int size_;
    float nearPlane_;
    float farPlane_;
    glm::vec3 lightPos_;
};

