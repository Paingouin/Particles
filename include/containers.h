#pragma once
#define _ITERATOR_DEBUG_LEVEL 0
#include <vector>


template<class T>
struct Vec2d
{
	// misc types
	using value_type = typename std::vector<T>::value_type;
	using size_type = typename std::vector<T>::size_type;

	using reference = typename std::vector<T>::reference;
	using const_reference = typename std::vector<T>::const_reference;

	using iterator = typename std::vector<T>::iterator;
	using const_iterator = typename std::vector<T>::const_iterator;
	using reverse_iterator = typename std::vector<T>::reverse_iterator;
	using const_reverse_iterator = typename std::vector<T>::const_reverse_iterator;

	// iterators
	constexpr iterator               begin() noexcept { return vec.begin(); };
	constexpr const_iterator         begin() const noexcept { return vec.begin(); };
	constexpr iterator               end() noexcept { return vec.end(); };
	constexpr const_iterator         end() const noexcept { return vec.end(); };
	constexpr reverse_iterator       rbegin() noexcept { return vec.rbegin(); };
	constexpr const_reverse_iterator rbegin() const noexcept { return vec.rbegin(); };
	constexpr reverse_iterator       rend() noexcept { return vec.rend(); };
	constexpr const_reverse_iterator rend() const noexcept { return vec.rend(); };

	//constexpr const_iterator         cbegin() const noexcept { return vec.cbegin(); };
	//constexpr const_iterator         cend() const noexcept { return vec.cend(); };
	//constexpr const_reverse_iterator crbegin() const noexcept { return vec.crbegin(); };
	//constexpr const_reverse_iterator crend() const noexcept { return vec.crend(); };

	Vec2d(size_type  sizeX, size_type  sizeY)
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

	inline const bool isBound(const int x, const int y) const noexcept
	{
		if (x < 0 || y < 0 || x >= sizeX || y >= sizeY) return false;  
		else return true;
	}

	std::vector<T> vec;
	size_type  sizeX;
	size_type  sizeY;
};






