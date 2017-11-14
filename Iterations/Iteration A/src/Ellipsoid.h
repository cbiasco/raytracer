#pragma once
//
// Ellipsoid.h
//
// Code by Caleb Biasco (biasc007) for Assignment-1A of CSCI5607

/*
 * The Ellipsoid class inherits the Object superclass and also holds a three radius values.
 */

#include "Object.h"

class Ellipsoid :
	public Object
{
public:
	Ellipsoid(double x, double y, double z, double rx, double ry, double rz);
	Ellipsoid(double x, double y, double z, double rx, double ry, double rz, Color c);
	Ellipsoid(Vec3 v, double rx, double ry, double rz);
	Ellipsoid(Vec3 v, double rx, double ry, double rz, Color c);
	virtual ~Ellipsoid();

	// Tests ray intersection using the sphere implicit surface function.
	double intersection(Vec3 ray, Vec3 origin);

	// Calculates the color from the intersected ray on the object.
	Color shade(Vec3 ray, Vec3 origin, double t);

	double m_rx; // the x radius of the sphere
	double m_ry; // the y radius of the sphere
	double m_rz; // the z radius of the sphere
};

