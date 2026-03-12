#pragma once

#include <cmath>
#include <cstring>

struct vec3 {
    float x, y, z;
    vec3() : x(0.f), y(0.f), z(0.f) {}
    vec3(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}
};

vec3 operator+(const vec3 &a, const vec3 &b);
vec3 operator-(const vec3 &a, const vec3 &b);
vec3 operator*(const vec3 &a, float s);
vec3 operator*(float s, const vec3 &a);

float dot(const vec3 &a, const vec3 &b);
vec3  cross(const vec3 &a, const vec3 &b);
vec3  normalize(const vec3 &a);

struct mat4 {
    float m[16];

    mat4() { std::memset(m, 0, sizeof(m)); }

    float  operator[](int i) const { return m[i]; }
    float &operator[](int i)       { return m[i]; }
};

mat4 operator*(const mat4 &a, const mat4 &b);

mat4 identity();
mat4 translate(float tx, float ty, float tz);
mat4 scale(float sx, float sy, float sz);
mat4 rotateX(float angle);
mat4 rotateY(float angle);
mat4 rotateZ(float angle);
mat4 perspective(float fovY, float aspect, float zNear, float zFar);
mat4 lookAt(const vec3 &eye, const vec3 &center, const vec3 &up);
