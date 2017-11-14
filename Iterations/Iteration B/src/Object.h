#pragma once
//
// Object.h
//
// Code by Caleb Biasco (biasc007) for Assignment-1A of CSCI5607

/*
 * The Object class is the superclass of all rendered 3D objects.
 * Any general rendering functions should be specified here.
 */
 
#include <vector>
#include "Color.h"
#include "Material.h"
#include "Light.h"
#include "Vec3.h"

using std::vector;

class Object
{
public:
	Object(double x, double y, double z);
	Object(double x, double y, double z, Material mat);
	Object(Vec3 v);
	Object(Vec3 v, Material mat);
	virtual ~Object();

	// Pure virtual function for testing ray intersections.
	virtual double intersection(Vec3 ray, Vec3 origin) = 0;
	
	// Pure virtual function for getting the color from the object.
	virtual Color shade(Vec3 ray, Vec3 origin, double t, vector<Light*> lights,
		vector<Object*> objects, int index) = 0;

	Vec3 m_pos; // a vector of the object's position
	double m_minParam; // the smallest parameter of the object
	Material m_mat; // the material of the object
};

