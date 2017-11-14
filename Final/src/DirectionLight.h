#pragma once
//
// DirectionLight.h
//
// Code by Caleb Biasco (biasc007) for Assignment-1 of CSCI5607

/*
 * The DirectionLight class is a Light subclass that holds the behavior
 * of a direction light.
 */

#include "Light.h"
#include "Object.h"

using std::vector;

class DirectionLight :
	public Light
{
public:
	DirectionLight(double x, double y, double z);
	DirectionLight(double x, double y, double z, Color c);
	DirectionLight(Vec3 v);
	DirectionLight(Vec3 v, Color c);
	DirectionLight(double l[7]);
	virtual ~DirectionLight();
	
	// Calculates the L vector from the light.
	Vec3 getL(Vec3 &intersectionPoint);
	
	// Pure virtual function for calculating shadows cast onto the object.
	// Includes soft shadowing.
	double shadow(Vec3 &intersectionPoint, Scene &scene, RayPayload &payload);

	Vec3 m_dir; // a vector of the light's direction
};