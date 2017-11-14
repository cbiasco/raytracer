#pragma once
//
// Spotlight.h
//
// Code by Caleb Biasco (biasc007) for Assignment-1 of CSCI5607

/*
 * The Spotlight class is a Light subclass that holds the parameters and behaviors
 * of a spotlight.
 */

#include "Light.h"
#include "Object.h"

using std::vector;

class Spotlight :
	public Light
{
public:
	Spotlight(double x, double y, double z, double dx, double dy, double dz,
		double theta);
	Spotlight(double x, double y, double z, double dx, double dy, double dz,
		double theta, Color c);
	Spotlight(Vec3 m_pos, Vec3 m_dir, double theta);
	Spotlight(Vec3 m_pos, Vec3 m_dir, double theta, Color c);
	Spotlight(double sl[10]);
	virtual ~Spotlight();
	
	// Calculates the L vector from the light.
	Vec3 getL(Vec3 &intersectionPoint);
	
	// Pure virtual function for calculating shadows cast onto the object.
	// Includes soft shadowing.
	double shadow(Vec3 &intersectionPoint, Scene &scene, RayPayload &payload);

	Vec3 m_pos; // a vector of the light's position
	Vec3 m_dir; // a vector of the light's direction
	double m_theta; // the angle of illumination for the spotlight
};