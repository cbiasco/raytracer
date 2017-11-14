//
// Object.cpp
//
// Code by Caleb Biasco (biasc007) for Assignment-1 of CSCI5607

/*
 * The Object class is the superclass of all rendered 3D objects.
 * Any general rendering functions should be specified here.
 */

#include "Object.h"

using std::string;

Object::Object()
{
	m_pos = Vec3(0.0);
	m_mat = 0;
}

Object::Object(int mat, int texture)
{
	m_pos = Vec3(0.0);
	m_mat = mat;
	m_texture = texture;
}

Object::Object(double x, double y, double z, int mat, int texture)
{
	m_pos = Vec3(x, y, z);
	m_mat = mat;
	m_texture = texture;
}

Object::Object(Vec3 v, int mat, int texture)
{
	m_pos = v;
	m_mat = mat;
	m_texture = texture;
}

Object::~Object() {}

string Object::getName()
{
	return "Object";
}
