#pragma once
#include <string>
#include <glad/glad.h>
#include "glm.hpp"
class Shader {
public:
    Shader();
    ~Shader();
    bool compileFromSource(const std::string& vert, const std::string& frag);
    bool compileFromFiles(const std::string& vertPath, const std::string& fragPath);
    void use() const;
    GLuint program() const;
    const std::string& error() const;
    GLint uniform(const char* name) const;
    void setBool(const std::string &name, bool value) const;
    void setInt(const std::string &name, int value) const;
    void setFloat(const std::string &name, float value) const;
    void setVec3(const std::string &name, const glm::vec3& value) const;
private:
    GLuint program_;
    std::string error_;
    static bool readFile(const std::string& path, std::string& out);
    static GLuint compileStage(GLenum type, const std::string& src, std::string& outError);
};
