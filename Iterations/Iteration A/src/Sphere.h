#pragma once
//
// Sphere.h
//
// Code by Caleb Biasco (biasc007) for Assignment-1A of CSCI5607

/*
 * The Sphere class inherits the Object superclass and also holds a radius value.
 */

#include "Object.h"

class Sphere :
	public Object
{
public:
	Sphere(double x, double y, double z, double r);
	Sphere(double x, double y, double z, double r, Color c);
	Sphere(Vec3 v, double r);
	Sphere(Vec3 v, double r, Color c);
	virtual ~Sphere();

	// Tests ray intersection using the sphere implicit surface function.
	double intersection(Vec3 ray, Vec3 origin);

	// Calculates the color from the intersected ray on the object.
	Color shade(Vec3 ray, Vec3 origin, double t);

	double m_r; // the radius of the sphere
};

