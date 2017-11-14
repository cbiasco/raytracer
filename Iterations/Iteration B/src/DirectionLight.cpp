//
// DirectionLight.cpp
//
// Code by Caleb Biasco (biasc007) for Assignment-1B of CSCI5607

/*
 * The DirectionLight class is a Light subclass that holds the behavior
 * of a direction light.
 */

#include "DirectionLight.h"

DirectionLight::DirectionLight(double x, double y, double z) : Light()
{
	m_dir = Vec3(x, y, z);
	m_dir.normalize();
}

DirectionLight::DirectionLight(double x, double y, double z, Color c) : Light(c)
{
	m_dir = Vec3(x, y, z);
	m_dir.normalize();
}

DirectionLight::DirectionLight(Vec3 v) : Light()
{
	m_dir = v;
	m_dir.normalize();
}

DirectionLight::DirectionLight(Vec3 v, Color c) : Light(c)
{
	m_dir = v;
	m_dir.normalize();
}

DirectionLight::~DirectionLight() {}

Vec3 DirectionLight::getL(Vec3 intersectionPoint)
{
	return m_dir*(-1);
}

double DirectionLight::shadow(vector<Object*> objects, Vec3 intersectionPoint, int index)
{
	double shadow, shadowDistance;
	Vec3 L = getL(intersectionPoint);
	
	shadow = 1;
	
	for (int i = 0; i < objects.size() && shadow > 0; i++)
	{
		shadowDistance = objects[i]->intersection(L, intersectionPoint);
		
		// Block if the obstruction is not the object itself and lies in the direction
		// of the light away from the object surface.
		if (i != index && shadowDistance >= 0)
		{
			shadow = 0;
		}
	}
	
	return shadow;
}