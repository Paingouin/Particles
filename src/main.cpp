#include "utils.h"
#include "containers.h"
#include "timestep.h"
#include "game.h"

//Using SDL, glew and standard IO
#include <SDL.h>
#include <gl\glew.h>
#include <SDL_opengl.h>
#include <gl\glu.h>

#include <iostream>


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
		, m_iScaleFactor(1)
	{
		m_gDrawtexture.resize(m_iScreenWidth * m_iScreenHeight);

		//Initialize SDL
		if (SDL_Init(SDL_INIT_VIDEO) < 0)
		{
			std::cout << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
		}
		else
		{
			//Use OpenGL 3.3 core
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

			SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);        
			SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);      
			SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);      
			SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);   
			SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);    


			//Create window
			m_gWindow.reset( SDL_CreateWindow("Particles", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, m_iScreenWidth, m_iScreenHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE));
			if (m_gWindow.get() == nullptr)
			{
				std::cout << "Window could not be created! SDL_Error:" << SDL_GetError() << std::endl;
			}
			else
			{
				//Create context
				m_gContext = SDL_GL_CreateContext(m_gWindow.get());
				if (m_gContext == NULL)
				{
					printf("OpenGL context could not be created! SDL Error: %s\n", SDL_GetError());
				}
				else
				{
					//Initialize GLEW
					glewExperimental = GL_TRUE;
					GLenum glewError = glewInit();
					if (glewError != GLEW_OK)
					{
						printf("Error initializing GLEW! %s\n", glewGetErrorString(glewError));
					}
					//Use Vsync
					//if (SDL_GL_SetSwapInterval(1) < 0)
					//{
					//	printf("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());
					//}

					//Initialize OpenGL
					if (!initGL())
					{
						printf("Unable to initialize OpenGL!\n");
					}

				}
			}
		}
	}

	~Window() { glDeleteProgram(m_gProgramID); }

	bool initGL()
	{
		//Success flag
		bool success = true;

		//Generate program
		m_gProgramID = glCreateProgram();

		//Create vertex shader
		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

		//Get vertex source
		const GLchar* vertexShaderSource[] =
		{
			"#version 330 core\nlayout (location = 0) in vec3 aPos;\nvoid main()\n{\n   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n}\0"
		};

		//Set vertex source
		glShaderSource(vertexShader, 1, vertexShaderSource, NULL);

		//Compile vertex source
		glCompileShader(vertexShader);

		//Check vertex shader for errors
		GLint vShaderCompiled = GL_FALSE;
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &vShaderCompiled);
		if (vShaderCompiled != GL_TRUE)
		{
			printf("Unable to compile vertex shader %d!\n", vertexShader);
			char infoLog[512];
			glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
		}
		else
		{
			//Attach vertex shader to program
			glAttachShader(m_gProgramID, vertexShader);


			//Create fragment shader
			GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

			//Get fragment source
			const GLchar* fragmentShaderSource[] =
			{
				"#version 330 core\nout vec4 FragColor;\n void main() { FragColor  = vec4(1.0f, 0.5f, 0.2f, 1.0f); }"
			};

			//Set fragment source
			glShaderSource(fragmentShader, 1, fragmentShaderSource, NULL);

			//Compile fragment source
			glCompileShader(fragmentShader);

			//Check fragment shader for errors
			GLint fShaderCompiled = GL_FALSE;
			glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &fShaderCompiled);
			if (fShaderCompiled != GL_TRUE)
			{
				printf("Unable to compile fragment shader %d!\n", fragmentShader);
				char infoLog[512];
				glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
				std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
			}
			else
			{
				//Attach fragment shader to program
				glAttachShader(m_gProgramID, fragmentShader);

				//Link program
				glLinkProgram(m_gProgramID);

				//Check for errors
				GLint programSuccess = GL_TRUE;
				glGetProgramiv(m_gProgramID, GL_LINK_STATUS, &programSuccess);
				if (programSuccess != GL_TRUE)
				{
					printf("Error linking program %d!\n", m_gProgramID);
					char infoLog[512];
					glGetProgramInfoLog(m_gProgramID, 512, NULL, infoLog);
					std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
				}
				else
				{
					//glUseProgram(m_gProgramID);

					glDeleteShader(vertexShader);
					glDeleteShader(fragmentShader);
					
					//Initialize clear color
					glClearColor(138, 138, 138, 255.f);

					////VBO data
					//GLfloat vertexData[] =
					//{
					//	-0.5f, -0.5f,
					//	 0.5f, -0.5f,
					//	 0.5f,  0.5f,
					//	-0.5f,  0.5f
					//};
					//
					////IBO data
					//GLuint indexData[] = { 0, 1, 2, 3 };
					//
					////Create VBO
					//glGenBuffers(1, &m_gVBO);
					//glBindBuffer(GL_ARRAY_BUFFER, m_gVBO);
					//glBufferData(GL_ARRAY_BUFFER, 2 * 4 * sizeof(GLfloat), vertexData, GL_STATIC_DRAW);
					//
					////Create IBO
					//glGenBuffers(1, &m_gIBO);
					//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_gIBO);
					//glBufferData(GL_ELEMENT_ARRAY_BUFFER, 4 * sizeof(GLuint), indexData, GL_STATIC_DRAW);
					
				}
			}
		}

		return success;
	}

	void updateSurface() const { SDL_UpdateWindowSurface(m_gWindow.get()); }
	void changeSize(const int x, const int y)
	{
		m_iScreenWidth = x;
		m_iScreenHeight = y;
		m_gDrawtexture.resize(m_iScreenWidth* m_iScreenHeight);
	}

	//void fillSurface() 
	//{
	//	//Get window surface
	//	m_gSurface = SDL_GetWindowSurface(m_gWindow.get());
	//	//Fill the surface white
	//	SDL_FillRect(m_gSurface, NULL, SDL_MapRGB(m_gSurface->format, 0x44, 0x44, 0x44));
	//}

	void clearScreen() 
	{ 
		//Clear color buffer
		glClear(GL_COLOR_BUFFER_BIT);

	}
	
	void updateScreen() 
	{
		SDL_GL_SwapWindow(m_gWindow.get());
	}

	void drawPoint(const int posX,const int posY, const Uint32 color) 
	{ 

		
	};

	void drawScreenFromParticles(const vec2d<Particle>& parts)
	{

		
		auto f = [](const Particle& p) -> Uint32 {return p.color; };

		std::transform(parts.begin(), parts.end(), m_gDrawtexture.begin(), f);

		glDrawPixels(m_iScreenWidth, m_iScreenHeight, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, m_gDrawtexture.data());

		//Uint32 pNum = 0;
		//for (int j = 0; j < m_iScreenHeight / m_iScaleFactor; ++j)
		//{
		//	for (int i = 0; i < m_iScreenWidth / m_iScaleFactor; ++i)
		//	{
		//		const Particle& p = parts.at(i, j);
		//		if (p.type != 0)
		//		{
		//			drawPoint(i, ((m_iScreenHeight - 1) / m_iScaleFactor) - j, p.color);
		//			++pNum;
		//		}		
		//	}
		//}
		//std::cout << "Total particles : " << pNum <<std::endl;
	}


	//Order of these three member is important : 

	//The window we'll be rendering to
	std::unique_ptr<SDL_Window, SDL_Deleters_CD> m_gWindow = nullptr;

	//The window renderer
	//std::unique_ptr<SDL_Renderer, SDL_Deleters_CD> m_gRenderer = nullptr;

	//Current displayed texture
	//std::unique_ptr <SDL_Texture, SDL_Deleters_CD> m_gTexture = nullptr;


	//The surface contained by the window, managed by the window
	//SDL_Surface* m_gSurface = nullptr;

	//Graphics program
	GLuint m_gProgramID = 0;
	GLint m_gVertexPos2DLocation = -1;
	GLuint m_gVBO = 0;
	GLuint m_gIBO = 0;
	SDL_GLContext m_gContext;

	std::vector<Uint32> m_gDrawtexture;

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

			//Rendering
			win.clearScreen();
			win.drawScreenFromParticles(game.m_vParticles);
			win.updateScreen();

			std::cout << "FPS : " << t.fps() << std::endl;
			std::cout << t.timeSinceStart() << std::endl;

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