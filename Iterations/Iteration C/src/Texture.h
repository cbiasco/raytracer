#pragma once
//
// Texture.h
//
// Code by Caleb Biasco (biasc007) for Assignment-1 of CSCI5607

/*
 * The Texture class holds an int array that holds all pixel values
 * of an image to be used as a texture.
 */

#include "Color.h"

class Texture
{
public:
	Texture(int width, int height, int range, int *pixels);
	virtual ~Texture();

	// Set the texture with another int array
	void setPixels(int *pixels);

	// Get the texture at a given point
	Color getColor(double x, double y);

	int m_w; // width of the input image in pixels
	int m_h; // height of the input image in pixels
	int m_range; // color range of input image; default is 255

private:
	int *m_pixels; // pixel values of input image

};