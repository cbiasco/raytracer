#pragma once
//
// Ellipsoid.h
//
// Code by Caleb Biasco (biasc007) for Assignment-1B of CSCI5607

/*
 * The Ellipsoid class inherits the Object superclass and also holds a three radius values.
 */

#include "Object.h"

using std::vector;

class Ellipsoid :
	public Object
{
public:
	Ellipsoid(double x, double y, double z, double rx, double ry, double rz);
	Ellipsoid(double x, double y, double z, double rx, double ry, double rz, Material mat);
	Ellipsoid(Vec3 v, double rx, double ry, double rz);
	Ellipsoid(Vec3 v, double rx, double ry, double rz, Material mat);
	virtual ~Ellipsoid();

	// Tests ray intersection using the sphere implicit surface function.
	double intersection(Vec3 ray, Vec3 origin);

	// Calculates the color from the intersected ray on the ellipsoid.
	Color shade(Vec3 ray, Vec3 origin, double t, vector<Light*> lights,
		vector<Object*> objects, int index);

	double m_rx; // the x radius of the sphere
	double m_ry; // the y radius of the sphere
	double m_rz; // the z radius of the sphere
};

