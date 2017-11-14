//
// Spotlight.cpp
//
// Code by Caleb Biasco (biasc007) for Assignment-1B of CSCI5607

/*
 * The Spotlight class is a Light subclass that holds the parameters and behaviors
 * of a spotlight.
 */

#include <algorithm>
#include "Spotlight.h"

using std::min;

const double PI = 4*atan(1.0);

Spotlight::Spotlight(double x, double y, double z, double dx, double dy, double dz,
	double theta) : Light()
{
	m_pos = Vec3(x, y, z);
	m_dir = Vec3(dx, dy, dz);
	m_theta = theta;
}

Spotlight::Spotlight(double x, double y, double z, double dx, double dy, double dz,
	double theta, Color c) : Light(c)
{
	m_pos = Vec3(x, y, z);
	m_dir = Vec3(dx, dy, dz);
	m_theta = theta;
}

Spotlight::Spotlight(Vec3 pos, Vec3 dir, double theta) : Light()
{
	m_pos = pos;
	m_dir = dir;
	m_theta = theta;
}

Spotlight::Spotlight(Vec3 pos, Vec3 dir, double theta, Color c) : Light(c)
{
	m_pos = pos;
	m_dir = dir;
	m_theta = theta;
}

Spotlight::~Spotlight() {}

Vec3 Spotlight::getL(Vec3 intersectionPoint)
{
	Vec3 L = m_pos - intersectionPoint;
	L.normalize();
	return L;
}

double Spotlight::shadow(vector<Object*> objects, Vec3 intersectionPoint, int index)
{
	double shadow, lightDistance, shadowDistance;
	Vec3 L = getL(intersectionPoint);
	
	// "Block" if the object lies outside of the cone of illumination.
	if (dot(m_dir*(-1), L) < cos((m_theta * PI / 180) / 2))
	{
		return 0;
	}
	
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