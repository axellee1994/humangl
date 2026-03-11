#include "Shader.hpp"

#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>

std::string Shader::readFile(const std::string &path) {
    std::ifstream file(path);
    if (!file.is_open())
        throw std::runtime_error("Shader::readFile: cannot open " + path);
    std::ostringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

GLuint Shader::compileShader(GLenum type, const std::string &src) {
    GLuint shader = glCreateShader(type);
    const char *c = src.c_str();
    glShaderSource(shader, 1, &c, nullptr);
    glCompileShader(shader);

    GLint ok = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char log[1024];
        glGetShaderInfoLog(shader, sizeof(log), nullptr, log);
        glDeleteShader(shader);
        throw std::runtime_error(std::string("Shader compile error:\n") + log);
    }
    return shader;
}

Shader::Shader(const std::string &vertexPath, const std::string &fragmentPath) {
    std::string vertSrc = readFile(vertexPath);
    std::string fragSrc = readFile(fragmentPath);

    GLuint vert = compileShader(GL_VERTEX_SHADER,   vertSrc);
    GLuint frag = compileShader(GL_FRAGMENT_SHADER, fragSrc);

    _program = glCreateProgram();
    glAttachShader(_program, vert);
    glAttachShader(_program, frag);
    glLinkProgram(_program);

    GLint ok = 0;
    glGetProgramiv(_program, GL_LINK_STATUS, &ok);
    if (!ok) {
        char log[1024];
        glGetProgramInfoLog(_program, sizeof(log), nullptr, log);
        glDeleteShader(vert);
        glDeleteShader(frag);
        glDeleteProgram(_program);
        throw std::runtime_error(std::string("Shader link error:\n") + log);
    }

    glDeleteShader(vert);
    glDeleteShader(frag);
}

Shader::~Shader() {
    glDeleteProgram(_program);
}

void Shader::use() const {
    glUseProgram(_program);
}

void Shader::setMat4(const std::string &name, const mat4 &value) const {
    GLint loc = glGetUniformLocation(_program, name.c_str());
    glUniformMatrix4fv(loc, 1, GL_FALSE, value.m);
}

void Shader::setVec3(const std::string &name, const vec3 &value) const {
    GLint loc = glGetUniformLocation(_program, name.c_str());
    glUniform3f(loc, value.x, value.y, value.z);
}

void Shader::setFloat(const std::string &name, float value) const {
    GLint loc = glGetUniformLocation(_program, name.c_str());
    glUniform1f(loc, value);
}
