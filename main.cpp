#include <SDL.h>
#include <cstdio>
#include "core.hpp"

int main()
{
    /* tests -> vectors */
    mesh utah_mesh("utah_teapot.obj");

    Scene scene(&utah_mesh, 1, 2, 3, 0, 0.0, 0, M_PI/2, 0.1, 1500, 800, 600);
    screen viewport1(800, 600);

    // scene.frame(viewport1);
    // (scene.object.model)->print();
    //
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "video init error\n");
        SDL_Quit(); return 1;
    }

    SDL_Window *win  = SDL_CreateWindow("rengine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, viewport1.width, viewport1.height, SDL_WINDOW_SHOWN);
    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (!win || !ren) {
        fprintf(stderr, "window/renderer error\n");

        SDL_Quit(); return 1;
    }

    bool running = 1;
    while (running) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT)
                running = false;         /* toggle colour */
        }

        SDL_SetRenderDrawColor(ren, 0, 0, 0, 25);
        SDL_RenderClear(ren);


        for (triangle3d tri : scene.tris()) {
            SDL_Vertex element[3];
            SDL_Color  c = {(unsigned char)tri.color.r, (unsigned char)tri.color.g, (unsigned char)tri.color.b, 255};
            for (int i = 0; i < 3; ++i) {
                element[i].position = (SDL_FPoint) {(float)tri.p[i].x, (float)tri.p[i].y};
                element[i].color    = c;
                element[i].tex_coord = (SDL_FPoint){ 0, 0 };    /* unused */
            }
            SDL_RenderGeometry(ren, /*texture*/NULL, element, 3, NULL, 0);
        }

        SDL_RenderPresent(ren);
    }
	return 0;
}
