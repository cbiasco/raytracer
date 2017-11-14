#pragma once
//
// Object.h
//
// Code by Caleb Biasco (biasc007) for Assignment-1 of CSCI5607

/*
 * The Object class is the superclass of all rendered 3D objects.
 * Any general rendering functions should be specified here.
 */
 
#include <vector>
#include <string>
#include "Color.h"
#include "Material.h"
#include "Texture.h"
#include "Light.h"
#include "Vec3.h"
#include "structs.h"

using std::vector;
using std::string;

class Object
{
public:
	Object();
	Object(int mat, int texture);
	Object(double x, double y, double z, int mat, int texture);
	Object(Vec3 v, int mat, int texture);
	virtual ~Object();

	// Pure virtual function for testing ray intersections.
	virtual bool intersection(Vec3 &ray, Vec3 &origin, RayPayload &payload) = 0;
	
	// Pure virtual function for getting the color of the object at a given point.
	virtual Color objectDiffuse(Vec3 N, Scene &scene, RayPayload &payload) = 0;
	
	// Pure virtual function for getting the N vector of the object.
	virtual Vec3 getN(Vec3 intersectionPoint) = 0;

	// Returns the name of the object as a string
	virtual string getName();

	Vec3 m_pos; // a vector of the object's position
	int m_mat; // the material of the object
	int m_texture; // the index of the object's texture in the texture array
};

