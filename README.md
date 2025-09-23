# Thiggy

Thiggy is a lightweight OpenGL-based drawing library built with **SDL2** and **GLEW**.  
It provides a simple "pen-and-paper" style API for rendering lines and drawings to the screen.  

---

## Features

- Pen-style drawing system (`penDown`, `penUp`, `moveTo`, `lineTo`)
- Customizable pen color and size
- Automatic batching of lines for efficient rendering
- Framebuffer-backed drawing (persistent canvas)
- Simple shader-based rendering pipeline

---

## Dependencies

Thiggy requires the following libraries:

- [SDL2](https://github.com/libsdl-org/SDL) (for window/context creation)
- [GLEW](http://glew.sourceforge.net/) (for managing OpenGL extensions)
- OpenGL 3.3+ (core profile recommended)

---

## Building

Make sure you have **SDL2** and **GLEW** installed on your system.  
Then compile your project with:
### Linux:
```bash
g++ main.cpp Thiggy.cpp -o app -lSDL2 -lGLEW -lGL
```
### Windows:
```batch
g++ main.cpp Thiggy.cpp -o app.exe -lmingw32 -lSDL2main -lSDL2 -lglew32 -lopengl32
```
___

# Example

```c++
#include <SDL2/SDL.h>
#include "Thiggy.h"

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window* window = SDL_CreateWindow("Thiggy Demo",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        800, 600, SDL_WINDOW_OPENGL);

    SDL_GLContext context = SDL_GL_CreateContext(window);

    glewInit();

    Thiggy thiggy(800, 600);
    thiggy.setColor(1.0f, 0.0f, 0.0f); // Red
    thiggy.setSize(2.0f);

    bool running = true;
    SDL_Event e;

    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = false;
        }

        thiggy.penDown();
        thiggy.moveTo(100, 100);
        thiggy.lineTo(700, 500);
        thiggy.penUp();

        thiggy.render();

        SDL_GL_SwapWindow(window);
    }

    SDL_Quit();
    return 0;
}

```
---

# API Overview

```cpp
Thiggy(int width, int height);   // Create drawing context
~Thiggy();                       // Cleanup resources

void penDown();                  // Start drawing
void penUp();                    // Stop drawing
void setColor(float R, float G, float B); // Set pen color
void setSize(float s);           // Set pen size
void clear();                    // Clear canvas
void moveTo(float x, float y);   // Move pen to position
void lineTo(float x, float y);   // Draw line to position
void render();                   // Render queued lines

```
---

# This uses the Mozilla Public License Version 2.0
