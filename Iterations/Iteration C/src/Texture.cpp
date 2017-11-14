//
// Texture.cpp
//
// Code by Caleb Biasco (biasc007) for Assignment-1 of CSCI5607

/*
 * The Texture class holds an int array that holds all pixel values
 * of an image to be used as a texture.
 */

#include <cmath>
#include "Texture.h"

Texture::Texture(int width, int height, int range, int *pixels)
{
	m_w = width;
	m_h = height;
	m_range = range;
	m_pixels = pixels;
}

Texture::~Texture()
{
	if (m_pixels)
		delete m_pixels;
}

void Texture::setPixels(int *pixels)
{
	if (m_pixels)
		delete m_pixels;
	m_pixels = pixels;
}

Color Texture::getColor(double x, double y)
{
	int xPix = floor(x*m_w);
	if (xPix < 0) xPix = 0;
	if (xPix >= m_w) xPix = m_w - 1;
	int yPix = floor(y*m_h);
	if (yPix < 0) yPix = 0;
	if (yPix >= m_h) yPix = m_h - 1;
	double r = ((double) m_pixels[3*(xPix + yPix*m_w)]) / m_range;
	double g = ((double) m_pixels[3*(xPix + yPix*m_w) + 1]) / m_range;
	double b = ((double) m_pixels[3*(xPix + yPix*m_w) + 2]) / m_range;

	return Color(r, g, b);
}