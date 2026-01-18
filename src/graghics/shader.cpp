#include "shader.h"
#include "glm.hpp"
#include "gtc/type_ptr.hpp"
#include <fstream>
#include <sstream>
Shader::Shader() : program_(0) {}
Shader::~Shader() {
    if (program_) glDeleteProgram(program_);
}
bool Shader::readFile(const std::string& path, std::string& out) {
    std::ifstream f(path, std::ios::in | std::ios::binary);
    if (!f) return false;
    std::ostringstream ss;
    ss << f.rdbuf();
    out = ss.str();
    if (out.size() >= 3) {
        unsigned char b0 = static_cast<unsigned char>(out[0]);
        unsigned char b1 = static_cast<unsigned char>(out[1]);
        unsigned char b2 = static_cast<unsigned char>(out[2]);
        if (b0 == 0xEF && b1 == 0xBB && b2 == 0xBF) {
            out.erase(0, 3);
        }
    }
    return true;
}
GLuint Shader::compileStage(GLenum type, const std::string& src, std::string& outError) {
    GLuint s = glCreateShader(type);
    const char* c = src.c_str();
    GLint len = (GLint)src.size();
    glShaderSource(s, 1, &c, &len);
    glCompileShader(s);
    GLint ok = 0;
    glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        GLint logLen = 0;
        glGetShaderiv(s, GL_INFO_LOG_LENGTH, &logLen);
        std::string log;
        log.resize(logLen);
        glGetShaderInfoLog(s, logLen, nullptr, log.data());
        outError = log;
        glDeleteShader(s);
        return 0;
    }
    return s;
}
bool Shader::compileFromSource(const std::string& vert, const std::string& frag) {
    std::string errV, errF;
    GLuint v = compileStage(GL_VERTEX_SHADER, vert, errV);
    if (!v) { error_ = errV; return false; }
    GLuint f = compileStage(GL_FRAGMENT_SHADER, frag, errF);
    if (!f) { glDeleteShader(v); error_ = errF; return false; }
    GLuint p = glCreateProgram();
    glAttachShader(p, v);
    glAttachShader(p, f);
    glLinkProgram(p);
    glDetachShader(p, v);
    glDetachShader(p, f);
    glDeleteShader(v);
    glDeleteShader(f);
    GLint ok = 0;
    glGetProgramiv(p, GL_LINK_STATUS, &ok);
    if (!ok) {
        GLint logLen = 0;
        glGetProgramiv(p, GL_INFO_LOG_LENGTH, &logLen);
        std::string log;
        log.resize(logLen);
        glGetProgramInfoLog(p, logLen, nullptr, log.data());
        glDeleteProgram(p);
        error_ = log;
        return false;
    }
    if (program_) glDeleteProgram(program_);
    program_ = p;
    error_.clear();
    return true;
}
bool Shader::compileFromFiles(const std::string& vertPath, const std::string& fragPath) {
    std::string vsrc, fsrc;
    if (!readFile(vertPath, vsrc)) { error_ = "vertex file read failed"; return false; }
    if (!readFile(fragPath, fsrc)) { error_ = "fragment file read failed"; return false; }
    return compileFromSource(vsrc, fsrc);
}
void Shader::use() const {
    glUseProgram(program_);
}
GLuint Shader::program() const { return program_; }
const std::string& Shader::error() const { return error_; }
GLint Shader::uniform(const char* name) const { return glGetUniformLocation(program_, name); }

void Shader::setBool(const std::string &name, bool value) const {
    GLint loc = glGetUniformLocation(program_, name.c_str());
    if (loc != -1) glUniform1i(loc, value ? 1 : 0);
}
void Shader::setInt(const std::string &name, int value) const {
    GLint loc = glGetUniformLocation(program_, name.c_str());
    if (loc != -1) glUniform1i(loc, value);
}
void Shader::setFloat(const std::string &name, float value) const {
    GLint loc = glGetUniformLocation(program_, name.c_str());
    if (loc != -1) glUniform1f(loc, value);
}
void Shader::setVec3(const std::string &name, const glm::vec3& value) const {
    GLint loc = glGetUniformLocation(program_, name.c_str());
    if (loc != -1) glUniform3fv(loc, 1, glm::value_ptr(value));
}
void Shader::setMat4(const std::string& name, const glm::mat4& value) const {
    GLint loc = glGetUniformLocation(program_, name.c_str());
    if (loc != -1) glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(value));
}
