extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

#include <iostream>
#include <SDL.h>
#include <cstdio>
#include "core.hpp"

#define rad(x) ((x)*M_PI/180)

#define LUA_FILE "scripts/destroyer_flyover.lua"

Scene *sceneptr;

static int rengine_new_actor(lua_State *L) {
	const char *filename = luaL_checkstring(L, 1);
	ifstream f(filename);
	if (!f.good()) {
		return luaL_error(L, "cannot open mesh file '%s'", filename);
	}
	sceneptr->addmesh(filename);
	return 0;
}

static int rengine_set_campos(lua_State *L) {
	double x = luaL_checknumber(L, 1);
	double y = luaL_checknumber(L, 2);
	double z = luaL_checknumber(L, 3);

	sceneptr->camera.updatepos(x, y, z);
	return 0;
}


static int rengine_set_camrot(lua_State *L) {
	double x = luaL_checknumber(L, 1);
	double y = luaL_checknumber(L, 2);
	double z = luaL_checknumber(L, 3);

	sceneptr->camera.updaterot(x, y, z);
	return 0;
}


static int rengine_set_campos_delta(lua_State *L) {
	double x = luaL_checknumber(L, 1);
	double y = luaL_checknumber(L, 2);
	double z = luaL_checknumber(L, 3);

	sceneptr->camera.updatepos(sceneptr->camera.getpos() + vec3d(x, y, z));
	return 0;
}


static int rengine_set_camrot_delta(lua_State *L) {
	double x = luaL_checknumber(L, 1);
	double y = luaL_checknumber(L, 2);
	double z = luaL_checknumber(L, 3);

	sceneptr->camera.updaterot(sceneptr->camera.getrot() + vec3d(x, y, z));
	return 0;
}


static actor &rengine_get_actor(lua_State *L, int i) {
	int actor_index = (int) luaL_checkinteger(L, i);
	if (actor_index < 0 || actor_index >= (int) sceneptr->actors.size()) {
		luaL_error(L, "actor index %d out of range", actor_index);
	}
	return sceneptr->actors[actor_index];
}


static int rengine_set_actorpos(lua_State *L) {
	actor &target = rengine_get_actor(L, 1);
	double x = luaL_checknumber(L, 2);
	double y = luaL_checknumber(L, 3);
	double z = luaL_checknumber(L, 4);

	target.translate = vec3d(x, y, z);
	return 0;
}


static int rengine_set_actorrot(lua_State *L) {
	actor &target = rengine_get_actor(L, 1);
	double x = luaL_checknumber(L, 2);
	double y = luaL_checknumber(L, 3);
	double z = luaL_checknumber(L, 4);

	target.rotate = vec3d(x, y, z);
	return 0;
}


static int rengine_set_actorpos_delta(lua_State *L) {
	actor &target = rengine_get_actor(L, 1);
	double x = luaL_checknumber(L, 2);
	double y = luaL_checknumber(L, 3);
	double z = luaL_checknumber(L, 4);

	target.translate = target.translate + vec3d(x, y, z);
	return 0;
}


static int rengine_set_actorrot_delta(lua_State *L) {
	actor &target = rengine_get_actor(L, 1);
	double x = luaL_checknumber(L, 2);
	double y = luaL_checknumber(L, 3);
	double z = luaL_checknumber(L, 4);

	target.rotate = target.rotate + vec3d(x, y, z);
	return 0;
}


static bool rengine_call_lua_hook(lua_State *L, const char *name) {
	lua_getglobal(L, name);
	if (lua_isnil(L, -1)) {
		lua_pop(L, 1);
		return true;
	}
	if (!lua_isfunction(L, -1)) {
		std::cerr << "Lua global '" << name << "' is not a function.\n";
		lua_pop(L, 1);
		return false;
	}
	if (lua_pcall(L, 0, 0, 0) != LUA_OK) {
		std::cerr << "Lua " << name << " function failed: " << lua_tostring(L, -1) << "\n";
		lua_pop(L, 1);
		return false;
	}
	return true;
}


static bool rengine_call_keybind(lua_State *L, int scancode, bool shift) {
	lua_getglobal(L, "keybind");
	if (lua_isnil(L, -1)) {
		lua_pop(L, 1);
		return true;
	}
	if (!lua_isfunction(L, -1)) {
		std::cerr << "Lua global 'keybind' is not a function.\n";
		lua_pop(L, 1);
		return false;
	}

	lua_pushinteger(L, scancode);
	lua_pushboolean(L, shift);
	if (lua_pcall(L, 2, 0, 0) != LUA_OK) {
		std::cerr << "Lua keybind function failed: " << lua_tostring(L, -1) << "\n";
		lua_pop(L, 1);
		return false;
	}
	return true;
}


static bool rengine_call_update(lua_State *L, double delta_t) {
	lua_getglobal(L, "update");
	if (lua_isnil(L, -1)) {
		lua_pop(L, 1);
		return true;
	}
	if (!lua_isfunction(L, -1)) {
		std::cerr << "Lua global 'update' is not a function.\n";
		lua_pop(L, 1);
		return false;
	}

	lua_pushnumber(L, delta_t);
	if (lua_pcall(L, 1, 0, 0) != LUA_OK) {
		std::cerr << "Lua update function failed: " << lua_tostring(L, -1) << "\n";
		lua_pop(L, 1);
		return false;
	}
	return true;
}


int main()
{
    /* tests -> vectors */
	lua_State *L = luaL_newstate();
	luaL_openlibs(L);

	/* build the api */

	lua_newtable(L);

	lua_newtable(L);
	lua_pushcfunction(L, rengine_new_actor);
	lua_setfield(L, -2, "new");
	lua_pushcfunction(L, rengine_set_actorpos);
	lua_setfield(L, -2, "setpos");
	lua_pushcfunction(L, rengine_set_actorrot);
	lua_setfield(L, -2, "setrot");
	lua_pushcfunction(L, rengine_set_actorpos_delta);
	lua_setfield(L, -2, "setpos_delta");
	lua_pushcfunction(L, rengine_set_actorrot_delta);
	lua_setfield(L, -2, "setrot_delta");
	lua_setfield(L, -2, "actor");

	lua_newtable(L);
	lua_pushcfunction(L, rengine_set_campos);
	lua_setfield(L, -2, "setpos");
	lua_pushcfunction(L, rengine_set_camrot);
	lua_setfield(L, -2, "setrot");
	lua_pushcfunction(L, rengine_set_campos_delta);
	lua_setfield(L, -2, "setpos_delta");
	lua_pushcfunction(L, rengine_set_camrot_delta);
	lua_setfield(L, -2, "setrot_delta");
	lua_setfield(L, -2, "camera");

	lua_newtable(L);
#define SET_SDLKEY(name) \
	lua_pushinteger(L, SDL_SCANCODE_##name); \
	lua_setfield(L, -2, #name)
	SET_SDLKEY(A);
	SET_SDLKEY(B);
	SET_SDLKEY(C);
	SET_SDLKEY(D);
	SET_SDLKEY(E);
	SET_SDLKEY(F);
	SET_SDLKEY(G);
	SET_SDLKEY(H);
	SET_SDLKEY(I);
	SET_SDLKEY(J);
	SET_SDLKEY(K);
	SET_SDLKEY(L);
	SET_SDLKEY(M);
	SET_SDLKEY(N);
	SET_SDLKEY(O);
	SET_SDLKEY(P);
	SET_SDLKEY(Q);
	SET_SDLKEY(R);
	SET_SDLKEY(S);
	SET_SDLKEY(T);
	SET_SDLKEY(U);
	SET_SDLKEY(V);
	SET_SDLKEY(W);
	SET_SDLKEY(X);
	SET_SDLKEY(Y);
	SET_SDLKEY(Z);
#undef SET_SDLKEY
	lua_pushinteger(L, SDL_SCANCODE_UP);
	lua_setfield(L, -2, "UP");
	lua_pushinteger(L, SDL_SCANCODE_DOWN);
	lua_setfield(L, -2, "DOWN");
	lua_pushinteger(L, SDL_SCANCODE_LEFT);
	lua_setfield(L, -2, "LEFT");
	lua_pushinteger(L, SDL_SCANCODE_RIGHT);
	lua_setfield(L, -2, "RIGHT");
	lua_setfield(L, -2, "sdlkeys");

	lua_setglobal(L, "rengine");

	if (luaL_dofile(L, LUA_FILE) != LUA_OK) {
		std::cerr << "Failed to load " << LUA_FILE << ": " << lua_tostring(L, -1) << "\n";
		lua_close(L);
		return 1;
	}

    Scene scene(2, 4, 6, rad(17), rad(-10), rad(5), rad(80), 0.1, 1500, 1440, 830);
    screen &viewport1 = scene.display;

	sceneptr = &scene;

	if (!rengine_call_lua_hook(L, "init")) {
		lua_close(L);
		return 1;
	}

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
    double perf_freq = (double) SDL_GetPerformanceFrequency();
    Uint64 last_counter = SDL_GetPerformanceCounter();

    bool running = 1;
    while (running) {
        Uint64 now = SDL_GetPerformanceCounter();
        double delta_t = (double) (now - last_counter) / perf_freq;
        last_counter = now;
        if (delta_t > 0.1) delta_t = 0.1;

        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT)
                running = false;
            else if (e.type == SDL_KEYDOWN) {
                bool shift = (e.key.keysym.mod & KMOD_SHIFT) != 0;
				if (!rengine_call_keybind(L, e.key.keysym.scancode, shift)) {
					lua_close(L);
					SDL_DestroyTexture(texture);
					SDL_DestroyRenderer(ren);
					SDL_DestroyWindow(win);
					SDL_Quit();
					return 1;
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

		if (!rengine_call_update(L, delta_t)) {
			lua_close(L);
			return 1;
		}




		scene.update();
		int pixel_count = viewport1.width * viewport1.height;
		for (int i = 0; i < pixel_count; i++) {
			rgb cl = viewport1.data[i].color;
			frame_pixels[i] =
				0xFF000000u |
				(static_cast<Uint32>(cl.r) << 16) |
				(static_cast<Uint32>(cl.g) << 8) |
				 static_cast<Uint32>(cl.b);
		}

		SDL_UpdateTexture(texture, NULL, frame_pixels.data(), viewport1.width * sizeof(Uint32));
		SDL_RenderCopy(ren, texture, NULL, NULL);

        SDL_RenderPresent(ren);
    }

	lua_close(L);
	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(ren);
	SDL_DestroyWindow(win);
	SDL_Quit();
	return 0;
}
