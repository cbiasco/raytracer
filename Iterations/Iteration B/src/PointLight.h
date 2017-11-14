#pragma once
//
// PointLight.h
//
// Code by Caleb Biasco (biasc007) for Assignment-1B of CSCI5607

/*
 * The PointLight class is a Light subclass that holds the behavior
 * of a point light.
 */

#include "Light.h"
#include "Object.h"

using std::vector;

class PointLight :
	public Light
{
public:
	PointLight(double x, double y, double z);
	PointLight(double x, double y, double z, Color c);
	PointLight(Vec3 v);
	PointLight(Vec3 v, Color c);
	virtual ~PointLight();
	
	// Calculates the L vector from the light.
	Vec3 getL(Vec3 intersectionPoint);
	
	// Pure virtual function for calculating shadows cast onto the object.
	// Includes soft shadowing.
	double shadow(vector<Object*> objects, Vec3 intersectionPoint, int index);

	Vec3 m_pos; // a vector of the light's position
};