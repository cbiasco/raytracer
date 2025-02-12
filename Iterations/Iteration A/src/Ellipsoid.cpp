//
// Ellipsoid.h
//
// Code by Caleb Biasco (biasc007) for Assignment-1A of CSCI5607

/*
 * The Ellipsoid class inherits the Object superclass and also holds a three radius values.
 */

#include <cmath>
#include <algorithm>
#include "Ellipsoid.h"

using std::min;

Ellipsoid::Ellipsoid(double x, double y, double z, double rx, double ry, double rz) :
	Object(x, y, z)
{
	m_rx = rx;
	m_ry = ry;
	m_rz = rz;
}

Ellipsoid::Ellipsoid(double x, double y, double z, double rx, double ry, double rz,
	Color c) : Object(x, y, z, c)
{
	m_rx = rx;
	m_ry = ry;
	m_rz = rz;
}

Ellipsoid::Ellipsoid(Vec3 v, double rx, double ry, double rz) : Object(v)
{
	m_rx = rx;
	m_ry = ry;
	m_rz = rz;
}

Ellipsoid::Ellipsoid(Vec3 v, double rx, double ry, double rz, Color c) : Object(v, c)
{
	m_rx = rx;
	m_ry = ry;
	m_rz = rz;
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

Color Ellipsoid::shade(Vec3 ray, Vec3 origin, double t)
{
	return m_c;
}