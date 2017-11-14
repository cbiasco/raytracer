#pragma once
//
// structs.h
//
// Code by Caleb Biasco (biasc007) for Assignment-1 of CSCI5607

/*
 * Container for miscellaneous data types used to accelerate the program
 */

#include <vector>

class Object;
class Color;
class Material;
class Light;
class Vec3;
class Texture;
class Vec3;

using std::vector;

struct RayPayload {
	RayPayload() : object(-1), distance(-1), material(-1), originObject(-1),
		F(1.0), recursionCount(0), eta(1.0) {}
	int object; // Closest-intersected object
	double distance; // Distance to closest object
	int material; // Material of closest object
	int originObject; // Object that the ray may originate from
	double F; // Fresnel reflection coefficient tracker
	int recursionCount; // Counter for how deeply into recursion we are
	vector<int> *inObjects; // Pointer to stack of objects that the ray is currently inside of
	double eta; // current index of refraction
};

struct Scene {
	Scene() : background(Color(0, 0, 0)), eta(1.0) {}
	Color background;
	double eta;
	vector<Object*> objects;
	vector<Material*> materials;
	vector<Light*> lights;
	vector<Vec3*> vertices;
	vector<Texture*> textures;
	vector<double*> textureCoords;
	vector<Vec3*> vertexNorms;
};
