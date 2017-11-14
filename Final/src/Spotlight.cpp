//
// Spotlight.cpp
//
// Code by Caleb Biasco (biasc007) for Assignment-1 of CSCI5607

/*
 * The Spotlight class is a Light subclass that holds the parameters and behaviors
 * of a spotlight.
 */

#include <algorithm>
#include "Spotlight.h"

using std::max;
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

Spotlight::Spotlight(double sl[10]) : Light(Color(sl[7], sl[8], sl[9]))
{
	m_pos = Vec3(sl[0], sl[1], sl[2]);
	m_dir = Vec3(sl[3], sl[4], sl[5]);
	m_theta = sl[6];
}

Spotlight::~Spotlight() {}

Vec3 Spotlight::getL(Vec3 &intersectionPoint)
{
	Vec3 L = m_pos - intersectionPoint;
	L.normalize();
	return L;
}

double Spotlight::shadow(Vec3 &intersectionPoint, Scene &scene, RayPayload &payload)
{
	double shadow, lightDistance;
	RayPayload sPayload;
	Vec3 L = getL(intersectionPoint);
	
	// "Block" if the object lies outside of the cone of illumination.
	if (dot(m_dir*(-1), L) < cos((m_theta * PI / 180) / 2))
	{
		return 0;
	}
	
	lightDistance = (m_pos - intersectionPoint).length();
	
	shadow = 1;
	
	for (int i = 0; i < scene.objects.size() && shadow > 0; i++)
	{
		sPayload.distance = -1;
		scene.objects[i]->intersection(L, intersectionPoint, sPayload);
		
		// Block if the obstruction is not the object itself and lies strictly
		// between the point light and the object surface.
		if (i != payload.object && sPayload.distance > 0 && sPayload.distance < lightDistance)
		{
			shadow -= scene.materials[sPayload.material]->m_alpha;
		}
	}
	
	return max(0.0, shadow);
}
