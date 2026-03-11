#pragma once

#include <string>
#include <GL/glew.h>
#include "Math.hpp"

class Shader {
public:
    Shader(const std::string &vertexPath, const std::string &fragmentPath);
    ~Shader();

    void use() const;

    void setMat4 (const std::string &name, const mat4 &value) const;
    void setVec3 (const std::string &name, const vec3 &value) const;
    void setFloat(const std::string &name, float       value) const;

private:
    GLuint _program;

    static std::string readFile(const std::string &path);
    static GLuint      compileShader(GLenum type, const std::string &src);
};
