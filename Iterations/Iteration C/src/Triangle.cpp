//
// Triangle.cpp
//
// Code by Caleb Biasco (biasc007) for Assignment-1 of CSCI5607

/*
 * The Triangle class inherits the Object superclass, holds the
 * vertex/texture/normal array indices for its three vertices, and
 * implements the scene traversal functions.
 */

#include <cmath>
#include <algorithm>
#include "Triangle.h"

using std::min;
using std::max;
using std::pow;
using std::vector;

Scene *Triangle::scene;

Triangle::Triangle(int v1, int v2, int v3,
	int vt1, int vt2, int vt3,
	int vn1, int vn2, int vn3,
	int mat, int texture)
	: Object(mat, texture)
{
	m_p0 = v1;
	m_p1 = v2;
	m_p2 = v3;
	m_t0 = vt1;
	m_t1 = vt2;
	m_t2 = vt3;
	m_n0 = vn1;
	m_n1 = vn2;
	m_n2 = vn3;
}

Triangle::Triangle(double t[9], int mat, int texture)
	: Object(mat, texture)
{
	m_p0 = t[0];
	m_p1 = t[1];
	m_p2 = t[2];
	m_t0 = t[3];
	m_t1 = t[4];
	m_t2 = t[5];
	m_n0 = t[6];
	m_n1 = t[7];
	m_n2 = t[8];
}

Triangle::~Triangle() {}

void Triangle::setScene(Scene &newScene)
{
	scene = &newScene;
}

Vec3 Triangle::p0()
{
	return *(scene->vertices[m_p0]);
}

Vec3 Triangle::p1()
{
	return *(scene->vertices[m_p1]);
}

Vec3 Triangle::p2()
{
	return *(scene->vertices[m_p2]);
}

double *Triangle::t0()
{
	return scene->textureCoords[m_t0];
}

double *Triangle::t1()
{
	return scene->textureCoords[m_t1];
}

double *Triangle::t2()
{
	return scene->textureCoords[m_t2];
}

Vec3 Triangle::n0()
{
	return *(scene->vertexNorms[m_n0]);
}

Vec3 Triangle::n1()
{
	return *(scene->vertexNorms[m_n1]);
}

Vec3 Triangle::n2()
{
	return *(scene->vertexNorms[m_n2]);
}

bool Triangle::intersection(Vec3 &ray, Vec3 &origin, RayPayload &payload)
{
	Vec3 n;
	n = cross(p1() - p0(), p2() - p0());
	double denominator = n.m_x*ray.m_x + n.m_y*ray.m_y + n.m_z*ray.m_z;
	
	// If the denominator is equal to zero, the ray does not intersect with the plane of the triangle.
	// Return -1 to indicate no intersection.
	if (denominator == 0)
		return false;
	
	// If the triangle is behind the view, we don't care about the intersection.
	double D = -(n.m_x*p0().m_x + n.m_y*p0().m_y + n.m_z*p0().m_z);
	double t = -(n.m_x*origin.m_x + n.m_y*origin.m_y + n.m_z*origin.m_z + D)/denominator;
	
	if (t < 0 || (payload.distance > -1 && payload.distance < t))
		return false;
	
	// Otherwise, check if the intersection falls within the triangle's area.
	double *coords = baryCoords(origin + ray*t);
	
	// If the subtriangles fit within the triangle, then the point lies within the triangle.
	if (coords[0] + coords[1] + coords[2] - 1 < .000001 &&
		coords[0] >= 0 && coords[0] <= 1 &&
		coords[1] >= 0 && coords[1] <= 1 &&
		coords[2] >= 0 && coords[2] <= 1)
	{
		payload.material = m_mat;
		payload.distance = t;
		delete coords;
		return true;
	}
	delete coords;
	return false;
}

Color Triangle::objectDiffuse(Vec3 intersectionPoint, Scene &scene, RayPayload &payload)
{
	if (m_texture == -1 || m_t0 == -1)
		return scene.materials[payload.material]->getDiffuse();
	
	double *c0 = t0();
	double *c1 = t1();
	double *c2 = t2();
	double *coords = baryCoords(intersectionPoint);
	
	double u = coords[0]*c0[0] + coords[1]*c1[0] + coords[2]*c2[0];
	double v = coords[0]*c0[1] + coords[1]*c1[1] + coords[2]*c2[1];

	delete coords;
	return scene.textures[m_texture]->getColor(u, v);
}

Vec3 Triangle::getN(Vec3 intersectionPoint)
{
	Vec3 N;
	if (m_n0 == -1)
		N = cross(p1() - p0(), p2() - p0());
	else
	{
		double *coords = baryCoords(intersectionPoint);
		N = n0()*coords[0] + n1()*coords[1] + n2()*coords[2];
		delete coords;
	}
	
	N.normalize();
	return N;
}

double *Triangle::baryCoords(Vec3 point)
{
	double area;
	double *coords = new double[3];
	Vec3 e1, e2, e3, e4;

	e1 = p1() - p0();
	e2 = p2() - p0();
	e3 = point - p1();
	e4 = point - p2();

	area = cross(e1, e2).length() / 2;
	coords[0] = cross(e3, e4).length() / (2 * area);
	coords[1] = cross(e4, e2).length() / (2 * area);
	coords[2] = cross(e1, e3).length() / (2 * area);
	
	return coords;
}