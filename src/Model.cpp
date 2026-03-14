#include "Model.hpp"

#include <stdexcept>

static const float CUBE_VERTS[] = {
    -0.5f,-0.5f,-0.5f,  0, 0,-1,
     0.5f,-0.5f,-0.5f,  0, 0,-1,
     0.5f, 0.5f,-0.5f,  0, 0,-1,
    -0.5f, 0.5f,-0.5f,  0, 0,-1,
    -0.5f,-0.5f, 0.5f,  0, 0, 1,
     0.5f,-0.5f, 0.5f,  0, 0, 1,
     0.5f, 0.5f, 0.5f,  0, 0, 1,
    -0.5f, 0.5f, 0.5f,  0, 0, 1,
    -0.5f,-0.5f,-0.5f, -1, 0, 0,
    -0.5f, 0.5f,-0.5f, -1, 0, 0,
    -0.5f, 0.5f, 0.5f, -1, 0, 0,
    -0.5f,-0.5f, 0.5f, -1, 0, 0,
     0.5f,-0.5f,-0.5f,  1, 0, 0,
     0.5f, 0.5f,-0.5f,  1, 0, 0,
     0.5f, 0.5f, 0.5f,  1, 0, 0,
     0.5f,-0.5f, 0.5f,  1, 0, 0,
    -0.5f,-0.5f,-0.5f,  0,-1, 0,
     0.5f,-0.5f,-0.5f,  0,-1, 0,
     0.5f,-0.5f, 0.5f,  0,-1, 0,
    -0.5f,-0.5f, 0.5f,  0,-1, 0,
    -0.5f, 0.5f,-0.5f,  0, 1, 0,
     0.5f, 0.5f,-0.5f,  0, 1, 0,
     0.5f, 0.5f, 0.5f,  0, 1, 0,
    -0.5f, 0.5f, 0.5f,  0, 1, 0,
};

static const unsigned int CUBE_IDX[] = {
     0, 1, 2,  2, 3, 0,
     4, 5, 6,  6, 7, 4,
     8, 9,10, 10,11, 8,
    12,13,14, 14,15,12,
    16,17,18, 18,19,16,
    20,21,22, 22,23,20,
};

Model::Model()
    : torsoW(0.30f), torsoH(0.45f), torsoD(0.20f),
      headS(0.23f),
      uArmW(0.10f),  uArmH(0.22f),  uArmD(0.10f),
      fArmW(0.09f),  fArmH(0.19f),  fArmD(0.09f),
      thighW(0.12f), thighH(0.24f), thighD(0.12f),
      lLegW(0.10f),  lLegH(0.22f),  lLegD(0.10f),
      _vao(0), _vbo(0), _ebo(0),
      _current(identity())
{
    glGenVertexArrays(1, &_vao);
    glGenBuffers(1, &_vbo);
    glGenBuffers(1, &_ebo);

    glBindVertexArray(_vao);

    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(CUBE_VERTS), CUBE_VERTS, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(CUBE_IDX), CUBE_IDX, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void*>(0));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));

    glBindVertexArray(0);
}

Model::~Model() {
    glDeleteBuffers(1, &_ebo);
    glDeleteBuffers(1, &_vbo);
    glDeleteVertexArrays(1, &_vao);
}

void Model::_push() {
    _stack.push_back(_current);
}

void Model::_pop() {
    if (_stack.empty())
        throw std::runtime_error("Model::_pop: stack underflow");
    _current = _stack.back();
    _stack.pop_back();
}

void Model::_apply(const mat4 &m) {
    _current = _current * m;
}

void Model::_drawCube(Shader &shader) {
    shader.setMat4("model", _current);
    glBindVertexArray(_vao);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

void Model::draw(Shader &shader, const Pose &pose) {
    _stack.clear();
    _current = identity();

    _apply(translate(0.f, pose.bodyY, 0.f));
    _apply(rotateY(pose.joints[TORSO_Y]));
    _apply(rotateX(pose.joints[TORSO_X]));

    _push();
        _apply(scale(torsoW, torsoH, torsoD));
        shader.setVec3("color", {0.2f, 0.4f, 0.8f});
        _drawCube(shader);
    _pop();

    _push();
        _apply(translate(0.f, torsoH * 0.5f + headS * 0.5f + 0.02f, 0.f));
        _apply(rotateY(pose.joints[HEAD_Y]));
        _apply(scale(headS, headS, headS));
        shader.setVec3("color", {0.9f, 0.7f, 0.5f});
        _drawCube(shader);
    _pop();

    _push();
        _apply(translate(-(torsoW * 0.5f + uArmW * 0.5f + 0.01f),
                          torsoH * 0.5f - uArmH * 0.15f,
                          0.f));
        _apply(rotateX(pose.joints[L_SHOULDER]));

        _push();
            _apply(translate(0.f, -uArmH * 0.5f, 0.f));
            _apply(scale(uArmW, uArmH, uArmD));
            shader.setVec3("color", {0.2f, 0.4f, 0.8f});
            _drawCube(shader);
        _pop();

        _apply(translate(0.f, -uArmH, 0.f));
        _apply(rotateX(pose.joints[L_ELBOW]));
        _apply(translate(0.f, -fArmH * 0.5f, 0.f));
        _apply(scale(fArmW, fArmH, fArmD));
        shader.setVec3("color", {0.2f, 0.4f, 0.8f});
        _drawCube(shader);
    _pop();

    _push();
        _apply(translate( torsoW * 0.5f + uArmW * 0.5f + 0.01f,
                          torsoH * 0.5f - uArmH * 0.15f,
                          0.f));
        _apply(rotateX(pose.joints[R_SHOULDER]));

        _push();
            _apply(translate(0.f, -uArmH * 0.5f, 0.f));
            _apply(scale(uArmW, uArmH, uArmD));
            shader.setVec3("color", {0.2f, 0.4f, 0.8f});
            _drawCube(shader);
        _pop();

        _apply(translate(0.f, -uArmH, 0.f));
        _apply(rotateX(pose.joints[R_ELBOW]));
        _apply(translate(0.f, -fArmH * 0.5f, 0.f));
        _apply(scale(fArmW, fArmH, fArmD));
        shader.setVec3("color", {0.2f, 0.4f, 0.8f});
        _drawCube(shader);
    _pop();

    _push();
        _apply(translate(-(thighW * 0.6f), -torsoH * 0.5f, 0.f));
        _apply(rotateX(pose.joints[L_HIP]));

        _push();
            _apply(translate(0.f, -thighH * 0.5f, 0.f));
            _apply(scale(thighW, thighH, thighD));
            shader.setVec3("color", {0.1f, 0.2f, 0.6f});
            _drawCube(shader);
        _pop();

        _apply(translate(0.f, -thighH, 0.f));
        _apply(rotateX(pose.joints[L_KNEE]));
        _apply(translate(0.f, -lLegH * 0.5f, 0.f));
        _apply(scale(lLegW, lLegH, lLegD));
        shader.setVec3("color", {0.1f, 0.2f, 0.6f});
        _drawCube(shader);
    _pop();

    _push();
        _apply(translate( thighW * 0.6f, -torsoH * 0.5f, 0.f));
        _apply(rotateX(pose.joints[R_HIP]));

        _push();
            _apply(translate(0.f, -thighH * 0.5f, 0.f));
            _apply(scale(thighW, thighH, thighD));
            shader.setVec3("color", {0.1f, 0.2f, 0.6f});
            _drawCube(shader);
        _pop();

        _apply(translate(0.f, -thighH, 0.f));
        _apply(rotateX(pose.joints[R_KNEE]));
        _apply(translate(0.f, -lLegH * 0.5f, 0.f));
        _apply(scale(lLegW, lLegH, lLegD));
        shader.setVec3("color", {0.1f, 0.2f, 0.6f});
        _drawCube(shader);
    _pop();
}
