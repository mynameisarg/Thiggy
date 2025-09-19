#pragma once
#include <GL/glew.h>
#include <vector>

class Thiggy {
public:
    Thiggy(int width, int height);
    ~Thiggy();

    void penDown();
    void penUp();
    void setColor(float R, float G, float B);
    void setSize(float s);
    void clear();
    void moveTo(float x, float y);
    void lineTo(float x, float y);
    void render(); // Automatically draws queued lines

private:
    void flushLines(); // Draw queued lines to FBO
    void initShaders(); // Compile shaders

    // Screen settings
    int screenWidth, screenHeight;
    float r, g, b;
    float size;
    bool isDown;
    float lastX, lastY;
    bool hasLast;

    // FBO + texture
    GLuint fbo;
    GLuint penTexture;

    // Line rendering
    GLuint vao;       // Vertex Array Object (lines)
    GLuint vbo;       // Vertex Buffer Object (lines)
    GLuint shaderProgram; // Line shader program

    struct Vertex { float x, y; float r, g, b; };
    std::vector<Vertex> vertices;

    // Fullscreen quad rendering
    GLuint quadVAO;
    GLuint quadVBO;
    GLuint quadEBO;
    GLuint quadShaderProgram;
};
