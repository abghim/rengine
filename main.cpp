#include <SDL.h>
#include <cstdio>
#include <vector>
#include "core.hpp"

#define rad(x) ((x)*M_PI/180)

int main()
{
    /* tests -> vectors */
    mesh utah_mesh("tie.obj");

    Scene scene(&utah_mesh, 2, 4, 6, rad(17), rad(-10), rad(5), rad(80), 0.1, 1500, 1440, 830);
    screen &viewport1 = scene.display;

    vec3d camera_pos = scene.camera.getpos();
    vec3d camear_dir = scene.camera.getrot();

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


    SDL_Texture* texture = SDL_CreateTexture(
        ren,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        viewport1.width,
        viewport1.height
    );

    if (!texture) {
        fprintf(stderr, "texture creation error\n");
        SDL_DestroyRenderer(ren);
        SDL_DestroyWindow(win);
        SDL_Quit(); return 1;
    }

    std::vector<Uint32> frame_pixels(viewport1.width * viewport1.height);

    bool running = 1;
    while (running) {

        vec3d camera_pos = scene.camera.getpos();
        vec3d camera_dir = scene.camera.getrot();


        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT)
                running = false;
            else if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.scancode) {
                    case SDL_SCANCODE_S: scene.camera.updatepos(camera_pos.x, camera_pos.y, camera_pos.z+0.1); break;
                    case SDL_SCANCODE_W: scene.camera.updatepos(camera_pos.x, camera_pos.y, camera_pos.z-0.1); break;
                    case SDL_SCANCODE_Q: scene.camera.updatepos(camera_pos.x, camera_pos.y+0.1, camera_pos.z); break;
                    case SDL_SCANCODE_E: scene.camera.updatepos(camera_pos.x, camera_pos.y-0.1, camera_pos.z); break;
                    case SDL_SCANCODE_A: scene.camera.updatepos(camera_pos.x+0.1, camera_pos.y, camera_pos.z); break;
                    case SDL_SCANCODE_D: scene.camera.updatepos(camera_pos.x-0.1, camera_pos.y, camera_pos.z); break;

                    case SDL_SCANCODE_Z: scene.camera.updaterot(camera_dir.x, camera_dir.y, camera_dir.z+rad(1)); break;
                    case SDL_SCANCODE_X: scene.camera.updaterot(camera_dir.x, camera_dir.y, camera_dir.z-rad(1)); break;
                    case SDL_SCANCODE_UP: scene.camera.updaterot(camera_dir.x, camera_dir.y-rad(1), camera_dir.z); break;
                    case SDL_SCANCODE_DOWN: scene.camera.updaterot(camera_dir.x, camera_dir.y+rad(1), camera_dir.z); break;
                    case SDL_SCANCODE_RIGHT: scene.camera.updaterot(camera_dir.x+rad(1), camera_dir.y, camera_dir.z); break;
                    case SDL_SCANCODE_LEFT: scene.camera.updaterot(camera_dir.x-rad(1), camera_dir.y, camera_dir.z); break;

                    default: break;
                }
            }

        }

        SDL_SetRenderDrawColor(ren, 0, 0, 0, 25);
        SDL_RenderClear(ren);


/*
        for (triangle3d tri : scene.tris()) {
            SDL_Vertex element[3];
            SDL_Color  c = {(unsigned char)tri.color.r, (unsigned char)tri.color.g, (unsigned char)tri.color.b, 255};
            for (int i = 0; i < 3; ++i) {
                element[i].position = (SDL_FPoint) {(float)tri.p[i].x, (float)tri.p[i].y};
                element[i].color    = c;
                element[i].tex_coord = (SDL_FPoint){ 0, 0 };  // unused 
            }
            SDL_RenderGeometry(ren, NULL, element, 3, NULL, 0);
        }
	*/
		scene.update();
		for (int x=0; x<viewport1.width; x++) {
			for (int y=0; y<viewport1.height; y++) {
				rgb cl = viewport1.get(x, y).color;
				frame_pixels[y * viewport1.width + x] =
					0xFF000000u |
					(static_cast<Uint32>(cl.r) << 16) |
					(static_cast<Uint32>(cl.g) << 8) |
					 static_cast<Uint32>(cl.b);
			}
		}

		SDL_UpdateTexture(texture, NULL, frame_pixels.data(), viewport1.width * sizeof(Uint32));
		SDL_RenderCopy(ren, texture, NULL, NULL);

        SDL_RenderPresent(ren);
    }
	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(ren);
	SDL_DestroyWindow(win);
	SDL_Quit();
	return 0;
}
