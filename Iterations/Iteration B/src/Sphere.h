#pragma once
//
// Sphere.h
//
// Code by Caleb Biasco (biasc007) for Assignment-1B of CSCI5607

/*
 * The Sphere class inherits the Object superclass and also holds a radius value.
 */

#include "Object.h"

using std::vector;

class Sphere :
	public Object
{
public:
	Sphere(double x, double y, double z, double r);
	Sphere(double x, double y, double z, double r, Material mat);
	Sphere(Vec3 v, double r);
	Sphere(Vec3 v, double r, Material mat);
	virtual ~Sphere();

	// Tests ray intersection using the sphere implicit surface function.
	double intersection(Vec3 ray, Vec3 origin);

	// Calculates the color from the intersected ray on the sphere.
	Color shade(Vec3 ray, Vec3 origin, double t, vector<Light*> lights,
		vector<Object*> objects, int index);
		
	double m_r; // the radius of the sphere
};

