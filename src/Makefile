SHELL := /bin/sh

CXX := clang++
SDL_CFLAGS := $(shell sdl2-config --cflags)
SDL_LIBS := $(shell sdl2-config --libs)

LUA_CFLAGS := $(shell pkg-config --cflags lua)
LUA_LIBS := $(shell pkg-config --libs lua)
CXXFLAGS := -Wno-deprecated-declarations

GREEN := \033[1;92m
WHITE := \033[0;97m
GREY := \033[0;90m
NC := \033[0m
CLEAR_LINE := \033[1A\033[2K\r

.PHONY: all clean run finished

all: rengine finished

core.o: core.cpp core.hpp
	@printf "$(GREEN)%12s$(NC) $(WHITE)%s$(NC)\n" "Compiling" "core.cpp"
	@printf "$(GREY)%s$(NC)\n" "$(CXX) $(CXXFLAGS) -c core.cpp"; \
	$(CXX) $(CXXFLAGS) -c core.cpp; status=$$?; \
	if [ $$status -eq 0 ]; then printf "$(CLEAR_LINE)"; else exit $$status; fi

main.o: main.cpp core.hpp core.cpp
	@printf "$(GREEN)%12s$(NC) $(WHITE)%s$(NC)\n" "Compiling" "main.cpp"
	@printf "$(GREY)%s$(NC)\n" "$(CXX) $(SDL_CFLAGS) $(CXXFLAGS)  $(LUA_CFLAGS) $(LUA_LIBS) -c main.cpp"; \
	$(CXX) $(SDL_CFLAGS) $(CXXFLAGS) $(LUA_CFLAGS) $(LUA_LIBS) -c main.cpp; status=$$?; \
	if [ $$status -eq 0 ]; then printf "$(CLEAR_LINE)"; else exit $$status; fi

rengine: main.o core.o
	@printf "$(GREEN)%12s$(NC) $(WHITE)%s$(NC)\n" "Linking" "rengine"
	@printf "$(GREY)%s$(NC)\n" "$(CXX) $(SDL_CFLAGS) $(SDL_LIBS) $(LUA_LIBS) main.o core.o -o rengine"; \
	$(CXX) $(SDL_CFLAGS) $(SDL_LIBS) $(LUA_LIBS) main.o core.o -o rengine; status=$$?; \
	if [ $$status -eq 0 ]; then printf "$(CLEAR_LINE)"; else exit $$status; fi

finished:
	@printf "$(GREEN)%12s$(NC) $(WHITE)%s$(NC)\n" "Finished" "rengine"

clean:
	@printf "$(GREEN)%12s$(NC) $(WHITE)%s$(NC)\n" "Removing" "build artifacts"
	@printf "$(GREY)%s$(NC)\n" "rm -f *.o rengine"; \
	rm -f *.o rengine; status=$$?; \
	if [ $$status -eq 0 ]; then printf "$(CLEAR_LINE)"; else exit $$status; fi

run: rengine
	@printf "$(GREEN)%12s$(NC) $(WHITE)%s$(NC)\n" "Running" "rengine"
	@printf "$(GREY)%s$(NC)\n" "./rengine"
	@./rengine
