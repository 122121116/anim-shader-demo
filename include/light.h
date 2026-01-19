#pragma once
#include "glm.hpp"
#include <glad/glad.h>

// 光源管理类
// 负责管理场景中的点光源属性，以及生成全向阴影贴图 (Omnidirectional Shadow Map)
class Light {
public:
    Light();
    ~Light();

    // 设置光源位置和颜色
    void setPoint(const glm::vec3& pos, const glm::vec3& color);

    // 初始化阴影贴图资源
    // size: 阴影贴图分辨率 (如 1024, 2048)
    // nearPlane, farPlane: 阴影投影的近/远平面距离
    void setupShadowCube(int size, float nearPlane, float farPlane);

    // 开始深度 Pass：绑定帧缓冲，准备渲染深度图
    void beginDepthPass();
    // 结束深度 Pass：解绑帧缓冲
    void endDepthPass();

    // 获取深度立方体纹理 ID
    GLuint depthCubeTexture() const;
    
    // 获取光源位置
    const glm::vec3& position() const;
    // 获取光源颜色
    const glm::vec3& color() const;
    // 获取远平面距离
    float farPlane() const;
    // 获取阴影贴图尺寸
    int shadowSize() const;

private:
    glm::vec3 position_; // 光源位置
    glm::vec3 color_;    // 光源颜色

    GLuint depthMapFBO_;   // 阴影帧缓冲对象
    GLuint depthCubemap_;  // 深度立方体纹理
    int shadowSize_;       // 纹理分辨率
    float nearPlane_;      // 近平面
    float farPlane_;       // 远平面
};

