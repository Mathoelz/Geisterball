#include "color.h"
#include <assert.h>

Color::Color()
{
	this->R = 0;
	this->G = 0;
	this->B = 0;
}

Color::Color( float r, float g, float b)
{
	this->R = r;
	this->G = g;
	this->B = b;
}

Color Color::operator*(const Color& c) const
{
	Color result = Color();
	result.R = this->R * c.R;
	result.G = this->G * c.G;
	result.B = this->B * c.B;
    return result; 
}

Color Color::operator*(const float Factor) const
{
	Color result = *this;
	result.R *= Factor;
	result.G *= Factor;
	result.B *= Factor;
	return result; 
}

Color Color::operator+(const Color& c) const
{
	Color result = Color();
	result.R = this->R + c.R;
	result.G = this->G + c.G;
	result.B = this->B + c.B;
	return result;
}

Color& Color::operator+=(const Color& c)
{
	this->R += c.R;
	this->G += c.G;
	this->B += c.B;
	return *this; 
}