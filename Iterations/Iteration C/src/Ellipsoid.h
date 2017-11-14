#pragma once
//
// Ellipsoid.h
//
// Code by Caleb Biasco (biasc007) for Assignment-1 of CSCI5607

/*
 * The Ellipsoid class inherits the Object superclass and also holds a three radius values.
 */

#include "Object.h"

using std::vector;

class Ellipsoid :
	public Object
{
public:
	Ellipsoid(double x, double y, double z, double rx, double ry, double rz, int mat,
		int texture);
	Ellipsoid(Vec3 v, double rx, double ry, double rz, int mat, int texture);
	Ellipsoid(Vec3 v, Vec3 r, int mat, int texture);
	Ellipsoid(double e[6], int mat, int texture);
	virtual ~Ellipsoid();

	// Tests ray intersection using the ellipsoid implicit surface function.
	bool intersection(Vec3 &ray, Vec3 &origin, RayPayload &payload);

	// Gets the color of the object at a given point.
	Color objectDiffuse(Vec3 N, Scene &scene, RayPayload &payload);
	
	// Calculates the N vector of the object.
	Vec3 getN(Vec3 intersectionPoint);

	Vec3 m_r; // the radii of the ellipsoid
};

