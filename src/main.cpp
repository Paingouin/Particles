#include "utils.h"
#include "containers.h"
#include "timestep.h"
#include "game.h"

//Using SDL and standard IO
#include <SDL.h>
#include <iostream>


struct Window
{
	//TODO : Use Opengl + glew (or glad)

	//custom deleter for unique ptr (because we don't use a complete object)
	struct SDL_Deleters_CD
	{
		void operator()(SDL_Window* d)   const { SDL_DestroyWindow(d); SDL_Quit();}
		void operator()(SDL_Renderer* d) const { SDL_DestroyRenderer(d); };
		void operator()(SDL_Texture* d)  const { SDL_DestroyTexture(d); };
	};

	Window()
		: m_iScreenWidth(800)
		, m_iScreenHeight(600)
		, m_iScaleFactor(2)
	{
		//Initialize SDL
		if (SDL_Init(SDL_INIT_VIDEO) < 0)
		{
			std::cout << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
		}
		else
		{
			//Create window
			m_gWindow.reset( SDL_CreateWindow("Particles", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, m_iScreenWidth, m_iScreenHeight, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE));
			if (m_gWindow.get() == nullptr)
			{
				std::cout << "Window could not be created! SDL_Error:" << SDL_GetError() << std::endl;
			}
			else
			{
				//Create renderer for window
				m_gRenderer.reset(SDL_CreateRenderer( m_gWindow.get(), -1, SDL_RENDERER_ACCELERATED ));
				if (m_gRenderer.get()== nullptr)
				{
					std::cout << "Renderer could not be created! SDL Error:" << SDL_GetError() << std::endl;
				}
				//Initialize renderer color
				SDL_SetRenderDrawColor(m_gRenderer.get(), 0x44, 0x44, 0x44, 0xFF);

				SDL_RenderSetScale(m_gRenderer.get(), m_iScaleFactor, m_iScaleFactor);
			}
		}
	}

	~Window() {}

	void updateSurface() const { SDL_UpdateWindowSurface(m_gWindow.get()); }
	void changeSize(const int x, const int y)
	{
		m_iScreenWidth = x;
		m_iScreenHeight = y;
	}

	void fillSurface() 
	{
		//Get window surface
		m_gSurface = SDL_GetWindowSurface(m_gWindow.get());
		//Fill the surface white
		SDL_FillRect(m_gSurface, NULL, SDL_MapRGB(m_gSurface->format, 0x44, 0x44, 0x44));
	}

	void clearScreen() 
	{ 
		SDL_SetRenderDrawColor(m_gRenderer.get(), 0x44, 0x44, 0x44, 0xFF);
		SDL_RenderClear(m_gRenderer.get()); 
	}
	
	void updateScreen() 
	{ 
		SDL_RenderPresent(m_gRenderer.get());
	}

	void drawPoint(const int posX,const int posY, const Uint32 color) 
	{ 
		SDL_SetRenderDrawColor(m_gRenderer.get(), (color&0xFF000000 )>> 24, (color & 0x00FF0000) >> 16, (color & 0x0000FF00) >> 8, (color & 0x000000FF));
		SDL_RenderDrawPoint(m_gRenderer.get(), posX, posY);
		SDL_SetRenderDrawColor(m_gRenderer.get(), 0x44, 0x44, 0x44, 0xFF);
	};

	void drawScreenFromParticles(const vec2d<Particle>& parts)
	{
		Uint32 pNum = 0;
		for (int j = 0; j < m_iScreenHeight / m_iScaleFactor; ++j)
		{
			for (int i = 0; i < m_iScreenWidth / m_iScaleFactor; ++i)
			{
				const Particle& p = parts.at(i, j);
				if (p.type != 0)
				{
					drawPoint(i, ((m_iScreenHeight - 1) / m_iScaleFactor) - j, p.color);
					++pNum;
				}		
			}
		}
		//std::cout << "Total particles : " << pNum <<std::endl;
	}


	//Order of these three member is important : 

	//The window we'll be rendering to
	std::unique_ptr<SDL_Window, SDL_Deleters_CD> m_gWindow = nullptr;

	//The window renderer
	std::unique_ptr<SDL_Renderer, SDL_Deleters_CD> m_gRenderer = nullptr;

	//Current displayed texture
	std::unique_ptr <SDL_Texture, SDL_Deleters_CD> m_gTexture = nullptr;


	//The surface contained by the window, managed by the window
	SDL_Surface* m_gSurface = nullptr;

	int m_iScreenWidth;
	int m_iScreenHeight;

	int m_iScaleFactor;
};




int main( int argc, char* args[] )
{
	try
	{
		//Initialize time
		TimeStep t{};

		//Initialize Window
		Window win{};

		//Initialize Game
		Game game{win.m_iScreenWidth/ win.m_iScaleFactor, win.m_iScreenHeight/ win.m_iScaleFactor };


		Controller cnt{};

		//Main loop flag
		bool quit = false;
		while (!quit)
		{
			t.startFrame();
			
			//Event handler
			SDL_Event e;
			while (SDL_PollEvent(&e) != 0)
			{

				if (e.type == SDL_WINDOWEVENT) {
					switch (e.window.event) {
					case SDL_WINDOWEVENT_RESIZED:
						win.changeSize(e.window.data1, e.window.data2);
						game.changeSize(win.m_iScreenWidth / win.m_iScaleFactor, win.m_iScreenHeight / win.m_iScaleFactor);
						break;
					default:
						break;
					}
				}
				//event is quit
				if (e.type == SDL_QUIT)
				{
					quit = true;
				}
				else if (e.type == SDL_MOUSEMOTION)
				{
					//Get mouse position
					SDL_GetMouseState(&cnt.m_iMouseX, &cnt.m_iMouseY);
					cnt.m_iMouseX /= win.m_iScaleFactor;
					cnt.m_iMouseY  = win.m_iScreenHeight - cnt.m_iMouseY;
					cnt.m_iMouseY /= win.m_iScaleFactor;
				}
				else if (e.type == SDL_MOUSEBUTTONDOWN || e.type == SDL_MOUSEBUTTONUP)
				{
					if (e.button.button == SDL_BUTTON_LEFT)
						cnt.m_bLeftClick = (e.button.state == SDL_PRESSED) ? true : false;
					else if (e.button.button == SDL_BUTTON_RIGHT)
						cnt.m_bRightClick = (e.button.state == SDL_PRESSED) ? true : false;
				}
				else if (e.type == SDL_MOUSEWHEEL)
				{
					if (e.wheel.y > 0) // scroll up
					{
						cnt.m_iRadius = (cnt.m_iRadius < 20) ? cnt.m_iRadius +  1 :  20;
					}
					else if (e.wheel.y < 0) // scroll down
					{
						cnt.m_iRadius = (cnt.m_iRadius > 0) ? cnt.m_iRadius -  1 : 0;
					}
				}
				else if (e.type == SDL_KEYDOWN)
				{
					//Select surfaces based on key press
					switch (e.key.keysym.sym)
					{
					default:
						break;
					}
				}
			}

			//Update while needed
			while(t.update())
				game.update(cnt,t.getDeltaTime());
			
			std::cout << "FPS : " << t.fps() << std::endl;
			std::cout << t.timeSinceStart() << std::endl; //get time of the update(s)


			//Rendering
			win.clearScreen();
			win.drawScreenFromParticles(game.m_vParticles);
			win.updateScreen();

			//Sleep
			t.pause();
		}
	}
	catch (std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}

    return 0;
}