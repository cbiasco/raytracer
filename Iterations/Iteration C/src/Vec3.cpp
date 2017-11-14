//
// Vec3.cpp
//
// Code by Caleb Biasco (biasc007) for Assignment-1 of CSCI5607

/*
 * The Vec3 class represents 3-dimensional vectors and holds component
 * operations for them, including vector addition and subtraction,
 * scalar multiplication and division, and cross and dot products.
 */

#include <cmath>
#include "Vec3.h"

Vec3::Vec3()
{
	m_x = 0;
	m_y = 0;
	m_z = 0;
}

Vec3::Vec3(double a)
{
	m_x = a;
	m_y = a;
	m_z = a;
}

Vec3::Vec3(double x, double y, double z)
{
	m_x = x;
	m_y = y;
	m_z = z;
}
Vec3::Vec3(double v[3])
{
	m_x = v[0];
	m_y = v[1];
	m_z = v[2];
}

Vec3::~Vec3() {}

double Vec3::length()
{
	return sqrt(m_x*m_x + m_y*m_y + m_z*m_z);
}

void Vec3::normalize()
{
	double len = length();
	m_x /= len;
	m_y /= len;
	m_z /= len;
}

// Component-wise operations
bool operator== (const Vec3& a, const Vec3& b)
{
	if (a.m_x == b.m_x && a.m_y == b.m_y && a.m_z == b.m_z)
		return true;
	return false;
}

Vec3 operator+ (const Vec3& a, const Vec3& b)
{
	return Vec3(a.m_x + b.m_x, a.m_y + b.m_y, a.m_z + b.m_z);
}

Vec3 operator- (const Vec3& a, const Vec3& b)
{
	return Vec3(a.m_x - b.m_x, a.m_y - b.m_y, a.m_z - b.m_z);
}

Vec3 operator/ (const Vec3& a, double d)
{
	return Vec3(a.m_x / d, a.m_y / d, a.m_z / d);
}

Vec3 operator/ (const Vec3& a, const Vec3& b)
{
	return Vec3(a.m_x / b.m_x, a.m_y / b.m_y, a.m_z / b.m_z);
}

Vec3 operator* (const Vec3& a, double d)
{
	return Vec3(a.m_x * d, a.m_y * d, a.m_z * d);
}

Vec3 cross(const Vec3& a, const Vec3& b)
{
	double x, y, z;
	x = a.m_y*b.m_z - a.m_z*b.m_y;
	y = a.m_z*b.m_x - a.m_x*b.m_z;
	z = a.m_x*b.m_y - a.m_y*b.m_x;
	return Vec3(x, y, z);
}

double dot(const Vec3& a, const Vec3& b)
{
	return a.m_x*b.m_x + a.m_y*b.m_y + a.m_z*b.m_z;
}