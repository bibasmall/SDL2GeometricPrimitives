#pragma once
#include <vector>
#include <string>
#include <memory>
#include <array>
#include <tuple>
#include <SDL.h>
#include <assert.h>
#include <SDL_ttf.h>
#include <functional>


class IPrimitive
{
public:
	virtual ~IPrimitive() {}

public:
	virtual void Draw(SDL_Renderer*) const = 0;
};

class Triangle : public IPrimitive
{
public:
	Triangle(SDL_FPoint x, SDL_FPoint y, SDL_FPoint z) : IPrimitive(), verts{ SDL_Vertex{ x, SDL_Color{ 255, 0, 0, 255 }, SDL_FPoint{ 0 } },
																			  SDL_Vertex{ y, SDL_Color{ 0, 0, 255, 255 }, SDL_FPoint{ 0 } },
																			  SDL_Vertex{ z, SDL_Color{ 0, 255, 0, 255 }, SDL_FPoint{ 0 } } }
	{
		
	}
	Triangle(SDL_FPoint x, SDL_FPoint y, SDL_FPoint z, SDL_Color color) : IPrimitive(), verts{ SDL_Vertex{ x, SDL_Color{ color }, SDL_FPoint{ 0 } },
																							   SDL_Vertex{ y, SDL_Color{ color }, SDL_FPoint{ 0 } },
																							   SDL_Vertex{ z, SDL_Color{ color }, SDL_FPoint{ 0 } } } 
	{
	
	}
	Triangle(SDL_FPoint x, SDL_FPoint y, SDL_FPoint z, SDL_Color xc, SDL_Color yc, SDL_Color zc)
		: IPrimitive(), verts{	SDL_Vertex{ x, SDL_Color{ xc }, SDL_FPoint{ 0 } }, 
								SDL_Vertex{ y, SDL_Color{ yc }, SDL_FPoint{ 0 } },
								SDL_Vertex{ z, SDL_Color{ zc }, SDL_FPoint{ 0 } } } 
	{

	}

	~Triangle() = default;


public:
	void Draw(SDL_Renderer* renderer) const override
	{
		assert(renderer);
		SDL_RenderGeometry(renderer, nullptr, verts.data(), verts.size(), nullptr, 0);
	}

private:
	std::array<SDL_Vertex, 3> verts;
};

class Rectangle : public IPrimitive
{
public:
	Rectangle() = delete;
	Rectangle(int w, int h, int x, int y, SDL_Color color = { 255, 255, 255, 255 }) : rect{.x = x, .y = y, .w = w, .h = h}, color{color} 
	{

	}
	~Rectangle() = default;

public:
	void Draw(SDL_Renderer* renderer) const override
	{
		assert(renderer);
		SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
		SDL_RenderFillRect(renderer, &rect);
	}

private:
	SDL_Rect  rect;
	SDL_Color color;
};

class Circle : public IPrimitive
{
	struct Point
	{
		int x;
		int y;
	};

public:
	Circle() = delete;
	Circle(int x, int y, int radius, SDL_Color color = { 255, 255, 255, 255 }) : center{x, y}, radius{radius}, color{color} {}
	~Circle() = default;

public:
	void Draw(SDL_Renderer* renderer) const override
	{
		assert(renderer);
		SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
		for (int w = 0; w < radius * 2; w++)
		{
			for (int h = 0; h < radius * 2; h++)
			{
				int dx = radius - w; 
				int dy = radius - h; 
				if ((dx * dx + dy * dy) <= (radius * radius))
					SDL_RenderDrawPoint(renderer, center.x + dx, center.y + dy);
			}
		}
	}

private:
	Point	  center;
	int		  radius;
	SDL_Color color;
};


class SDLBox
{
public:
	SDLBox()
	{
		auto errorInitSDL = SDL_Init(SDL_INIT_EVERYTHING);
		assert(!errorInitSDL);
		auto errorInitTTF = TTF_Init();
		assert(!errorInitTTF);
		window = std::unique_ptr<SDL_Window, std::function<void(SDL_Window*)>>(
			SDL_CreateWindow("SDL2GeometricPrimitives", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1000, 1000, SDL_WINDOW_SHOWN),
			[](SDL_Window* wnd) { SDL_DestroyWindow(wnd); });
		assert(window);
		renderer = std::unique_ptr<SDL_Renderer, std::function<void(SDL_Renderer*)>>(SDL_CreateRenderer(window.get(), -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC),
												  [](SDL_Renderer* renderer) { SDL_DestroyRenderer(renderer); });
		assert(renderer);

		auto font = TTF_OpenFont("./data/FreeSans.ttf", 70);
		assert(font);
		surfaceMessage = std::unique_ptr<SDL_Surface, std::function<void(SDL_Surface*)>>(TTF_RenderText_Solid(font, "press ESC to quit", { 255, 255, 255 }),
													   [](SDL_Surface* surfaceMessage) { SDL_FreeSurface(surfaceMessage); });
		assert(surfaceMessage);
		message = std::unique_ptr<SDL_Texture, std::function<void(SDL_Texture*)>>(SDL_CreateTextureFromSurface(renderer.get(), surfaceMessage.get()),
												[](SDL_Texture* message){ SDL_DestroyTexture(message); });
		assert(message);
	}
	~SDLBox()
	{
		TTF_Quit();
		SDL_Quit();
	}

	SDLBox(const SDLBox&)			= delete;
	SDLBox(SDLBox&& other) noexcept = default;

	SDLBox& operator=(const SDLBox& other)	  = delete;
	SDLBox& operator=(SDLBox&& other) noexcept = default;

public:
	void Run() const
	{
		SDL_SetRenderDrawColor(renderer.get(), 0, 0, 0, 0);
		SDL_RenderClear(renderer.get());
		for (auto& p : primitives)
			p->Draw(renderer.get());
		
		SDL_Rect Message_rect;
		Message_rect.x = 10;
		Message_rect.y = 10;
		Message_rect.w = 300;
		Message_rect.h = 70;
		SDL_RenderCopy(renderer.get(), message.get(), NULL, &Message_rect);		

		SDL_RenderPresent(renderer.get());
		WaitForESC();
		
	}
	void PushPrimitive(std::unique_ptr<IPrimitive> primitive)
	{
		primitives.push_back(std::move(primitive));
	}
	
private:
	void WaitForESC() const
	{
		while (true)
		{
			SDL_Event ev;
			while (SDL_PollEvent(&ev))
			{
				if ((SDL_QUIT == ev.type) || (SDL_KEYDOWN == ev.type && SDL_SCANCODE_ESCAPE == ev.key.keysym.scancode))
					return;
			}
		}
	}

private:
	std::unique_ptr<SDL_Texture, std::function<void(SDL_Texture*)>>		message;
	std::unique_ptr<SDL_Surface, std::function<void(SDL_Surface*)>>		surfaceMessage;
	std::vector<std::unique_ptr<IPrimitive>>							primitives;
	std::unique_ptr<SDL_Window, std::function<void(SDL_Window*)>>		window;
	std::unique_ptr<SDL_Renderer, std::function<void(SDL_Renderer*)>>	renderer;
};

