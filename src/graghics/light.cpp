#include "light.h"
#include "gtc/matrix_transform.hpp"

// 构造函数：初始化光源参数
Light::Light()
    : position_(0.0f, 0.0f, 0.0f)
    , color_(1.0f, 1.0f, 1.0f)
    , depthMapFBO_(0)
    , depthCubemap_(0)
    , shadowSize_(0)
    , nearPlane_(1.0f)
    , farPlane_(25.0f) {
}

// 析构函数：清理 OpenGL 资源
Light::~Light() {
    if (depthCubemap_) {
        glDeleteTextures(1, &depthCubemap_);
    }
    if (depthMapFBO_) {
        glDeleteFramebuffers(1, &depthMapFBO_);
    }
}

// 设置点光源位置和颜色
void Light::setPoint(const glm::vec3& pos, const glm::vec3& color) {
    position_ = pos;
    color_ = color;
}

// 初始化阴影立方体贴图资源
// size: 阴影贴图的分辨率（如 1024, 2048）
// nearPlane: 阴影投射的近平面
// farPlane: 阴影投射的远平面
void Light::setupShadowCube(int size, float nearPlane, float farPlane) {
    shadowSize_ = size;
    nearPlane_ = nearPlane;
    farPlane_ = farPlane;

    // 如果已存在资源，先清理
    if (depthCubemap_) {
        glDeleteTextures(1, &depthCubemap_);
        depthCubemap_ = 0;
    }
    if (depthMapFBO_) {
        glDeleteFramebuffers(1, &depthMapFBO_);
        depthMapFBO_ = 0;
    }

    // 创建帧缓冲对象 (FBO) 用于离屏渲染
    glGenFramebuffers(1, &depthMapFBO_);

    // 创建立方体贴图
    glGenTextures(1, &depthCubemap_);
    glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap_);
    
    // 为立方体贴图的 6 个面分配内存（仅深度分量）
    for (int i = 0; i < 6; ++i) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, shadowSize_, shadowSize_, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    }
    
    // 设置纹理参数
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    // 将立方体贴图附加到 FBO 的深度附件点
    // 注意：这里我们不需要颜色附件，所以将绘制和读取缓冲区都设为 GL_NONE
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO_);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemap_, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    
    // 检查 FBO 完整性（实际项目中建议检查）
    // if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) ...

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// 开始阴影深度贴图渲染 pass
// 将渲染目标切换到阴影 FBO，并设置视口大小
void Light::beginDepthPass() {
    glViewport(0, 0, shadowSize_, shadowSize_);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO_);
    glClear(GL_DEPTH_BUFFER_BIT);
}

// 结束阴影深度贴图渲染 pass
// 恢复默认帧缓冲
void Light::endDepthPass() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// 获取深度立方体贴图 ID
GLuint Light::depthCubeTexture() const {
    return depthCubemap_;
}

// 获取光源位置
const glm::vec3& Light::position() const {
    return position_;
}

// 获取光源颜色
const glm::vec3& Light::color() const {
    return color_;
}

// 获取阴影远平面距离
float Light::farPlane() const {
    return farPlane_;
}

// 获取阴影贴图尺寸
int Light::shadowSize() const {
    return shadowSize_;
}
