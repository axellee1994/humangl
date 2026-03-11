#pragma once

enum JointIndex {
    TORSO_Y   = 0,
    TORSO_X   = 1,
    HEAD_Y    = 2,
    L_SHOULDER = 3,
    L_ELBOW   = 4,
    R_SHOULDER = 5,
    R_ELBOW   = 6,
    L_HIP     = 7,
    L_KNEE    = 8,
    R_HIP     = 9,
    R_KNEE    = 10,
    JOINT_COUNT = 11
};

enum class AnimType {
    IDLE,
    WALK,
    JUMP
};

struct Pose {
    float joints[JOINT_COUNT];
    float bodyY;

    Pose() : bodyY(0.f) {
        for (int i = 0; i < JOINT_COUNT; ++i)
            joints[i] = 0.f;
    }
};

class Animation {
public:
    Animation();

    void     update(float dt);
    Pose     getPose() const;
    void     setType(AnimType type);
    AnimType getType() const;

private:
    AnimType _type;
    float    _time;

    Pose _computeIdle()  const;
    Pose _computeWalk()  const;
    Pose _computeJump()  const;
};
