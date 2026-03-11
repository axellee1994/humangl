#include "Animation.hpp"

#include <cmath>
#include <algorithm>

static constexpr float PI = 3.14159265358979323846f;

Animation::Animation() : _type(AnimType::IDLE), _time(0.f) {}

void Animation::update(float dt) {
    _time += dt;
}

void Animation::setType(AnimType type) {
    _type = type;
    _time = 0.f;
}

AnimType Animation::getType() const {
    return _type;
}

Pose Animation::getPose() const {
    switch (_type) {
        case AnimType::WALK: return _computeWalk();
        case AnimType::JUMP: return _computeJump();
        default:             return _computeIdle();
    }
}

Pose Animation::_computeIdle() const {
    Pose p;
    p.bodyY = 0.005f * std::sin(2.f * _time);
    return p;
}

Pose Animation::_computeWalk() const {
    Pose p;
    const float speed = 2.5f;
    float phase = _time * speed;

    p.joints[L_HIP]      =  0.35f * std::sin(phase);
    p.joints[R_HIP]      = -0.35f * std::sin(phase);
    p.joints[L_KNEE]     =  0.25f * std::max(0.f, std::sin(phase + 0.3f));
    p.joints[R_KNEE]     =  0.25f * std::max(0.f, -std::sin(phase + 0.3f));
    p.joints[L_SHOULDER] = -0.3f  * std::sin(phase);
    p.joints[R_SHOULDER] =  0.3f  * std::sin(phase);
    p.joints[L_ELBOW]    =  0.2f  * std::max(0.f, std::sin(phase + 0.5f));
    p.joints[R_ELBOW]    =  0.2f  * std::max(0.f, -std::sin(phase + 0.5f));
    p.bodyY = 0.015f * std::sin(phase * 2.f);
    return p;
}

Pose Animation::_computeJump() const {
    Pose p;
    const float period = 1.8f;
    float phase = std::fmod(_time, period) / period; // 0..1

    if (phase < 0.2f) {
        float crouchT = phase / 0.2f;
        p.bodyY = -0.08f * std::sin(crouchT * PI);
        float hipAngle   = 0.4f * std::sin(crouchT * PI);
        float kneeAngle  = 0.5f * std::sin(crouchT * PI);
        p.joints[L_HIP]  = hipAngle;
        p.joints[R_HIP]  = hipAngle;
        p.joints[L_KNEE] = kneeAngle;
        p.joints[R_KNEE] = kneeAngle;
    } else if (phase < 0.65f) {
        float airT = (phase - 0.2f) / 0.45f;
        p.bodyY = 0.5f * std::sin(airT * PI);
        float hipAngle  = -0.25f * std::sin(airT * PI);
        float armAngle  = -0.9f  * std::sin(airT * PI);
        p.joints[TORSO_X]    = -0.1f * std::sin(airT * PI);
        p.joints[L_HIP]      = hipAngle;
        p.joints[R_HIP]      = hipAngle;
        p.joints[L_SHOULDER] = armAngle;
        p.joints[R_SHOULDER] = armAngle;
    } else {
        float landT = (phase - 0.65f) / 0.35f;
        p.bodyY = -0.06f * std::sin(landT * PI);
        float hipAngle  = 0.3f * std::sin(landT * PI);
        float kneeAngle = 0.4f * std::sin(landT * PI);
        p.joints[L_HIP]  = hipAngle;
        p.joints[R_HIP]  = hipAngle;
        p.joints[L_KNEE] = kneeAngle;
        p.joints[R_KNEE] = kneeAngle;
    }
    return p;
}
