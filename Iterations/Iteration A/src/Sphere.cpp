//
// Sphere.cpp
//
// Code by Caleb Biasco (biasc007) for Assignment-1A of CSCI5607

/*
 * The Sphere class inherits the Object superclass and also holds a radius value.
 */

#include <cmath>
#include <algorithm>
#include "Sphere.h"

using std::min;

Sphere::Sphere(double x, double y, double z, double r) : Object(x, y, z)
{
	m_r = r;
}

Sphere::Sphere(double x, double y, double z, double r, Color c) : Object(x, y, z, c)
{
	m_r = r;
}

Sphere::Sphere(Vec3 v, double r) : Object(v)
{
	m_r = r;
}

Sphere::Sphere(Vec3 v, double r, Color c) : Object(v, c)
{
	m_r = r;
}

Sphere::~Sphere() {}

double Sphere::intersection(Vec3 ray, Vec3 origin)
{
	Vec3 distance;
	double discriminant, quadraticAdd, quadraticSub;
	distance = origin - m_pos;
	discriminant = dot(ray, distance)*dot(ray, distance) - (dot(distance, distance) - m_r*m_r);

	// If the discriminant is less than zero, the ray does not intersect with the sphere.
	// Return -1 to indicate no intersection.
	if (discriminant < 0)
		return -1;

	// Otherwise, calculate the closest intersection in front of the origin and return it.
	quadraticSub = -dot(ray, distance) - sqrt(discriminant);
	quadraticAdd = -dot(ray, distance) + sqrt(discriminant);
	if (quadraticSub >= 0 && quadraticAdd >= 0)
	{
		return min(quadraticSub, quadraticAdd);
	}
	else if (quadraticSub < 0)
	{
		return quadraticAdd;
	}
	return quadraticSub;
}

Color Sphere::shade(Vec3 ray, Vec3 origin, double t)
{
	return m_c;
}