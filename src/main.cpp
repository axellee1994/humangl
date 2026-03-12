#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <stdexcept>
#include <string>

#include "Animation.hpp"
#include "Math.hpp"
#include "Model.hpp"
#include "Shader.hpp"
#include "TextRenderer.hpp"

static constexpr int WIN_W = 800;
static constexpr int WIN_H = 600;

static AnimType g_animType    = AnimType::IDLE;
static bool     g_animChanged = false;
static bool     g_titleDirty  = false;

enum PartSel {
    SEL_HEAD = 0,
    SEL_TORSO,
    SEL_UPPER_ARM,
    SEL_FOREARM,
    SEL_THIGH,
    SEL_LOWER_LEG,
    SEL_COUNT
};

static PartSel  g_part  = SEL_TORSO;
static Model   *g_model = nullptr;

static int g_fbW = WIN_W;
static int g_fbH = WIN_H;
static bool g_resized = false;

static void framebufferSizeCallback(GLFWwindow * /*window*/, int w, int h) {
    g_fbW     = w;
    g_fbH     = h;
    g_resized = true;
}

static const char *partName(PartSel p) {
    switch (p) {
        case SEL_HEAD:      return "Head";
        case SEL_TORSO:     return "Torso";
        case SEL_UPPER_ARM: return "Upper Arm";
        case SEL_FOREARM:   return "Forearm";
        case SEL_THIGH:     return "Thigh";
        case SEL_LOWER_LEG: return "Lower Leg";
        default:            return "?";
    }
}

static void resizePart(float factor) {
    if (!g_model) return;
    switch (g_part) {
        case SEL_HEAD:
            g_model->headS *= factor;
            break;
        case SEL_TORSO:
            g_model->torsoW *= factor;
            g_model->torsoH *= factor;
            g_model->torsoD *= factor;
            break;
        case SEL_UPPER_ARM:
            g_model->uArmW *= factor;
            g_model->uArmH *= factor;
            g_model->uArmD *= factor;
            break;
        case SEL_FOREARM:
            g_model->fArmW *= factor;
            g_model->fArmH *= factor;
            g_model->fArmD *= factor;
            break;
        case SEL_THIGH:
            g_model->thighW *= factor;
            g_model->thighH *= factor;
            g_model->thighD *= factor;
            break;
        case SEL_LOWER_LEG:
            g_model->lLegW *= factor;
            g_model->lLegH *= factor;
            g_model->lLegD *= factor;
            break;
        default:
            break;
    }
    g_titleDirty = true;
}

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
        case GLFW_KEY_1:
            g_part = SEL_HEAD;      g_titleDirty = true; break;
        case GLFW_KEY_2: g_part = SEL_TORSO;     g_titleDirty = true; break;
        case GLFW_KEY_3: g_part = SEL_UPPER_ARM; g_titleDirty = true; break;
        case GLFW_KEY_4: g_part = SEL_FOREARM;   g_titleDirty = true; break;
        case GLFW_KEY_5: g_part = SEL_THIGH;     g_titleDirty = true; break;
        case GLFW_KEY_6: g_part = SEL_LOWER_LEG; g_titleDirty = true; break;
        case GLFW_KEY_EQUAL: resizePart(1.1f);         break;
        case GLFW_KEY_MINUS: resizePart(1.f / 1.1f);   break;
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

static void updateTitle(GLFWwindow *window, AnimType anim) {
    std::string title = std::string("humangl — ")
                      + animName(anim)
                      + " | Part: " + partName(g_part)
                      + " (1-6 select, +/- resize)";
    glfwSetWindowTitle(window, title.c_str());
}

int main() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialise GLFW\n";
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(WIN_W, WIN_H, "humangl", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, keyCallback);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
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

    glfwGetFramebufferSize(window, &g_fbW, &g_fbH);

    glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, g_fbW, g_fbH);

    Shader       shader("shaders/vertex.glsl", "shaders/fragment.glsl");
    TextRenderer text;
    text.init(g_fbW, g_fbH);
    Model     model;
    Animation anim;

    g_model = &model;

    updateTitle(window, g_animType);

    vec3 camPos(0.f, 0.5f, 3.f);
    vec3 camTarget(0.f, 0.f, 0.f);
    vec3 camUp(0.f, 1.f, 0.f);

    mat4 view       = lookAt(camPos, camTarget, camUp);
    mat4 projection = perspective(
        0.785398f,
        static_cast<float>(g_fbW) / static_cast<float>(g_fbH),
        0.1f,
        100.f
    );

    double lastTime = glfwGetTime();

    while (!glfwWindowShouldClose(window)) {
        double now = glfwGetTime();
        float  dt  = static_cast<float>(now - lastTime);
        lastTime   = now;

        if (g_resized) {
            glViewport(0, 0, g_fbW, g_fbH);
            projection = perspective(
                0.785398f,
                static_cast<float>(g_fbW) / static_cast<float>(g_fbH),
                0.1f,
                100.f
            );
            text.cleanup();
            text.init(g_fbW, g_fbH);
            g_resized = false;
        }

        if (g_animChanged) {
            anim.setType(g_animType);
            g_animChanged = false;
            g_titleDirty  = true;
        }

        if (g_titleDirty) {
            updateTitle(window, g_animType);
            g_titleDirty = false;
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

        glDisable(GL_DEPTH_TEST);
        const float X  = 10.f;
        float       Y  = 10.f;
        const float dy = 18.f;
        const float KR = 1.0f, KG = 0.85f, KB = 0.2f;
        const float TR = 0.9f, TG = 0.9f, TB = 0.9f;
        text.drawString("ANIMATIONS",            X, Y, TR, TG, TB); Y += dy;
        text.drawString("W", X, Y, KR, KG, KB);
        text.drawString(" - Walk",         X+16, Y, TR, TG, TB); Y += dy;
        text.drawString("I", X, Y, KR, KG, KB);
        text.drawString(" - Idle",         X+16, Y, TR, TG, TB); Y += dy;
        text.drawString("J", X, Y, KR, KG, KB);
        text.drawString(" - Jump",         X+16, Y, TR, TG, TB); Y += dy;
        Y += 4.f;
        text.drawString("BODY PARTS",           X, Y, TR, TG, TB); Y += dy;
        text.drawString("1", X, Y, KR, KG, KB);
        text.drawString(" - Head",         X+16, Y, TR, TG, TB); Y += dy;
        text.drawString("2", X, Y, KR, KG, KB);
        text.drawString(" - Torso",        X+16, Y, TR, TG, TB); Y += dy;
        text.drawString("3", X, Y, KR, KG, KB);
        text.drawString(" - Upper Arm",    X+16, Y, TR, TG, TB); Y += dy;
        text.drawString("4", X, Y, KR, KG, KB);
        text.drawString(" - Forearm",      X+16, Y, TR, TG, TB); Y += dy;
        text.drawString("5", X, Y, KR, KG, KB);
        text.drawString(" - Thigh",        X+16, Y, TR, TG, TB); Y += dy;
        text.drawString("6", X, Y, KR, KG, KB);
        text.drawString(" - Lower Leg",    X+16, Y, TR, TG, TB); Y += dy;
        Y += 4.f;
        text.drawString("RESIZE PART",          X, Y, TR, TG, TB); Y += dy;
        text.drawString("+", X, Y, KR, KG, KB);
        text.drawString(" - Enlarge",      X+16, Y, TR, TG, TB); Y += dy;
        text.drawString("-", X, Y, KR, KG, KB);
        text.drawString(" - Shrink",       X+16, Y, TR, TG, TB); Y += dy;
        Y += 4.f;
        text.drawString("ESC", X, Y, KR, KG, KB);
        text.drawString(" - Quit",         X+48, Y, TR, TG, TB);
        glEnable(GL_DEPTH_TEST);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    g_model = nullptr;
    text.cleanup();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
