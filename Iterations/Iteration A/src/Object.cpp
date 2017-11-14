//
// Object.cpp
//
// Code by Caleb Biasco (biasc007) for Assignment-1A of CSCI5607

/*
 * The Object class is the superclass of all rendered 3D objects.
 * Any general rendering functions should be specified here.
 */

#include "Object.h"

Object::Object(double x, double y, double z)
{
	m_pos = Vec3(x, y, z);
	Color m_c;
}

Object::Object(double x, double y, double z, Color c)
{
	m_pos = Vec3(x, y, z);
	m_c = c;
}

Object::Object(Vec3 v)
{
	m_pos = v;
	Color m_c;
}

Object::Object(Vec3 v, Color c)
{
	m_pos = v;
	m_c = c;
}

Object::~Object() {}