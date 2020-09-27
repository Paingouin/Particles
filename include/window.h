#pragma once

//Using SDL, glew and standard IO

#include <GL/glew.h>

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#endif

#include <SDL.h>
#include <SDL_opengl.h>

#include <iostream>
#include <fstream>
#include <algorithm>



struct Window
{
	//custom deleter for unique ptr (because we don't use a complete object)
	struct SDL_Deleters_CD
	{
		void operator()(SDL_Window* d)   const
		{ 
			//Destoy SDL_GL context ?
			SDL_DestroyWindow(d); SDL_Quit(); 
		}
		void operator()(SDL_Renderer* d) const { SDL_DestroyRenderer(d); };
		void operator()(SDL_Texture* d)  const { SDL_DestroyTexture(d); };
	};

	Window()
		: m_iScreenWidth(800)
		, m_iScreenHeight(600)
		, m_iScaleFactor(1)
	{
		//Initialize SDL
		if (SDL_Init(SDL_INIT_VIDEO) < 0)
		{
			std::cout << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
		}
		else
		{
			//Use OpenGL 3.3 core
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);


			//Create window
			m_gWindow.reset(SDL_CreateWindow("Particles", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, m_iScreenWidth, m_iScreenHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI));
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
					std::cout << "OpenGL context could not be created! SDL Error: "<< SDL_GetError() << std::endl;
				}
				else
				{
					SDL_GL_MakeCurrent(m_gWindow.get(), m_gContext);

					//Use Vsync
					if (SDL_GL_SetSwapInterval(1) < 0)
					{
						std::cout << "Warning: Unable to set VSync! SDL Error:"<< SDL_GetError() << std::endl;
					}

					//Initialize GLEW
					glewExperimental = GL_TRUE;
					GLenum glewError = glewInit();
					if (glewError != GLEW_OK)
					{
						std::cout << "Error initializing GLEW! " << glewGetErrorString(glewError) << std::endl;
					}

					//Initialize OpenGL
					if (!initGL())
					{
						std::cout << "Unable to initialize OpenGL" << std::endl;
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

		std::ifstream  vsFile{ "./resources/shaders/vertex.vs"};

		if (!vsFile)
			throw std::runtime_error(" error opening ./resources/shaders/vertex.vs" );

		std::string vertexShaderSource{ std::istreambuf_iterator<GLchar>(vsFile), std::istreambuf_iterator<GLchar>() };
		
		vsFile.close();
		const char* c_str = vertexShaderSource.c_str();
		//Set vertex source
		glShaderSource(vertexShader, 1,	&c_str, NULL);

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

			std::ifstream  fsFile{ "./resources/shaders/frag.fs" };

			if (!fsFile)
				throw std::runtime_error("  error opening ./resources/shaders/frag.fs");

			std::string fragmentShaderSource{ std::istreambuf_iterator<GLchar>(fsFile), std::istreambuf_iterator<GLchar>() };

			fsFile.close();
			c_str = fragmentShaderSource.c_str();

			//Set fragment source
			glShaderSource(fragmentShader, 1, &c_str, NULL);

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
					glGenTextures(1, &m_gTexture);
					glBindTexture(GL_TEXTURE_2D, m_gTexture);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);  //Always set the base and max mipmap levels of a texture.
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

					m_gDrawtexture.resize(m_iScreenWidth * m_iScreenHeight);

					glDeleteShader(vertexShader);
					glDeleteShader(fragmentShader);

					//VBO data
					GLfloat vertexData[] =
					{
						// positions          // colors                // texture coords
						 0.8f, -0.8f, 0.0f,   0.0f, 1.0f, 0.0f, 1.0f,   1.0f, 0.0f,   // bottom right
						-0.8f, -0.8f, 0.0f,   0.0f, 0.0f, 1.0f, 1.0f,   0.0f, 0.0f,   // bottom left
						-0.8f,  0.8f, 0.0f,   1.0f, 1.0f, 0.0f, 1.0f,   0.0f, 1.0f,    // top left 
						 0.8f,  0.8f, 0.0f,   1.0f, 0.0f, 0.0f, 1.0f,   1.0f, 1.0f,   // top right
					};

					//IBO data
					GLuint indexData[] =
					{
						0, 1, 3,  // first Triangle
						1, 2, 3
					};  // second Triangle};

					//Create VA0; (struct of info)
					glGenVertexArrays(1, &m_gVAO);
					glBindVertexArray(m_gVAO);

					//Create VBO (struct of data of vertices)
					glGenBuffers(1, &m_gVBO);
					glBindBuffer(GL_ARRAY_BUFFER, m_gVBO);
					glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);

					//Create IBO (index for order etc)
					glGenBuffers(1, &m_gIBO);
					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_gIBO);
					glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexData), indexData, GL_STATIC_DRAW);

					// position attribute
					glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
					glEnableVertexAttribArray(0);
					// color attribute
					glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
					glEnableVertexAttribArray(1);
					// texture coord attribute
					glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(7 * sizeof(float)));
					glEnableVertexAttribArray(2);

					//glBindBuffer(GL_ARRAY_BUFFER, 0);
					//glBindVertexArray(0);

				}
			}
		}

		return success;
	}

	void updateSurface() const { SDL_UpdateWindowSurface(m_gWindow.get()); }
	void changeSize(const int sizeX, const int sizeY)
	{
		m_iScreenWidth = sizeX;
		m_iScreenHeight = sizeY;
		m_gDrawtexture.resize(m_iScreenWidth* m_iScreenHeight);

		glViewport(0, 0, sizeX, sizeY);
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
		//Initialize clear color
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
	}

	void updateScreen()
	{
		SDL_GL_SwapWindow(m_gWindow.get());
	}

	void drawScreenFromParticles(const Vec2d<Particle>& parts)
	{
		auto f = [](const Particle& p) -> Uint32 {return p.color; };
		std::transform(parts.begin(), parts.end(), m_gDrawtexture.begin(), f); //sequenced_policy 

		glActiveTexture(m_gTexture);
		glBindTexture(GL_TEXTURE_2D, m_gTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, parts.sizeX, parts.sizeY, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, m_gDrawtexture.data());

		glUseProgram(m_gProgramID);
		glBindVertexArray(m_gVAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, m_gTexture);
		glActiveTexture(GL_TEXTURE0);
	}

	void drawRectangle(int x, int y, int x1, int y1, Uint32 color)
	{

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
	GLuint m_gVAO = 0;
	GLuint m_gVBO = 0;
	GLuint m_gIBO = 0;
	GLuint m_gTexture = 0;

	SDL_GLContext m_gContext;

	std::vector<Uint32> m_gDrawtexture;

	int m_iScreenWidth;
	int m_iScreenHeight;

	int m_iScaleFactor;
};
