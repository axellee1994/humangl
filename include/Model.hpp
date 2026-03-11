#pragma once

#include <GL/glew.h>
#include <vector>
#include "Math.hpp"
#include "Shader.hpp"
#include "Animation.hpp"

class Model {
public:
    float torsoW, torsoH, torsoD;
    float headS;
    float uArmW,  uArmH,  uArmD;
    float fArmW,  fArmH,  fArmD;
    float thighW, thighH, thighD;
    float lLegW,  lLegH,  lLegD;

    Model();
    ~Model();

    void draw(Shader &shader, const Pose &pose);

private:
    GLuint _vao, _vbo, _ebo;

    std::vector<mat4> _stack;
    mat4              _current;

    void _push();
    void _pop();
    void _apply(const mat4 &m);

    void _drawCube(Shader &shader);
};
