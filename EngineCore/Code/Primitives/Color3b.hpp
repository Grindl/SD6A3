#pragma once
#ifndef include_COLOR3B
#define include_COLOR3B

#include "Color4f.hpp"

class Color3b
{
public:
	unsigned char r, g, b;

	Color3b()
	{
		r = 255;
		g = 255;
		b = 255;
	}
	Color3b(Color4f init)
	{
		r = (unsigned char)(init.red*255);
		g = (unsigned char)(init.green*255);
		b = (unsigned char)(init.blue*255);
	}
	Color3b(unsigned char red, unsigned char green, unsigned char blue)
	{
		r = red;
		g = green;
		b = blue;
	}

	const bool operator==(const Color3b& rhs) const
	{
		bool isEqual = true;
		isEqual = isEqual && (r == rhs.r);
		isEqual = isEqual && (g == rhs.g);
		isEqual = isEqual && (b == rhs.b);
		return isEqual;
	}
};

#endif