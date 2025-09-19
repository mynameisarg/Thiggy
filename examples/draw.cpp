#include <SDL2/SDL.h>
#include <GL/glew.h>
#include "thiggy.h"

//bool LeftMouseDown(){    return SDL_GetMouseState(NULL, NULL)&SDL_BUTTON(0);}

int main(int argc, char* argv[]) {
    int buttons = SDL_GetMouseState(NULL, NULL);

    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("Thiggy Example 1",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        800, 600, SDL_WINDOW_OPENGL);

    SDL_GLContext glContext = SDL_GL_CreateContext(window);
    glewInit();
    glClearColor(1, 1, 1, 0);

    Thiggy pen(800, 600);
    pen.setColor(0, 0, 1); // blue
    
    int penSize = 1;

    bool running = true;
    SDL_Event e;
    int x = 50, y = 50;

    pen.penDown();

    pen.lineTo(50,50);
    pen.lineTo(50,25);
    pen.lineTo(25,25);
    pen.lineTo(25,50);
    pen.lineTo(50,50);

    pen.penUp();
    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = false;
            switch (e.type) {
                case SDL_MOUSEWHEEL:
                    penSize += e.wheel.y;
                break;
            }
        }
        Uint32 buttons = SDL_GetMouseState(&x, &y);

        pen.setSize(penSize);

        pen.lineTo(x,y);
        
        if (buttons & SDL_BUTTON(SDL_BUTTON_LEFT)) {
            pen.penDown();
        } else {
            pen.penUp();
        }
        //pen.penDown();
        //x += 1;
        //y += 0.5;
        //pen.lineTo(x, y);

        // Clear screen
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT);
        

        // Draw pen canvas
        pen.render();

        SDL_GL_SwapWindow(window);
        SDL_Delay(16);
    }

    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
