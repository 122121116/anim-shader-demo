#include "shadow.h"
#include "ext/matrix_transform.hpp"

PointLightShadow::PointLightShadow()
    : depthMapFBO_(0),
      depthCubemap_(0),
      size_(0),
      nearPlane_(0.1f),
      farPlane_(25.0f),
      lightPos_(0.0f, 0.0f, 0.0f) {
}

PointLightShadow::~PointLightShadow() {
    if (depthCubemap_) {
        glDeleteTextures(1, &depthCubemap_);
    }
    if (depthMapFBO_) {
        glDeleteFramebuffers(1, &depthMapFBO_);
    }
}

bool PointLightShadow::init(int size, float nearPlane, float farPlane) {
    size_ = size;
    nearPlane_ = nearPlane;
    farPlane_ = farPlane;

    glGenFramebuffers(1, &depthMapFBO_);
    glGenTextures(1, &depthCubemap_);
    glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap_);
    for (int i = 0; i < 6; ++i) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, size_, size_, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO_);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemap_, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    bool ok = (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return ok;
}

void PointLightShadow::setLightPos(const glm::vec3& pos) {
    lightPos_ = pos;
}

const glm::vec3& PointLightShadow::lightPos() const {
    return lightPos_;
}

float PointLightShadow::nearPlane() const {
    return nearPlane_;
}

float PointLightShadow::farPlane() const {
    return farPlane_;
}

GLuint PointLightShadow::depthMap() const {
    return depthCubemap_;
}

GLuint PointLightShadow::fbo() const {
    return depthMapFBO_;
}

int PointLightShadow::size() const {
    return size_;
}

void PointLightShadow::computeMatrices(std::vector<glm::mat4>& out) const {
    out.resize(6);
    glm::mat4 proj = glm::perspective(glm::radians(90.0f), 1.0f, nearPlane_, farPlane_);
    glm::vec3 pos = lightPos_;
    out[0] = proj * glm::lookAt(pos, pos + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
    out[1] = proj * glm::lookAt(pos, pos + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
    out[2] = proj * glm::lookAt(pos, pos + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    out[3] = proj * glm::lookAt(pos, pos + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f));
    out[4] = proj * glm::lookAt(pos, pos + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
    out[5] = proj * glm::lookAt(pos, pos + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
}

