//
// PointLight.cpp
//
// Code by Caleb Biasco (biasc007) for Assignment-1 of CSCI5607

/*
 * The PointLight class is a Light subclass that holds the behavior
 * of a point light.
 */

#include <algorithm>
#include "PointLight.h"

using std::max;
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

PointLight::PointLight(double l[7]) : Light(Color(l[4], l[5], l[6]))
{
	m_pos = Vec3(l[0], l[1], l[2]);
}

PointLight::~PointLight() {}

Vec3 PointLight::getL(Vec3 &intersectionPoint)
{
	Vec3 L = m_pos - intersectionPoint;
	L.normalize();
	return L;
}

double PointLight::shadow(Vec3 &intersectionPoint, Scene &scene, RayPayload &payload)
{
	double shadow, lightDistance;
	RayPayload sPayload = RayPayload();
	Vec3 L = getL(intersectionPoint);
	
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
