//
// Ellipsoid.h
//
// Code by Caleb Biasco (biasc007) for Assignment-1 of CSCI5607

/*
 * The Ellipsoid class inherits the Object superclass and also holds a three radius values.
 */

#include <cmath>
#include <algorithm>
#include "Ellipsoid.h"

using std::min;
using std::max;
using std::vector;

const double PI = 4 * atan(1.0);

Ellipsoid::Ellipsoid(double x, double y, double z, double rx, double ry, double rz,
	int mat, int texture) : Object(x, y, z, mat, texture)
{
	m_r = Vec3(rx, ry, rz);
}

Ellipsoid::Ellipsoid(Vec3 v, double rx, double ry, double rz, int mat, int texture)
	: Object(v, mat, texture)
{
	m_r = Vec3(rx, ry, rz);
}

Ellipsoid::Ellipsoid(Vec3 v, Vec3 r, int mat, int texture)
	: Object(v, mat, texture)
{
	m_r = r;
}

Ellipsoid::Ellipsoid(double e[6], int mat, int texture) : Object(e[0], e[1], e[2], mat, texture)
{
	m_r = Vec3(e[3], e[4], e[5]);
}

Ellipsoid::~Ellipsoid() {}

bool Ellipsoid::intersection(Vec3 &ray, Vec3 &origin, RayPayload &payload)
{
	Vec3 direction;
	Vec3 radRay; // a ray copy to be divided by the radii vector
	double discriminant, quadraticAdd, quadraticSub, distance;
	direction = (origin - m_pos) / m_r;
	radRay = ray / m_r;
	discriminant = dot(direction, radRay)*dot(direction, radRay) -
		dot(radRay, radRay)*(dot(direction, direction) - 1);

	// If the discriminant is less than zero, the ray does not intersect with the ellipsoid.
	// Return -1 to indicate no intersection.
	if (discriminant < 0)
		return false;

	// Otherwise, calculate the closest intersection in front of the origin and return it.
	quadraticSub = (-dot(radRay, direction) - sqrt(discriminant)) / dot(radRay, radRay);
	quadraticAdd = (-dot(radRay, direction) + sqrt(discriminant)) / dot(radRay, radRay);
	if (quadraticSub >= 0 && quadraticAdd >= 0)
		distance = min(quadraticSub, quadraticAdd);
	else if (quadraticSub < 0)
		distance = quadraticAdd;
	else
		distance = quadraticSub;
	
	if (payload.distance > -1 && payload.distance < distance)
		return false;
	payload.distance = distance;
	payload.material = m_mat;
	return true;
}

Color Ellipsoid::objectDiffuse(Vec3 intersectionPoint, Scene &scene, RayPayload &payload)
{
	if (m_texture == -1)
		return scene.materials[payload.material]->getDiffuse();
	
	Vec3 N = getN(intersectionPoint);
	double phi = acos(N.m_y); // poles are aligned with Y axis
	double theta = atan2(N.m_x, N.m_z);
	if (theta < 0)
		theta = theta + 2 * PI;
	return scene.textures[m_texture]->getColor(theta/(2*PI), phi/PI);
}

Vec3 Ellipsoid::getN(Vec3 intersectionPoint)
{
	Vec3 N = (intersectionPoint - m_pos) / m_r;
	N.normalize();
	return N;
}