//
// Light.cpp
//
// Code by Caleb Biasco (biasc007) for Assignment-1B of CSCI5607

/*
 * The Light class is a superclass of all lighting objects for
 * the scene. Any general lighting functions should be specified
 * here.
 */

#include "Light.h"

Light::Light()
{
	Color m_c;
}

Light::Light(Color c)
{
	m_c = c;
}

Light::~Light() {}