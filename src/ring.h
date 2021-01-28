#pragma once
#include <cstddef>

namespace container {
namespace ring {

template<typename Size = size_t>
struct index {
	constexpr index(Size s, Size p) noexcept : size{s}, position{p} {}
	constexpr index(Size s) noexcept : index{s, 0} {}
	constexpr index operator+(Size n) noexcept
	{
		return {size, (position + n) % size};
	}
	constexpr index& operator+=(Size n) noexcept
	{
		position = (position + n) % size;
		return *this;
	}
	constexpr index& operator++() noexcept
	{
		return (*this) += 1;
	}
	constexpr index operator++(int) noexcept
	{
		index before = *this;
		++(*this);
		return before;
	}
	constexpr explicit operator Size() const noexcept
	{
		return position;
	}
private:
	const Size size;
	Size position;
};

template<typename T, typename Size = size_t>
struct vector {
	constexpr vector(T* b, Size s) noexcept : front{s}, base{b} {}
	constexpr T& operator[](Size p) noexcept
	{
		return base[(Size)(front + p)];
	}
	constexpr vector& operator>>(Size n) noexcept
	{
		front += n;
		return *this;
	}
private:
	index<Size> front;
	T* const base;
};

}}
