//
// Color.cpp
//
// Code by Caleb Biasco (biasc007) for Assignment-1A of CSCI5607

/*
 * The Color class holds RGB values and pertinent operations on them.
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

double Color::getR()
{
	return m_r;
}

void Color::setR(double r)
{
	m_r = r;
}

double Color::getG()
{
	return m_g;
}

void Color::setG(double g)
{
	m_g = g;
}

double Color::getB()
{
	return m_b;
}

void Color::setB(double b)
{
	m_b = b;
}