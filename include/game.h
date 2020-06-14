#pragma once

//Using SDL and standard IO
#include <SDL.h>
#include <random>

struct Particle
{
	double velocity = 1.0;
	bool   asMoved = false;
	Uint32 color;

	Uint8 type = 0;
};


struct Controller
{
	bool m_bLeftClick = false;
	bool m_bRightClick = false;

	int m_iMouseX = 0;
	int m_iMouseY = 0;

	int m_iRadius = 20;
};

struct Game
{
	Game(const int screenWidht, const int screenHeight) : m_vParticles(screenWidht, screenHeight)
	{
		srand(time(NULL));
	}

	void changeSize(const int screenWidht, const int screenHeight)
	{
		m_vParticles = vec2d<Particle>(screenWidht, screenHeight);
	}

	void update(const Controller& cnt, const double dt)
	{
	
		if(cnt.m_bLeftClick)
			spawnCircle(cnt, 1);
		else if (cnt.m_bRightClick)
			spawnCircle(cnt, 2);

		for (int j = 1; j < m_vParticles.sizeY -1; ++j)
		{
			for (int i = 1; i < m_vParticles.sizeX -1; ++i)
			{
				Particle& p = m_vParticles.at(i, j);

				if (p.asMoved == true)
					continue;

				switch (p.type)
				{
				case 1:
					updateSand(i, j, dt);
					break;
				case 2: 
					updateWater(i, j, dt);
					break;
				default:
					break;
				}
			}
		}
	}

	void updateSand(const int i, const int j , const double dt)
	{

		if (m_vParticles.at(i, j - 1).type == 0)
		{
			m_vParticles.at(i, j - 1) = m_vParticles.at(i, j);
			m_vParticles.at(i, j) = {};
		}
		else
		{
			if (m_vParticles.at(i - 1 , j - 1).type == 0)
			{
				m_vParticles.at(i - 1 , j - 1) = m_vParticles.at(i, j);
				m_vParticles.at(i, j) = {};	
			}
			else if (m_vParticles.at(i + 1 , j - 1).type == 0)
			{
				m_vParticles.at(i + 1 , j - 1) = m_vParticles.at(i, j);
				m_vParticles.at(i, j) = {};
			}
		}
	}

	void updateWater(const int i, const int j, const double dt)
	{
		if (m_vParticles.at(i, j - 1).type == 0)
		{
			m_vParticles.at(i, j - 1) = m_vParticles.at(i, j);
			m_vParticles.at(i, j) = {};
		}
		else
		{
			if (m_vParticles.at(i - 1, j - 1).type == 0)
			{
				m_vParticles.at(i - 1, j - 1) = m_vParticles.at(i, j);
				m_vParticles.at(i, j) = {};
			}
			else if (m_vParticles.at(i + 1, j - 1).type == 0)
			{
				m_vParticles.at(i + 1 , j - 1) = m_vParticles.at(i, j);
				m_vParticles.at(i, j) = {};
			}
			else if (m_vParticles.at(i - 1 , j).type == 0)
			{
				m_vParticles.at(i - 1 , j) = m_vParticles.at(i, j);
				m_vParticles.at(i, j) = {};
			}
			else if (m_vParticles.at(i + 1, j ).type == 0)
			{
				m_vParticles.at(i + 1, j ) = m_vParticles.at(i, j);
				m_vParticles.at(i, j) = {};
			}
		}
	}

	void putPoint(const int x, const int y , const Uint8 type )
	{
		Particle p {};

		if (type == 1)
			p = { 1, false, (Uint32)RGB_TO_UINT(255, 199, 87, 255), 1 };
		else if (type == 2)
			p = { 1, false, (Uint32)RGB_TO_UINT(0, 94, 255, 255), 2 };

		if (m_vParticles.isBound(x,y))
			m_vParticles.at(x,y) = p;
	}

	void putSymetricPoint(const int xc, const int yc, const int x, const int y , const Uint8 type)
	{
		putPoint(xc + x, yc + y, type);
		putPoint(xc - x, yc + y, type);
		putPoint(xc + x, yc - y, type);
		putPoint(xc - x, yc - y, type);
		putPoint(xc + y, yc + x, type);
		putPoint(xc - y, yc + x, type);
		putPoint(xc + y, yc - x, type);
		putPoint(xc - y, yc - x, type);
	}


	void spawnCircle(const Controller& cnt , const Uint8 type)
	{
		int x = 0;
		int y = cnt.m_iRadius;

		int d  = 3 - 2 * cnt.m_iRadius;

		putSymetricPoint(cnt.m_iMouseX , cnt.m_iMouseY, x, y, type);

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
			putSymetricPoint(cnt.m_iMouseX, cnt.m_iMouseY, x, y, type);
		}


	}

	
	vec2d<Particle> m_vParticles;

	double m_dGravity = 9.81;
};
