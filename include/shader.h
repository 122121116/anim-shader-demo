#pragma once
#include <string>
#include <glad/glad.h>
#include "glm.hpp"

// 着色器管理类
// 负责编译、链接 GLSL 着色器程序，并提供设置 Uniform 变量的接口
class Shader {
public:
    // 构造函数
    Shader();
    // 析构函数，负责清理 Program 资源
    ~Shader();
    
    // 从源码字符串编译着色器
    // vert: 顶点着色器源码
    // frag: 片元着色器源码
    bool compileFromSource(const std::string& vert, const std::string& frag);
    
    // 从文件路径加载并编译着色器
    // vertPath: 顶点着色器文件路径
    // fragPath: 片元着色器文件路径
    bool compileFromFiles(const std::string& vertPath, const std::string& fragPath);
    
    // 激活当前着色器程序 (glUseProgram)
    void use() const;
    
    // 获取 Program ID
    GLuint program() const;
    
    // 获取编译/链接错误信息
    const std::string& error() const;
    
    // 获取 Uniform 变量的位置 Location
    GLint uniform(const char* name) const;
    
    // 设置 Uniform 变量
    void setBool(const std::string &name, bool value) const;
    void setInt(const std::string &name, int value) const;
    void setFloat(const std::string &name, float value) const;
    void setVec3(const std::string &name, const glm::vec3& value) const;
    void setMat4(const std::string& name, const glm::mat4& value) const;
    
private:
    GLuint program_;        // 着色器程序 ID
    std::string error_;     // 错误信息缓存
    
    // 辅助函数：读取文件内容
    static bool readFile(const std::string& path, std::string& out);
    // 辅助函数：编译单个着色器阶段 (Vertex/Fragment)
    static GLuint compileStage(GLenum type, const std::string& src, std::string& outError);
};
