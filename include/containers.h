#pragma once
//#define _ITERATOR_DEBUG_LEVEL 0
#include <vector>

//fast in realise target
template<class T>
struct vec2d
{
	// misc types
	using value_type = typename std::vector<T>::value_type;
	using size_type = typename std::vector<T>::size_type;

	using reference = typename std::vector<T>::reference;
	using const_reference = typename std::vector<T>::const_reference;

	vec2d(size_type  sizeX, size_type  sizeY) 
		: sizeX{ sizeX }
		, sizeY{ sizeY }
	{
		vec.resize(sizeX*sizeY);
	}

	inline reference at(const size_type  posX, const size_type  posY) noexcept
	{
		return vec[posX +  posY * sizeX];
	}

	inline const_reference at(const size_type  posX,const  size_type  posY) const noexcept
	{
		return vec[posX + posY * sizeX];
	}

	std::vector<T> vec;
	size_type  sizeX;
	size_type  sizeY;
};






