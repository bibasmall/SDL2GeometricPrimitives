#pragma once
#include <vector>
#include <string>
#include <memory>
#include <array>
#include <tuple>
#include <SDL.h>
#include <SDL_ttf.h>


class IPrimitive
{
public:
	IPrimitive() {}
	virtual ~IPrimitive() {};

public:
	virtual void Draw(SDL_Renderer*) const = 0;
};

class Triangle : public IPrimitive
{
public:
	Triangle() = delete;
	
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
		: IPrimitive(), verts{ SDL_Vertex{ x, SDL_Color{ xc }, SDL_FPoint{ 0 } },
								 SDL_Vertex{ y, SDL_Color{ yc }, SDL_FPoint{ 0 } },
								 SDL_Vertex{ z, SDL_Color{ zc }, SDL_FPoint{ 0 } } } 
	{

	}

	~Triangle() {}


public:
	void Draw(SDL_Renderer* renderer) const override
	{
		if (!renderer)
			return;
		SDL_RenderGeometry(renderer, nullptr, verts.data(), verts.size(), nullptr, 0);
	}

private:
	std::array<SDL_Vertex, 3> verts;
};

class Rectangle : public IPrimitive
{
public:
	Rectangle() = delete;
	Rectangle(int w, int h, int x, int y, SDL_Color color = { 255, 255, 255, 255 }) : rect{}, color{color}
	{
		rect.x = x;
		rect.y = y;
		rect.w = w;
		rect.h = h;
	}
	~Rectangle() {}

public:
	void Draw(SDL_Renderer* renderer) const override
	{
		if (!renderer)
			return;
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
	~Circle() {}

public:
	void Draw(SDL_Renderer* renderer) const override
	{
		if (!renderer)
			return;

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
		SDL_Init(SDL_INIT_EVERYTHING);
		window = SDL_CreateWindow("SDL2GeometricPrimitives", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1000, 1000, SDL_WINDOW_SHOWN);
		renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

		TTF_Init();
		TTF_Font* sans = TTF_OpenFont("./data/FreeSans.ttf", 70);
		surfaceMessage = TTF_RenderText_Solid(sans, "press ESC to quit", { 255, 255, 255 });
		message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
	}
	~SDLBox()
	{
		SDL_FreeSurface(surfaceMessage);
		SDL_DestroyTexture(message);
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
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
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
		SDL_RenderClear(renderer);
		for (auto& p : primitives)
			p->Draw(renderer);

		{
			SDL_Rect Message_rect;
			Message_rect.x = 10;
			Message_rect.y = 10;
			Message_rect.w = 300;
			Message_rect.h = 70;
			SDL_RenderCopy(renderer, message, NULL, &Message_rect);
		}

		bool running = true;
		while (running)
		{
			SDL_Event ev;
			while (SDL_PollEvent(&ev))
			{
				if ((SDL_QUIT == ev.type) ||
					(SDL_KEYDOWN == ev.type && SDL_SCANCODE_ESCAPE == ev.key.keysym.scancode))
				{
					running = false;
					break;
				}
			}

			SDL_RenderPresent(renderer);
		}
	}
	void PushPrimitive(std::unique_ptr<IPrimitive> primitive)
	{
		primitives.push_back(std::move(primitive));
	}

private:


private:
	SDL_Window*								 window = nullptr;
	SDL_Renderer*							 renderer = nullptr;
	SDL_Surface*							 surfaceMessage = nullptr;
	SDL_Texture*							 message = nullptr;
	std::vector<std::unique_ptr<IPrimitive>> primitives = {};
};

