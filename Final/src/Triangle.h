#pragma once
//
// Triangle.h
//
// Code by Caleb Biasco (biasc007) for Assignment-1 of CSCI5607

/*
 * The Triangle class inherits the Object superclass, holds the
 * vertex/texture/normal array indices for its three vertices, and
 * implements the scene traversal functions.
 */

#include "Object.h"

using std::vector;
using std::string;

class Triangle :
	public Object
{
public:
	Triangle(int v1, int v2, int v3,
		int vt1, int vt2, int vt3,
		int vn1, int vn2, int vn3,
		int mat, int texture);
	Triangle(double t[9], int mat, int texture);
	virtual ~Triangle();

	// Tests ray intersection using the barycentric coordinates of the triangle.
	bool intersection(Vec3 &ray, Vec3 &origin, RayPayload &payload);

	// Gets the color of the object at a given point.
	Color objectDiffuse(Vec3 N, Scene &scene, RayPayload &payload);
	
	// Gets the N vector of the object.
	Vec3 getN(Vec3 intersectionPoint);

	// Returns the name of the object as a string.
	string getName();

	// Calculates and sets the barycentric coordinates from the last
	// intersection with the triangle.
	double *baryCoords(Vec3 point);
	
	// Sets the scene to pull the vertices from
	static void setScene(Scene &newScene);

	int m_p0; // vertex 0 index
	int m_p1; // vertex 1 index
	int m_p2; // vertex 2 index
	int m_t0; // texture coordinate 0 index
	int m_t1; // texture coordinate 1 index
	int m_t2; // texture coordinate 2 index
	int m_n0; // vertex normal 0 index
	int m_n1; // vertex normal 1 index
	int m_n2; // vertex normal 2 index
	static Scene *scene; // scene to pull vertex data from
	
	// Helper functions to simplify using the vector list.
	// Returns a Vec3 of the point.
	Vec3 p0();
	Vec3 p1();
	Vec3 p2();

	// Helper functions to simplify using the texture coordinate list.
	// Returns a texture coordinate array of the two coordinates.
	double *t0();
	double *t1();
	double *t2();

	// Helper functions to simplify using the vertex normal list.
	// Returns a Vec3 of the vector.
	Vec3 n0();
	Vec3 n1();
	Vec3 n2();

};

