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
	Material m_mat;
}

Object::Object(double x, double y, double z, Material mat)
{
	m_pos = Vec3(x, y, z);
	m_mat = mat;
}

Object::Object(Vec3 v)
{
	m_pos = v;
	Material m_mat;
}

Object::Object(Vec3 v, Material mat)
{
	m_pos = v;
	m_mat = mat;
}

Object::~Object() {}