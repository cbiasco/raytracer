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
class Material;
class Light;
class Vec3;
class Texture;
class Vec3;

using std::vector;

struct RayPayload {
	RayPayload() : object(-1), distance(-1), material(-1) {}
	int object; // Closest-intersected object
	double distance; // Distance to closest object
	int material; // Material of closest object
};

struct Scene {
	vector<Object*> objects;
	vector<Material*> materials;
	vector<Light*> lights;
	vector<Vec3*> vertices;
	vector<Texture*> textures;
	vector<double*> textureCoords;
	vector<Vec3*> vertexNorms;
};