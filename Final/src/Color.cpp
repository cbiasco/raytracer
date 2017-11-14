//
// Color.cpp
//
// Code by Caleb Biasco (biasc007) for Assignment-1 of CSCI5607

/*
 * The Color class holds red, green, and blue values and checks
 * that they fit within certain bounds.
 */

#include "Color.h"

Color::Color()
{
	m_r = 0;
	m_g = 0;
	m_b = 0;
}

Color::Color(double r, double g, double b)
{
	m_r = r;
	m_g = g;
	m_b = b;
}

Color::~Color() {}

bool Color::outOfBounds()
{
	return m_r < 0 || m_r > 1 || m_g < 0 ||
		m_g > 1 || m_b < 0 || m_b > 1;
}

Color Color::clampColor()
{
	double r, g, b;
	r = Color::clamp(m_r, 0.0, 1.0);
	g = Color::clamp(m_g, 0.0, 1.0);
	b = Color::clamp(m_b, 0.0, 1.0);
	return Color(r, g, b);
}

// Component-wise operations
Color operator+ (const Color& a, const Color& b)
{
	return Color(a.m_r + b.m_r, a.m_g + b.m_g, a.m_b + b.m_b);
}

Color operator- (const Color& a, const Color& b)
{
	return Color(a.m_r - b.m_r, a.m_g - b.m_g, a.m_b - b.m_b);
}

Color operator* (const Color& a, double d)
{
	return Color(a.m_r * d, a.m_g * d, a.m_b * d);
}

Color operator* (double d, const Color& b)
{
	return Color(b.m_r * d, b.m_g * d, b.m_b * d);
}

Color operator* (const Color& a, const Color& b)
{
	return Color(a.m_r * b.m_r, a.m_g * b.m_g, a.m_b * b.m_b);
}

Color operator/ (const Color& a, double d)
{
	return Color(a.m_r / d, a.m_g / d, a.m_b / d);
}

Color operator/ (double d, const Color& b)
{
	return Color(b.m_r / d, b.m_g / d, b.m_b / d);
}

Color operator/ (const Color& a, const Color& b)
{
	return Color(a.m_r / b.m_r, a.m_g / b.m_g, a.m_b / b.m_b);
}
