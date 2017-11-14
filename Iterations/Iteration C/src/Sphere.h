#pragma once
//
// Sphere.h
//
// Code by Caleb Biasco (biasc007) for Assignment-1 of CSCI5607

/*
 * The Sphere class inherits the Object superclass and also holds a radius value.
 */

#include "Object.h"

using std::vector;

class Sphere :
	public Object
{
public:
	Sphere(double x, double y, double z, double r, int mat, int texture);
	Sphere(Vec3 v, double r, int mat, int texture);
	Sphere(double s[4], int mat, int texture);
	virtual ~Sphere();

	// Tests ray intersection using the sphere implicit surface function.
	bool intersection(Vec3 &ray, Vec3 &origin, RayPayload &payload);
	
	// Gets the color of the object at a given point.
	Color objectDiffuse(Vec3 N, Scene &scene, RayPayload &payload);
	
	// Gets the N vector of the objects.
	Vec3 getN(Vec3 intersectionPoint);
	
	double m_r; // the radius of the sphere
};

