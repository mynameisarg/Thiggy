#include "thiggy.h"
#include <iostream>
#include <GL/glew.h>

// -------------------- Line Shaders --------------------
const char* vertexShaderSrc = R"(
#version 330 core
layout(location = 0) in vec2 pos;
layout(location = 1) in vec3 color;
out vec3 vColor;

uniform float screenWidth;
uniform float screenHeight;

void main() {
    // Convert from pixels [0..screenWidth/screenHeight] to NDC [-1..1]
    float ndcX = (pos.x / screenWidth) * 2.0 - 1.0;
    float ndcY = 1.0 - (pos.y / screenHeight) * 2.0; // invert Y
    gl_Position = vec4(ndcX, ndcY, 0.0, 1.0);
    vColor = color;
}
)";

const char* fragmentShaderSrc = R"(
#version 330 core
in vec3 vColor;
out vec4 FragColor;
void main() {
    FragColor = vec4(vColor, 1.0);
}
)";

// -------------------- Quad Shaders --------------------
const char* quadVertexShaderSrc = R"(
#version 330 core
layout(location = 0) in vec2 pos;
layout(location = 1) in vec2 texCoord;
out vec2 vTexCoord;
void main() {
    vTexCoord = texCoord;
    gl_Position = vec4(pos, 0.0, 1.0);
}
)";

const char* quadFragmentShaderSrc = R"(
#version 330 core
in vec2 vTexCoord;
out vec4 FragColor;
uniform sampler2D screenTex;
void main() {
    FragColor = texture(screenTex, vTexCoord);
}
)";

// ======================================================
// Constructor
// ======================================================
Thiggy::Thiggy(int width, int height)
    : screenWidth(width), screenHeight(height),
      r(0), g(0), b(0), size(1.0f), isDown(false),
      lastX(0), lastY(0), hasLast(false)
{
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    glGenTextures(1, &penTexture);
    glBindTexture(GL_TEXTURE_2D, penTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, penTexture, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cerr << "FBO not complete!" << std::endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    float quadVertices[] = {
        // pos      // tex
        -1.0f, -1.0f, 0.0f, 0.0f,
         1.0f, -1.0f, 1.0f, 0.0f,
         1.0f,  1.0f, 1.0f, 1.0f,
        -1.0f,  1.0f, 0.0f, 1.0f
    };
    unsigned int quadIndices[] = { 0, 1, 2, 2, 3, 0 };

    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glGenBuffers(1, &quadEBO);

    glBindVertexArray(quadVAO);

    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), quadIndices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    initShaders();
}

// ======================================================
// Destructor
// ======================================================
Thiggy::~Thiggy() {
    glDeleteFramebuffers(1, &fbo);
    glDeleteTextures(1, &penTexture);
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
    glDeleteProgram(shaderProgram);

    glDeleteVertexArrays(1, &quadVAO);
    glDeleteBuffers(1, &quadVBO);
    glDeleteBuffers(1, &quadEBO);
    glDeleteProgram(quadShaderProgram);
}

// ======================================================
// Shader Init
// ======================================================
void Thiggy::initShaders() {
    auto compileShader = [](GLenum type, const char* src) -> GLuint {
        GLuint shader = glCreateShader(type);
        glShaderSource(shader, 1, &src, nullptr);
        glCompileShader(shader);
        GLint success;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            char info[512]; glGetShaderInfoLog(shader, 512, nullptr, info);
            std::cerr << "Shader compile error: " << info << std::endl;
        }
        return shader;
    };

    GLuint vs = compileShader(GL_VERTEX_SHADER, vertexShaderSrc);
    GLuint fs = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSrc);
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vs);
    glAttachShader(shaderProgram, fs);
    glLinkProgram(shaderProgram);
    GLint success;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) std::cerr << "Line shader link failed!\n";
    glDeleteShader(vs);
    glDeleteShader(fs);

    GLuint qvs = compileShader(GL_VERTEX_SHADER, quadVertexShaderSrc);
    GLuint qfs = compileShader(GL_FRAGMENT_SHADER, quadFragmentShaderSrc);
    quadShaderProgram = glCreateProgram();
    glAttachShader(quadShaderProgram, qvs);
    glAttachShader(quadShaderProgram, qfs);
    glLinkProgram(quadShaderProgram);
    glGetProgramiv(quadShaderProgram, GL_LINK_STATUS, &success);
    if (!success) std::cerr << "Quad shader link failed!\n";
    glDeleteShader(qvs);
    glDeleteShader(qfs);
}

// ======================================================
// Thiggy Functions
// ======================================================
void Thiggy::penDown() { isDown = true; }
void Thiggy::penUp() { isDown = false; }
void Thiggy::setColor(float R, float G, float B) { r = R; g = G; b = B; }
void Thiggy::setSize(float s) { size = s; }

void Thiggy::clear() {
    vertices.clear();
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glClearColor(1,1,1,0);
    glClear(GL_COLOR_BUFFER_BIT);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Thiggy::moveTo(float x, float y) {
    lastX = x; lastY = y; hasLast = true;
}

void Thiggy::lineTo(float x, float y) {
    if (!isDown || !hasLast) { moveTo(x, y); return; }
    vertices.push_back({lastX, lastY, r, g, b});
    vertices.push_back({x, y, r, g, b});
    lastX = x; lastY = y; hasLast = true;
}

// ======================================================
// Drawing
// ======================================================
void Thiggy::flushLines() {
    if (vertices.empty()) return;

    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glUseProgram(shaderProgram);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    // Upload vertex data to GPU
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_DYNAMIC_DRAW);

    // Set uniforms for screen size
    GLint locW = glGetUniformLocation(shaderProgram, "screenWidth");
    GLint locH = glGetUniformLocation(shaderProgram, "screenHeight");
    glUniform1f(locW, (float)screenWidth);
    glUniform1f(locH, (float)screenHeight);

    glLineWidth(size);
    glDrawArrays(GL_LINES, 0, vertices.size());

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glUseProgram(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    vertices.clear();
}

void Thiggy::render() {
    flushLines();

    glUseProgram(quadShaderProgram);
    glBindVertexArray(quadVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, penTexture);

    GLint texLoc = glGetUniformLocation(quadShaderProgram, "screenTex");
    glUniform1i(texLoc, 0);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
    glUseProgram(0);
}
