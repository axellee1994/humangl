#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <stdexcept>

#include "Animation.hpp"
#include "Math.hpp"
#include "Model.hpp"
#include "Shader.hpp"

static constexpr int WIN_W = 800;
static constexpr int WIN_H = 600;

static AnimType g_animType = AnimType::IDLE;
static bool     g_animChanged = false;

static void keyCallback(GLFWwindow *window, int key, int /*scancode*/, int action, int /*mods*/) {
    if (action != GLFW_PRESS)
        return;
    switch (key) {
        case GLFW_KEY_ESCAPE:
            glfwSetWindowShouldClose(window, GLFW_TRUE);
            break;
        case GLFW_KEY_W:
            g_animType    = AnimType::WALK;
            g_animChanged = true;
            break;
        case GLFW_KEY_J:
            g_animType    = AnimType::JUMP;
            g_animChanged = true;
            break;
        case GLFW_KEY_I:
            g_animType    = AnimType::IDLE;
            g_animChanged = true;
            break;
        default:
            break;
    }
}

static const char *animName(AnimType t) {
    switch (t) {
        case AnimType::WALK: return "Walk";
        case AnimType::JUMP: return "Jump";
        default:             return "Idle";
    }
}

int main() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialise GLFW\n";
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(WIN_W, WIN_H, "humangl — Idle", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, keyCallback);
    glfwSwapInterval(1);

    const GLubyte *glVer = glGetString(GL_VERSION);
    std::cout << "GL_VERSION before GLEW: " << (glVer ? (const char*)glVer : "NULL") << "\n";

    glewExperimental = GL_TRUE;
    GLenum glewErr = glewInit();
    if (glewErr != GLEW_OK && glewErr != GLEW_ERROR_NO_GLX_DISPLAY) {
        std::cerr << "Failed to initialise GLEW: " << glewGetErrorString(glewErr) << "\n";
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }

    glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, WIN_W, WIN_H);

    Shader    shader("shaders/vertex.glsl", "shaders/fragment.glsl");
    Model     model;
    Animation anim;

    vec3  camPos(0.f, 0.5f, 3.f);
    vec3  camTarget(0.f, 0.f, 0.f);
    vec3  camUp(0.f, 1.f, 0.f);

    mat4 view       = lookAt(camPos, camTarget, camUp);
    mat4 projection = perspective(
        0.785398f,                          // 45 deg in radians
        static_cast<float>(WIN_W) / WIN_H,
        0.1f,
        100.f
    );

    double lastTime = glfwGetTime();

    while (!glfwWindowShouldClose(window)) {
        double now = glfwGetTime();
        float  dt  = static_cast<float>(now - lastTime);
        lastTime   = now;

        if (g_animChanged) {
            anim.setType(g_animType);
            g_animChanged = false;

            std::string title = std::string("humangl — ") + animName(g_animType);
            glfwSetWindowTitle(window, title.c_str());
        }

        anim.update(dt);
        Pose pose = anim.getPose();

        glClearColor(0.12f, 0.12f, 0.15f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();
        shader.setMat4("view",       view);
        shader.setMat4("projection", projection);
        shader.setVec3("lightPos",   {3.f, 5.f, 3.f});

        model.draw(shader, pose);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
