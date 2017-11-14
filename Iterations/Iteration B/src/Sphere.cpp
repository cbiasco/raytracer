//
// Sphere.cpp
//
// Code by Caleb Biasco (biasc007) for Assignment-1B of CSCI5607

/*
 * The Sphere class inherits the Object superclass and also holds a radius value.
 */

#include <cmath>
#include <algorithm>
#include "Sphere.h"

using std::min;
using std::max;
using std::pow;
using std::vector;

Sphere::Sphere(double x, double y, double z, double r) : Object(x, y, z)
{
	m_r = r;
	m_minParam = r;
}

Sphere::Sphere(double x, double y, double z, double r, Material mat) : Object(x, y, z, mat)
{
	m_r = r;
	m_minParam = r;
}

Sphere::Sphere(Vec3 v, double r) : Object(v)
{
	m_r = r;
	m_minParam = r;
}

Sphere::Sphere(Vec3 v, double r, Material mat) : Object(v, mat)
{
	m_r = r;
	m_minParam = r;
}

Sphere::~Sphere() {}

double Sphere::intersection(Vec3 ray, Vec3 origin)
{
	Vec3 distance, normRay;
	double discriminant, quadraticAdd, quadraticSub;
	distance = origin - m_pos;
	normRay = ray;
	normRay.normalize();
	discriminant = dot(normRay, distance)*dot(normRay, distance) - (dot(distance, distance) - m_r*m_r);

	// If the discriminant is less than zero, the ray does not intersect with the sphere.
	// Return -1 to indicate no intersection.
	if (discriminant < 0)
		return -1;

	// Otherwise, calculate the closest intersection in front of the origin and return it.
	quadraticSub = -dot(normRay, distance) - sqrt(discriminant);
	quadraticAdd = -dot(normRay, distance) + sqrt(discriminant);
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

Color Sphere::shade(Vec3 ray, Vec3 origin, double t, vector<Light*> lights, vector<Object*> objects, int index)
{
	Vec3 N, L, H;
	double shadow;
	Vec3 intersectionPoint = origin + ray*t;
	Vec3 V = ray*(-1);
	V.normalize();
	Color shade = m_mat.getAmbient();
	
	for (int i = 0; i < lights.size(); i++)
	{
		shadow = lights[i]->shadow(objects, intersectionPoint, index);
		
		// Allows for soft shadowing, if implemented
		if (shadow > 0)
		{
			L = lights[i]->getL(intersectionPoint);
			
			N = intersectionPoint - m_pos;
			N.normalize();
		
			H = L + V;
			H.normalize();
		
			// Shades according to the Phong equation.
			// Clamps afterward to avoid overflow.
			shade = shade + shadow*(lights[i]->m_c * (m_mat.getDiffuse() * m_mat.m_kd * max(0.0, dot(N, L)) +
					m_mat.getSpecular() * m_mat.m_ks * pow(max(0.0, dot(N, H)), m_mat.m_n)));
			shade = shade.clampColor();
		}
	}
	
	return shade;
}