#include "light.h"
#include "gtc/matrix_transform.hpp"

Light::Light()
    : position_(0.0f, 0.0f, 0.0f)
    , color_(1.0f, 1.0f, 1.0f)
    , depthMapFBO_(0)
    , depthCubemap_(0)
    , shadowSize_(0)
    , nearPlane_(1.0f)
    , farPlane_(25.0f) {
}

Light::~Light() {
    if (depthCubemap_) {
        glDeleteTextures(1, &depthCubemap_);
    }
    if (depthMapFBO_) {
        glDeleteFramebuffers(1, &depthMapFBO_);
    }
}

void Light::setPoint(const glm::vec3& pos, const glm::vec3& color) {
    position_ = pos;
    color_ = color;
}

void Light::setupShadowCube(int size, float nearPlane, float farPlane) {
    shadowSize_ = size;
    nearPlane_ = nearPlane;
    farPlane_ = farPlane;

    if (depthCubemap_) {
        glDeleteTextures(1, &depthCubemap_);
        depthCubemap_ = 0;
    }
    if (depthMapFBO_) {
        glDeleteFramebuffers(1, &depthMapFBO_);
        depthMapFBO_ = 0;
    }

    glGenFramebuffers(1, &depthMapFBO_);

    glGenTextures(1, &depthCubemap_);
    glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap_);
    for (int i = 0; i < 6; ++i) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, shadowSize_, shadowSize_, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO_);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Light::beginDepthPass() {
    glViewport(0, 0, shadowSize_, shadowSize_);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO_);
}

void Light::endDepthPass() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GLuint Light::depthCubeTexture() const {
    return depthCubemap_;
}

const glm::vec3& Light::position() const {
    return position_;
}

const glm::vec3& Light::color() const {
    return color_;
}

float Light::farPlane() const {
    return farPlane_;
}

int Light::shadowSize() const {
    return shadowSize_;
}
