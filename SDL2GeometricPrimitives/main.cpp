#include "core.h"


int main( int argc, char* args[] )
{
	SDLBox box{};

	box.PushPrimitive(std::make_unique<Triangle>(SDL_FPoint{ 300, 100 }, SDL_FPoint{ 50, 250 }, SDL_FPoint{ 400, 250 }));
	box.PushPrimitive(std::make_unique<Triangle>(SDL_FPoint{ 500, 100 }, SDL_FPoint{ 400, 250 }, SDL_FPoint{ 600, 250 }, SDL_Color{255, 70, 0}));
	
	box.PushPrimitive(std::make_unique<Rectangle>(100, 100, 600, 600, SDL_Color{ 122, 255, 66 }));
	box.PushPrimitive(std::make_unique<Rectangle>(400, 100, 200, 400, SDL_Color{ 64, 0, 128 }));
	
	box.PushPrimitive(std::make_unique<Circle>(400, 800, 100, SDL_Color{ 255, 128, 192 }));
	box.PushPrimitive(std::make_unique<Circle>(800, 800, 50));

	box.Run();

	return 0;
}
