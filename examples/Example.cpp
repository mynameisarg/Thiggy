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
