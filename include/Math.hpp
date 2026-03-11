#pragma once

#include <cmath>
#include <cstring>

struct vec3 {
    float x, y, z;
    vec3() : x(0.f), y(0.f), z(0.f) {}
    vec3(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}
};

inline vec3 operator+(const vec3 &a, const vec3 &b) { return {a.x+b.x, a.y+b.y, a.z+b.z}; }
inline vec3 operator-(const vec3 &a, const vec3 &b) { return {a.x-b.x, a.y-b.y, a.z-b.z}; }
inline vec3 operator*(const vec3 &a, float s)        { return {a.x*s,   a.y*s,   a.z*s};   }
inline vec3 operator*(float s, const vec3 &a)        { return a * s; }

inline float dot(const vec3 &a, const vec3 &b) {
    return a.x*b.x + a.y*b.y + a.z*b.z;
}

inline vec3 cross(const vec3 &a, const vec3 &b) {
    return { a.y*b.z - a.z*b.y,
             a.z*b.x - a.x*b.z,
             a.x*b.y - a.y*b.x };
}

inline vec3 normalize(const vec3 &a) {
    float len = std::sqrt(dot(a, a));
    if (len < 1e-8f) return {0.f, 0.f, 0.f};
    return a * (1.f / len);
}

struct mat4 {
    float m[16];

    mat4() { std::memset(m, 0, sizeof(m)); }

    float  operator[](int i) const { return m[i]; }
    float &operator[](int i)       { return m[i]; }
};

inline mat4 operator*(const mat4 &a, const mat4 &b) {
    mat4 r;
    for (int col = 0; col < 4; ++col)
        for (int row = 0; row < 4; ++row) {
            float s = 0.f;
            for (int k = 0; k < 4; ++k)
                s += a.m[k*4 + row] * b.m[col*4 + k];
            r.m[col*4 + row] = s;
        }
    return r;
}

inline mat4 identity() {
    mat4 r;
    r.m[0] = r.m[5] = r.m[10] = r.m[15] = 1.f;
    return r;
}

inline mat4 translate(float tx, float ty, float tz) {
    mat4 r = identity();
    r.m[12] = tx;
    r.m[13] = ty;
    r.m[14] = tz;
    return r;
}

inline mat4 scale(float sx, float sy, float sz) {
    mat4 r = identity();
    r.m[0]  = sx;
    r.m[5]  = sy;
    r.m[10] = sz;
    return r;
}

inline mat4 rotateX(float angle) {
    float c = std::cos(angle), s = std::sin(angle);
    mat4 r = identity();
    r.m[5]  =  c;  r.m[9]  = -s;
    r.m[6]  =  s;  r.m[10] =  c;
    return r;
}

inline mat4 rotateY(float angle) {
    float c = std::cos(angle), s = std::sin(angle);
    mat4 r = identity();
    r.m[0]  =  c;  r.m[8]  =  s;
    r.m[2]  = -s;  r.m[10] =  c;
    return r;
}

inline mat4 rotateZ(float angle) {
    float c = std::cos(angle), s = std::sin(angle);
    mat4 r = identity();
    r.m[0]  =  c;  r.m[4]  = -s;
    r.m[1]  =  s;  r.m[5]  =  c;
    return r;
}

inline mat4 perspective(float fovY, float aspect, float zNear, float zFar) {
    float f = 1.f / std::tan(fovY * 0.5f);
    mat4 r;
    r.m[0]  = f / aspect;
    r.m[5]  = f;
    r.m[10] = (zFar + zNear) / (zNear - zFar);
    r.m[11] = -1.f;
    r.m[14] = (2.f * zFar * zNear) / (zNear - zFar);
    return r;
}

inline mat4 lookAt(const vec3 &eye, const vec3 &center, const vec3 &up) {
    vec3 f = normalize(center - eye);
    vec3 s = normalize(cross(f, up));
    vec3 u = cross(s, f);

    mat4 r = identity();
    r.m[0]  =  s.x;  r.m[4]  =  s.y;  r.m[8]  =  s.z;
    r.m[1]  =  u.x;  r.m[5]  =  u.y;  r.m[9]  =  u.z;
    r.m[2]  = -f.x;  r.m[6]  = -f.y;  r.m[10] = -f.z;
    r.m[12] = -dot(s, eye);
    r.m[13] = -dot(u, eye);
    r.m[14] =  dot(f, eye);
    return r;
}
