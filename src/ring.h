#pragma once
#include <cstddef>

namespace container {
namespace ring {

template<typename Size = size_t>
struct index {
	index(Size s, Size p) : size{s}, position{p} {}
	index(Size s) : index{s, 0} {}
	index& operator++()
	{
		position = (position + 1) % size;
		return *this;
	}
	index  operator++(int)
	{
		index before = *this;
		++(*this);
		return before;
	}

	explicit operator Size()
	{
		return position;
	}
private:
	const Size size;
	Size position;
};

}}
