#include "utils.h"
#include "containers.h"
#include "timestep.h"
#include "game.h"
#include "window.h"


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

		bool quit = false;
		while (!quit)
		{
			//Update timers
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

			//Rendering

			//Clear screen + start imgui new frame
			win.clearScreen();

			//draw texture from particles
			win.drawScreenFromParticles(game.m_vParticles);
		
			//add blur + gamma correction etc...

			//Render Imgui + swap
			win.drawRectangle(0, 0, 1, 1, 111);

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