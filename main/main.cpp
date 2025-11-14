#include <iostream>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL.h>

int main(int argc, char* argv[]) {
  if (SDL_Init(SDL_INIT_VIDEO)) {
		std::cout << "SDL initialized successfully." << std::endl;
  } else {
    std::cout << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
  }
	return 0;
}