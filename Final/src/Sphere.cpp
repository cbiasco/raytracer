//
// Sphere.cpp
//
// Code by Caleb Biasco (biasc007) for Assignment-1 of CSCI5607

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

const double PI = 4 * atan(1.0);

Sphere::Sphere(double x, double y, double z, double r, int mat, int texture)
	: Object(x, y, z, mat, texture)
{
	m_r = r;
}

Sphere::Sphere(Vec3 v, double r, int mat, int texture) : Object(v, mat, texture)
{
	m_r = r;
}

Sphere::Sphere(double s[4], int mat, int texture) : Object(s[0], s[1], s[2], mat, texture)
{
	m_r = s[3];
}

Sphere::~Sphere() {}

bool Sphere::intersection(Vec3 &ray, Vec3 &origin, RayPayload &payload)
{
	Vec3 direction;
	double discriminant, quadraticAdd, quadraticSub, distance;
	direction = origin - m_pos;
	discriminant = dot(ray, direction)*dot(ray, direction) - (dot(direction, direction) - m_r*m_r);

	// If the discriminant is less than zero, the ray does not intersect with the sphere.
	// Return -1 to indicate no intersection.
	if (discriminant < 0)
		return false;

	// Otherwise, calculate the closest intersection in front of the origin and return it.
	quadraticSub = -dot(ray, direction) - sqrt(discriminant);
	quadraticAdd = -dot(ray, direction) + sqrt(discriminant);
	if (quadraticSub >= 0 && quadraticAdd >= 0)
		distance = min(quadraticSub, quadraticAdd);
	else if (quadraticSub < 0)
		distance = quadraticAdd;
	else
		distance = quadraticSub;
	
	if (distance <= 0 || (payload.distance > -1 && payload.distance < distance))
		return false;
	
	payload.distance = distance;
	payload.material = m_mat;
	return true;
}

Color Sphere::objectDiffuse(Vec3 intersectionPoint, Scene &scene, RayPayload &payload)
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

Vec3 Sphere::getN(Vec3 intersectionPoint)
{
	Vec3 N = intersectionPoint - m_pos;
	N.normalize();
	return N;
}

string Sphere::getName()
{
	return "Sphere";
}
