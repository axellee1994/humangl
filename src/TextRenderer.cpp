#include "TextRenderer.hpp"
#include <fstream>
#include <sstream>
#include <stdexcept>

void TextRenderer::init(int screenW, int screenH) {
    m_sw = screenW;
    m_sh = screenH;
    buildShader();
    buildFontTexture();
    buildQuadBuffers();
}

void TextRenderer::drawString(const std::string &text, float px, float py,
                               float r, float g, float b, int scale) const {
    glUseProgram(m_prog);

    GLint locScreen = glGetUniformLocation(m_prog, "screenSize");
    GLint locFont   = glGetUniformLocation(m_prog, "fontTex");
    GLint locColor  = glGetUniformLocation(m_prog, "textColor");

    glUniform2f(locScreen, static_cast<float>(m_sw), static_cast<float>(m_sh));
    glUniform1i(locFont, 0);
    glUniform3f(locColor, r, g, b);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_tex);

    glBindVertexArray(m_vao);

    float cw = static_cast<float>(CELL * scale);
    float ch = static_cast<float>(CELL * scale);

    float cx = px;
    for (char c : text) {
        if (c == '\n') { cx = px; py += ch; continue; }
        int idx = static_cast<unsigned char>(c) - 34;
        if (idx < 0 || idx >= 96) idx = 0;

        float u0 = static_cast<float>(idx)     / 96.f;
        float u1 = static_cast<float>(idx + 1) / 96.f;
        float v0 = 0.f;
        float v1 = 1.f;

        float x0 = cx, x1 = cx + cw;
        float y0 = py, y1 = py + ch;

        float verts[24] = {
            x0, y0, u0, v0,
            x1, y0, u1, v0,
            x1, y1, u1, v1,
            x0, y0, u0, v0,
            x1, y1, u1, v1,
            x0, y1, u0, v1,
        };
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(verts), verts);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        cx += cw;
    }

    glBindVertexArray(0);
    glUseProgram(0);
}

void TextRenderer::cleanup() {
    glDeleteTextures(1, &m_tex);
    glDeleteBuffers(1, &m_vbo);
    glDeleteVertexArrays(1, &m_vao);
    glDeleteProgram(m_prog);
}

static std::string readFile(const std::string &path) {
    std::ifstream file(path);
    if (!file.is_open())
        throw std::runtime_error("TextRenderer: cannot open " + path);
    std::ostringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

GLuint TextRenderer::compileShader(GLenum type, const char *src) {
    GLuint s = glCreateShader(type);
    glShaderSource(s, 1, &src, nullptr);
    glCompileShader(s);
    return s;
}

void TextRenderer::buildShader() {
    std::string vertSrc = readFile("shaders/hud.vert");
    std::string fragSrc = readFile("shaders/hud.frag");

    GLuint vs = compileShader(GL_VERTEX_SHADER,   vertSrc.c_str());
    GLuint fs = compileShader(GL_FRAGMENT_SHADER, fragSrc.c_str());
    m_prog = glCreateProgram();
    glAttachShader(m_prog, vs);
    glAttachShader(m_prog, fs);
    glLinkProgram(m_prog);
    glDeleteShader(vs);
    glDeleteShader(fs);
}

void TextRenderer::buildFontTexture() {
    static uint8_t atlas[8][96 * CELL];
    for (int ci = 0; ci < 96; ++ci) {
        for (int row = 0; row < CELL; ++row) {
            uint8_t byte = FONT8[ci][row];
            for (int bit = 0; bit < CELL; ++bit) {
                bool on = (byte >> (7 - bit)) & 1;
                atlas[row][ci * CELL + bit] = on ? 0xFF : 0x00;
            }
        }
    }

    glGenTextures(1, &m_tex);
    glBindTexture(GL_TEXTURE_2D, m_tex);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 96 * CELL, CELL,
                 0, GL_RED, GL_UNSIGNED_BYTE, atlas);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void TextRenderer::buildQuadBuffers() {
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                          reinterpret_cast<void*>(0));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                          reinterpret_cast<void*>(2 * sizeof(float)));

    glBindVertexArray(0);
}
