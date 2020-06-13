#include "containers.h"

//Using SDL and standard IO
#include <SDL.h>
#include <iostream>

#include <chrono>
#include <thread>

#include <random>


#define RGB_TO_UINT(R, G, B, A) ((R&0xFF)<<24  | (G&0xFF)<<16 |(B&0xFF)<<8 | (A&0xFF) )


struct Particle
{
	double velocity = 1.0;
	Uint32 color;

	Uint8 type = 0;
};


struct Window
{
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
		, m_iScaleFactor(4)
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
				SDL_RenderSetScale(m_gRenderer.get(), m_iScaleFactor, m_iScaleFactor);

				//Initialize renderer color
				SDL_SetRenderDrawColor(m_gRenderer.get(), 0x44, 0x44, 0x44, 0xFF);
			}
		}
	}

	~Window() {}

	void UpdateSurface() const { SDL_UpdateWindowSurface(m_gWindow.get()); }

	void fillSurface() 
	{
		//Get window surface
		m_gSurface = SDL_GetWindowSurface(m_gWindow.get());
		//Fill the surface white
		SDL_FillRect(m_gSurface, NULL, SDL_MapRGB(m_gSurface->format, 0x44, 0x44, 0x44));
	}

	void clearScreen() { SDL_RenderClear(m_gRenderer.get()); }
	
	void updateScreen() { SDL_RenderPresent(m_gRenderer.get()); }

	void drawPoint(int posX, int posY, Uint32 color) 
	{ 
		SDL_SetRenderDrawColor(m_gRenderer.get(), (color&0xFF000000 )>> 24, (color & 0x00FF0000) >> 16, (color & 0x0000FF00) >> 8, (color & 0x000000FF));
		SDL_RenderDrawPoint(m_gRenderer.get(), posX, posY);
		SDL_SetRenderDrawColor(m_gRenderer.get(), 0x44, 0x44, 0x44, 0xFF);
	};

	void drawScreenFromParticles(const vec2d<Particle>& parts)
	{
		Uint32 p = 0;
		for (int j = 0; j < m_iScreenHeight / m_iScaleFactor; ++j)
		{
			for (int i = 0; i < m_iScreenWidth / m_iScaleFactor; ++i)
			{
				if (parts.at(i, j).type != 0)
				{
					drawPoint(i, ((m_iScreenHeight - 1) / m_iScaleFactor) - j, parts.at(i, j).color);
					++p;
				}		
			}
		}
		std::cout << "Total particles : " << p <<std::endl;
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


struct TimeStep
{
	//time unit is the milliseconds.

	TimeStep()
		: m_iDesiredFramePS{60}
		, m_iDesiredUpdatePS{60}
	{
		m_dStartTime = std::chrono::high_resolution_clock::now();
	}

	void startFrame()
	{
		std::chrono::high_resolution_clock::time_point newTime = std::chrono::high_resolution_clock::now();
		m_dLastFrameTime = newTime - m_dStartTime;
		m_dStartTime = newTime;

		//Prevent loop of death (if LastFrameTime> 2*m_dFrameTicks) (clamp the last frame time )
		if (m_dLastFrameTime > 2 * m_dFrameTime)
			m_dLastFrameTime = 2 * m_dFrameTime;

		m_dPhyTimeAccum += m_dLastFrameTime;
	}

	bool update()
	{
		if (m_dPhyTimeAccum >= m_dPhyxTicks)
		{
			m_dPhyTimeAccum -= m_dPhyxTicks;
			return true;
		}
		return false;
	}

	//For interpolation
	inline const double getAlpha() { return m_dPhyTimeAccum / m_dPhyxTicks; }
	//For calculus
	inline const double getDeltaTime() { return m_dPhyxTicks.count(); }
	//For Log
	inline const double timeSinceStart() { return std::chrono::duration<double, std::milli>( std::chrono::high_resolution_clock::now() - m_dStartTime).count(); }
	//FPS
	inline const double fps() { return 1000.0/ m_dLastFrameTime.count(); }

	//TODO : change granularity of timer resolution (eg timeBeginPeriod)
	void pause()
	{
		std::chrono::high_resolution_clock::time_point newTime = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double, std::milli> sleepTime = m_dFrameTime - (newTime - m_dStartTime);

		if (sleepTime >= std::chrono::duration<double, std::milli>(1))
			std::this_thread::sleep_for(sleepTime);	
	}


	std::chrono::high_resolution_clock::time_point m_dStartTime;

	std::chrono::duration<double, std::milli> m_dLastFrameTime = std::chrono::duration<double>(0);
	std::chrono::duration<double, std::milli> m_dPhyTimeAccum = std::chrono::duration<double>(0);

	int m_iDesiredFramePS;
	int m_iDesiredUpdatePS;

	const std::chrono::duration<double, std::milli> m_dPhyxTicks = std::chrono::duration<double>(1.0 / m_iDesiredUpdatePS);
	const std::chrono::duration<double, std::milli> m_dFrameTime = std::chrono::duration<double>(1.0 / m_iDesiredFramePS);
};

struct Controller
{
	bool m_bLeftClick = false;
	bool m_bRightClick = false;

	int m_iMouseX = 0;
	int m_iMouseY = 0;

	int m_iRadius = 11;
};

struct Game
{
	Game(const int screenWidht, const int screenHeight) : m_vParticles(screenWidht, screenHeight)
	{

	}

	void changeSize(const int screenWidht, const int screenHeight)
	{
		m_vParticles = vec2d<Particle>(screenWidht, screenHeight);
	}

	void update(const Controller& cnt, const double dt)
	{
		if(cnt.m_bLeftClick)
			spawnCircle(cnt);

		for (int j = 1; j < m_vParticles.sizeY; ++j)
		{
			for (int i = 1; i < m_vParticles.sizeX -1; ++i)
			{
				Uint8 type = m_vParticles.at(i, j).type;
				switch (type)
				{
				case 1:
					updateSand(i, j, dt);
					break;
				default:
					break;
				}
			}
		}
	}

	//add rand for right or left first based on parity of number of random seed step
	void updateSand(const int i, const int j , const double dt)
	{
		if (m_vParticles.at(i, j - 1).type == 0)
		{
			m_vParticles.at(i, j - 1) = m_vParticles.at(i, j);
			m_vParticles.at(i, j) = {};
		}
		else
		{
			int8_t m = rand() % 2 ? -1 : 1;
			if (m_vParticles.at(i - 1 * m, j - 1).type == 0)
			{
				m_vParticles.at(i - 1 * m, j - 1) = m_vParticles.at(i, j);
				m_vParticles.at(i, j) = {};	
			}
		}
	}

	void putPoint(const int x, const int y)
	{
		Particle p = { 1, (Uint32)RGB_TO_UINT(255, 199, 87, 255), 1 };
		if (x < 0 || y < 0 || x >= m_vParticles.sizeX || y >= m_vParticles.sizeY)
			return;
		m_vParticles.at(x,y) = p;
	}

	void putSymetricPoint(const int xc, const int yc, const int x, const int y)
	{
		putPoint(xc + x, yc + y);
		putPoint(xc - x, yc + y);
		putPoint(xc + x, yc - y);
		putPoint(xc - x, yc - y);
		putPoint(xc + y, yc + x);
		putPoint(xc - y, yc + x);
		putPoint(xc + y, yc - x);
		putPoint(xc - y, yc - x);
	}


	void spawnCircle(const Controller& cnt)
	{
		int x = 0;
		int y = cnt.m_iRadius;

		int d  = 3 - 2 * cnt.m_iRadius;

		putSymetricPoint(cnt.m_iMouseX , cnt.m_iMouseY, x, y);

		while (y >= x)
		{
			++x;
			if (d > 0)
			{
				y--;
				d = d + 4 * (x - y) + 10;
			}
			else
				d = d + 4 * x + 6;
			putSymetricPoint(cnt.m_iMouseX, cnt.m_iMouseY, x, y);
		}


	}

	
	vec2d<Particle> m_vParticles;

	double m_dGravity = 9.81;
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
						win.m_iScreenWidth = e.window.data1;
						win.m_iScreenHeight = e.window.data2;
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
				//User presses a key
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

			while(t.update())
				game.update(cnt,t.getDeltaTime());
			
			std::cout << "FPS : " << t.fps() << std::endl;
			std::cout << t.timeSinceStart() << std::endl;


			win.clearScreen();

			win.drawScreenFromParticles(game.m_vParticles);

			win.updateScreen();

			t.pause();
		}
	}
	catch (std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}

    return 0;
}