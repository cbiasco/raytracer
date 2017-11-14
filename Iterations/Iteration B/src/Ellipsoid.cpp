//
// Ellipsoid.h
//
// Code by Caleb Biasco (biasc007) for Assignment-1B of CSCI5607

/*
 * The Ellipsoid class inherits the Object superclass and also holds a three radius values.
 */

#include <cmath>
#include <algorithm>
#include "Ellipsoid.h"

using std::min;
using std::max;
using std::vector;

Ellipsoid::Ellipsoid(double x, double y, double z, double rx, double ry, double rz) :
	Object(x, y, z)
{
	m_rx = rx;
	m_ry = ry;
	m_rz = rz;
	m_minParam = min(rx, min(ry, rz));
}

Ellipsoid::Ellipsoid(double x, double y, double z, double rx, double ry, double rz,
	Material mat) : Object(x, y, z, mat)
{
	m_rx = rx;
	m_ry = ry;
	m_rz = rz;
	m_minParam = min(rx, min(ry, rz));
}

Ellipsoid::Ellipsoid(Vec3 v, double rx, double ry, double rz) : Object(v)
{
	m_rx = rx;
	m_ry = ry;
	m_rz = rz;
	m_minParam = min(rx, min(ry, rz));
}

Ellipsoid::Ellipsoid(Vec3 v, double rx, double ry, double rz, Material mat) : Object(v, mat)
{
	m_rx = rx;
	m_ry = ry;
	m_rz = rz;
	m_minParam = min(rx, min(ry, rz));
}

Ellipsoid::~Ellipsoid() {}

double Ellipsoid::intersection(Vec3 ray, Vec3 origin)
{
	Vec3 distance;
	Vec3 radVec; // a vector of the radii, to simplify calculations
	Vec3 radRay; // a ray copy to be divided by the radii vector
	double discriminant, quadraticAdd, quadraticSub;
	radVec = Vec3(m_rx, m_ry, m_rz);
	distance = (origin - m_pos) / radVec;
	radRay = ray / radVec;
	discriminant = dot(distance, radRay)*dot(distance, radRay) -
		dot(radRay, radRay)*(dot(distance, distance) - 1);

	// If the discriminant is less than zero, the ray does not intersect with the ellipsoid.
	// Return -1 to indicate no intersection.
	if (discriminant < 0)
		return -1;

	// Otherwise, calculate the closest intersection in front of the origin and return it.
	quadraticSub = (-dot(radRay, distance) - sqrt(discriminant)) / dot(radRay, radRay);
	quadraticAdd = (-dot(radRay, distance) + sqrt(discriminant)) / dot(radRay, radRay);
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

Color Ellipsoid::shade(Vec3 ray, Vec3 origin, double t, vector<Light*> lights, vector<Object*> objects, int index)
{
	Vec3 N, L, H;
	double shadow;
	Vec3 intersectionPoint = origin + ray*t;
	Vec3 radSVec(m_rx*m_rx, m_ry*m_ry, m_rz*m_rz); // Used to find normal of the surface
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
			
			N = (intersectionPoint - m_pos)/radSVec;
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