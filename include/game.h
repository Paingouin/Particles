#pragma once

//Using SDL and standard IO
#include <SDL.h>
#include <random>
#include <algorithm>

//#include <iostream>


struct Particle
{
	double velocity = 1.0;
	Uint32 color = (Uint32)RGB_TO_UINT(138, 138, 138, 255);
	Uint8 flags = 0;  //1 : asMoved  //2: isBlocked
	Uint8 type = 0;   //0 : vide  1: sable
};

struct PartChunk
{
	bool to_update = false;
	int	 partActivated = 0;
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
	Game(const int screenWidht, const int screenHeight) : m_vParticles(screenWidht, screenHeight), m_vPartChunks((screenWidht / 64), (screenHeight / 64))
	{
		srand(time(NULL));
	}

	void changeSize(const int screenWidht, const int screenHeight)
	{
		m_vParticles = Vec2d<Particle>(screenWidht, screenHeight);
		m_vPartChunks = Vec2d<PartChunk>((screenWidht / 64), (screenHeight / 64));
	}

	void update(const Controller& cnt, const double dt)
	{
		//auto moveFalse = [](Particle& p) { p.flags &= 0x0 ; };
		//std::for_each(m_vParticles.begin(), m_vParticles.end(), moveFalse);

		if(cnt.m_bLeftClick)
			spawnCircle(cnt, 1);
		else if (cnt.m_bRightClick)
			spawnCircle(cnt, 2);

		m_iRNG =(rand() % 2) ? -1 : 1;

		for (size_t j =1; j < m_vParticles.sizeY - 1; ++j)
		{
			for (size_t i = 1; i < m_vParticles.sizeX -1; ++i)
			{
				Particle& p = m_vParticles.at(i, j);

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

		m_iNbPart = std::count_if(m_vParticles.begin(), m_vParticles.end(), [](Particle& p) {return p.type != 0; });
	}

	void updateSand(const int i, const int j , const double dt)
	{
		Particle& pM = m_vParticles.at(i, j);

		pM.velocity = (m_dGravity * dt) + pM.velocity ;

		int tPosY = j - pM.velocity ;

		bool moved = false;

		if(m_vParticles.isBound(i, tPosY) && m_vParticles.at(i,tPosY).type == 0)
		{
			movePoint(i, tPosY, i, j, pM);
			moved = true;
		}
		else
		{
			pM.velocity = 1.0;
			if (m_vParticles.at(i, j - 1).type == 0 && (rand() % 2))
			{
					movePoint(i, j-1, i, j, pM);
					moved = true;
			}
			else
			{
				if (rand() % 2)
				{
					if (m_vParticles.at(i - 1 * m_iRNG, j - 1).type == 0 && m_vParticles.at(i - 1 * m_iRNG, j).type == 0)
					{
						movePoint(i - 1 * m_iRNG, j - 1, i, j, pM);
						moved = true;
					}
					else if (m_vParticles.at(i + 1 * m_iRNG, j - 1).type == 0 && m_vParticles.at(i + 1 * m_iRNG, j ).type == 0)
					{
						movePoint(i + 1 * m_iRNG, j - 1, i, j, pM);
						moved = true;
					}
				}
			}	
		}

		if (moved == false)
		{
			removePartChunk(i, j);
		}
	}

	void updateWater(const int i, const int j, const double dt)
	{

		Particle& pM = m_vParticles.at(i, j);

		pM.velocity = (m_dGravity * dt) + pM.velocity;

		int tPosY = j - pM.velocity;

		if (m_vParticles.isBound(i, tPosY) && m_vParticles.at(i, tPosY).type == 0)
		{
			m_vParticles.at(i, tPosY) = pM;
			m_vParticles.at(i, j) = {};
			addPartChunk(i, j);
		}
		else
		{
			pM.velocity = 0;

			if (rand() % 2)
			{
				if (m_vParticles.at(i, j - 1).type == 0)
				{
					m_vParticles.at(i, j - 1) = m_vParticles.at(i, j);
					m_vParticles.at(i, j) = {};
					addPartChunk(i, j);
				}
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
					m_vParticles.at(i + 1, j - 1) = m_vParticles.at(i, j);
					m_vParticles.at(i, j) = {};
				}
				else if (m_vParticles.at(i - 1, j).type == 0)
				{
					m_vParticles.at(i - 1, j) = m_vParticles.at(i, j);
					m_vParticles.at(i, j) = {};
				}
				else if (m_vParticles.at(i + 1, j).type == 0)
				{
					m_vParticles.at(i + 1, j) = m_vParticles.at(i, j);
					m_vParticles.at(i, j) = {};
				}
			}
		}
	}

	void putPoint(const int x, const int y , const Uint8 type )
	{
		Particle p {};

		if (type == 1)
			p = { 1, (Uint32)RGB_TO_UINT(255, 199, 87, 255),0, 1 };
		else if (type == 2)
			p = { 1, (Uint32)RGB_TO_UINT(0, 94, 255, 255),0, 2 };

		if (m_vParticles.isBound(x, y))
		{
			m_vParticles.at(x, y) = p;
			addPartChunk( x,  y);
		}

	}

	void movePoint(const int x, const int y, const int x0 ,const int y0 , Particle& p)
	{
		m_vParticles.at(x, y) = p;
		m_vParticles.at(x0, y0) = {};
		addPartChunk(x, y);
		removePartChunk(x0, y0);
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


	void addPartChunk(int x, int y)
	{
		PartChunk& p = m_vPartChunks.at(x / 64, y / 64);
		
		if (p.to_update == false)
			p.to_update = true;

		++p.partActivated;
	}

	void removePartChunk(int x, int y)
	{
		PartChunk& p = m_vPartChunks.at(x / 64, y / 64);
		
		--p.partActivated;
		if (p.partActivated == 0)
			p.to_update = false;
	}
	
	Vec2d<Particle> m_vParticles;
	Vec2d<PartChunk> m_vPartChunks;

	double m_dGravity = 9.81;

	Uint32 m_iNbPart = 0;

	int m_iRNG = 0;
};
