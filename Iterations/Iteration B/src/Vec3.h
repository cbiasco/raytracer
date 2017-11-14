#pragma once
//
// Vec3.h
//
// Code by Caleb Biasco (biasc007) for Assignment-1A of CSCI5607

/*
 * The Vec3 class represents 3-dimensional vectors and holds component
 * operations for them, including vector addition and subtraction,
 * scalar multiplication and division, and cross and dot products.
 */

class Vec3
{
public:
	Vec3();
	Vec3(double a);
	Vec3(double x, double y, double z);
	virtual ~Vec3();

	// Component-wise operations
	friend bool operator== (const Vec3& a, const Vec3& b);
	friend Vec3 operator+ (const Vec3& a, const Vec3& b);
	friend Vec3 operator- (const Vec3& a, const Vec3& b);
	friend Vec3 operator/ (const Vec3& a, double d);
	friend Vec3 operator/ (const Vec3& a, const Vec3& b);
	friend Vec3 operator* (const Vec3& a, double d);
	friend Vec3 cross(const Vec3& a, const Vec3& b);
	friend double dot(const Vec3& a, const Vec3& b);

	// Calculates the norm of the vector.
	double length();
	
	// Normalizes the vector (changes member attributes!).
	void normalize();

	double m_x; // the x component of the vector
	double m_y; // the y component of the vector
	double m_z; // the z component of the vector
};

