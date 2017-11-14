//
// PointLight.cpp
//
// Code by Caleb Biasco (biasc007) for Assignment-1B of CSCI5607

/*
 * The PointLight class is a Light subclass that holds the behavior
 * of a point light.
 */

#include <algorithm>
#include "PointLight.h"

using std::min;

PointLight::PointLight(double x, double y, double z) : Light()
{
	m_pos = Vec3(x, y, z);
}

PointLight::PointLight(double x, double y, double z, Color c) : Light(c)
{
	m_pos = Vec3(x, y, z);
}

PointLight::PointLight(Vec3 v) : Light()
{
	m_pos = v;
}

PointLight::PointLight(Vec3 v, Color c) : Light(c)
{
	m_pos = v;
}

PointLight::~PointLight() {}

Vec3 PointLight::getL(Vec3 intersectionPoint)
{
	Vec3 L = m_pos - intersectionPoint;
	L.normalize();
	return L;
}

double PointLight::shadow(vector<Object*> objects, Vec3 intersectionPoint, int index)
{
	double shadow, lightDistance, shadowDistance;
	Vec3 L = getL(intersectionPoint);
	
	lightDistance = (m_pos - intersectionPoint).length();
	
	shadow = 1;
	
	for (int i = 0; i < objects.size() && shadow > 0; i++)
	{
		shadowDistance = objects[i]->intersection(L, intersectionPoint);
		
		// Block if the obstruction is not the object itself and lies strictly
		// between the point light and the object surface.
		if (i != index && shadowDistance >= 0 && shadowDistance < lightDistance)
		{
			shadow = 0;
		}
	}
	
	return shadow;
}