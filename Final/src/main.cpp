//
// main.cpp
//
// Code by Caleb Biasco (biasc007) for Assignment-1 of CSCI5607

/*
 * This program takes an appropriately formatted .txt file, reads it, calculates
 * its scene variables, and renders the scene as a .ppm file of the same name.
 */

#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <memory>
#include <typeinfo>

#include "Parser.h"
#include "Vec3.h"
#include "Object.h"
#include "Light.h"
#include "Material.h"
#include "Color.h"
#include "Sphere.h"
#include "Ellipsoid.h"
#include "Triangle.h"
#include "DirectionLight.h"
#include "PointLight.h"
#include "Spotlight.h"
#include "Texture.h"
#include "structs.h"

using std::string;
using std::vector;
using std::ofstream;
using std::min;
using std::max;

const double PI = 4 * atan(1.0);
double indexOfRefraction = 1.0;

void traceRay(Vec3 &ray, Vec3 &origin, Scene &scene, RayPayload &payload)
{
	payload.object = -1;
	for (int i = 0; i < scene.objects.size(); i++)
	{
		if (!(i == payload.originObject && payload.distance < 1e-16) && scene.objects[i]->intersection(ray, origin, payload))
			payload.object = i;
	}
}

Color shadeRay(Vec3 &ray, Vec3 &intersectionPoint, Scene &scene, RayPayload &payload)
{
	Vec3 I, N, Nt, L, H, R, T, reflectIntersect, refractIntersect;
	double shadow;
	bool exiting = false;
	
	Color shade, diffuse, specular, reflection, refraction;
	double kd, ks, n, F, alpha, cur_eta, new_eta;
	
	RayPayload reflectPayload;
	RayPayload refractPayload;
	
	// Calculate incoming ray I
	I = ray*(-1);
	I.normalize();
	
	// Store the color variables
	diffuse = scene.objects[payload.object]->objectDiffuse(intersectionPoint, scene, payload);
	shade = scene.materials[payload.material]->getAmbient(diffuse);
	specular = scene.materials[payload.material]->getSpecular();
	reflection = scene.background;
	refraction = scene.background;

	// Store the material variables
	kd = scene.materials[payload.material]->m_kd;
	ks = scene.materials[payload.material]->m_ks;
	n = scene.materials[payload.material]->m_n;
	alpha = scene.materials[payload.material]->m_alpha;

	
	// Calculate N based on entering or exiting an object
	// and set the eta values
	N = scene.objects[payload.object]->getN(intersectionPoint);
	cur_eta = payload.eta;
	if (alpha < 1)
	{
		// Check if the ray is inside of the object or if a triangle has been passed
		// and not exited.
		if (!payload.inObjects->empty() && (payload.inObjects->back() == payload.object ||
		(scene.objects[payload.inObjects->back()]->getName() == "Triangle" &&
		scene.objects[payload.object]->getName() == "Triangle")))
		{
			exiting = true;
			payload.inObjects->pop_back();
			if (payload.inObjects->empty())
				new_eta = scene.eta;
			else
				new_eta = scene.materials[scene.objects[payload.inObjects->back()]->m_mat]->m_eta;
			Nt = N*(-1);
		}
		else
		{
			new_eta = scene.materials[payload.material]->m_eta;
			payload.inObjects->push_back(payload.object);
			Nt = N*(1);
		}
	}
	else
	{
		Nt = N*(1);
		new_eta = scene.materials[payload.material]->m_eta;
	}


	// Calculate the Fresnel reflectance coefficient using the Schlick approximation
	if (alpha < 1)
		F = (new_eta - cur_eta)/(new_eta + cur_eta);
	else
		F = (new_eta - 1)/(new_eta + 1);
	F = F * F;
	if (F < 1)
		F = F + (1 - F)*pow(1 - fabs(dot(I, Nt)), 5);
	
	R = N*(2*dot(N, I)) - I;
	R.normalize();
	
	reflectPayload.originObject = payload.object;
	reflectPayload.F = payload.F * F;
	reflectPayload.inObjects = payload.inObjects;
	reflectPayload.recursionCount = payload.recursionCount + 1;
	
	reflectIntersect = intersectionPoint + R*1e-12; // Avoids self-intersecting
	traceRay(R, reflectIntersect, scene, reflectPayload);
	if (reflectPayload.object > -1 && reflectPayload.recursionCount < 10 && reflectPayload.F > .004)
	{
		reflectPayload.eta = payload.eta;
		reflectIntersect = reflectIntersect + R*reflectPayload.distance;
		reflection = shadeRay(R, reflectIntersect, scene, reflectPayload);
	}


	// Calculate the refracted ray and return its color
	refractPayload.originObject = payload.object;
	refractPayload.F = payload.F * (1 - F);
	refractPayload.inObjects = payload.inObjects;
	refractPayload.recursionCount = payload.recursionCount + 1;
	
	// Check for a critical angle along with recursion limits
	if (alpha < 1 && (new_eta/cur_eta >= 1 || acos(dot(Nt, I)) <= asin(new_eta/cur_eta)) &&
		refractPayload.recursionCount < 10 && refractPayload.F > .004)
	{	
		T = Nt*(-1)*sqrt(1 - (cur_eta/new_eta)*(cur_eta/new_eta)*(1 - dot(Nt, I)*dot(Nt, I))) + (Nt*dot(Nt, I) - I)*(cur_eta/new_eta);
		
		T.normalize();
		refractIntersect = intersectionPoint + T*1e-12; // Avoids self-intersecting
		traceRay(T, refractIntersect, scene, refractPayload);
		
		if (refractPayload.object > -1)
		{
			refractPayload.eta = new_eta;
			refractIntersect = refractIntersect + T*(refractPayload.distance);
			refraction = shadeRay(T, refractIntersect, scene, refractPayload);
		}
		
		// Avoids double-shading
		if (exiting)
		{
			return refraction;
		}
	}


	// Calculate the shading from all of the lights on the object
	for (int i = 0; i < scene.lights.size() && (shade.m_r < 1 || shade.m_g < 1 || shade.m_b < 1); i++)
	{
		shadow = scene.lights[i]->shadow(intersectionPoint, scene, payload);

		if (shadow > 0)
		{
			L = scene.lights[i]->getL(intersectionPoint);

			H = L + I;
			H.normalize();

			// Shade according to extended Phong model.
			// Clamps afterward to avoid overflow.
			shade = shade + shadow*(scene.lights[i]->m_c * (diffuse*kd*max(0.0, dot(N, L)) +
				specular*ks*pow(max(0.0, dot(N, H)), n)));
			shade = shade.clampColor();
		}
	}
	
	// Add together all shading components
	shade = shade + F*reflection + (1-F)*(1-alpha)*refraction;

	return shade.clampColor();
}

int main(int argc, char **argv)
{
	// Make sure an input file was given.
	if (argc < 2)
	{
		std::cerr << "**Error: You must specify an input file, " <<
			"e.g. \"./raycast ../examplefile.txt\"" << std::endl;
		return 0;
	}
	else if (argc > 2)
	{
		std::cerr << "**Error: raycaster only takes one argument." << std::endl;
		return 0;
	}

	Parser p; // Parser for input file
	Parser pTexture; // Parser for texture files

	// Get the filename and load it.
	string filename = argv[1];
	std::cout << "Input file: " << filename << std::endl;
	if (!p.loadFile(filename))
	{
		std::cerr << "**Error: \"" << filename << "\" not found." << std::endl;
		return 0;
	}

	// Initialize the scene variables and prepare for reading.
	Vec3 eye;
	Vec3 vdir;
	Vec3 udir;
	double fovv;
	double width, height;
	Color bkgcolor;
	double mtlcolor[12];

	Scene scene;
	Triangle::setScene(scene);

	string curParam;
	string textureName;
	bool validParam = true;
	double sphere[4];
	double ellipsoid[6];
	double vertex[3];
	double triangle[9];
	double light[7];
	double spotlight[10];
	double *vt;
	double vn[3];
	bool eyeSet = false;
	bool vdirSet = false;
	bool udirSet = false;
	bool fovvSet = false;
	bool imsizeSet = false;
	bool bkgcolorSet = false;
	bool mtlcolorSet = false;
	bool parallel = false;

	// Read the file for parameters until the end of the file.
	while (!p.endOfFile())
	{
		curParam = p.getKeyword();
		
		if (curParam == "eye")
		{
			// The view eye cannot have been already set.
			if (eyeSet)
			{
				std::cerr << "**Error: 'eye' parameter is already stored." << std::endl;
				return 0;
			}

			// FORMAT: 'eye <x> <y> <z>'
			eye.m_x = p.getNum(validParam);
			eye.m_y = p.getNum(validParam);
			eye.m_z = p.getNum(validParam);
			if (!validParam)
			{
				std::cerr << "**Error: Incorrect parameters for 'eye'." << std::endl;
				return 0;
			}

			eyeSet = true;
		}
		else if (curParam == "viewdir")
		{
			// The view direction cannot have been already set.
			if (vdirSet)
			{
				std::cerr << "**Error: 'viewdir' parameter is already stored." << std::endl;
				return 0;
			}

			// FORMAT: 'viewdir <dx> <dy> <dz>'
			vdir.m_x = p.getNum(validParam);
			vdir.m_y = p.getNum(validParam);
			vdir.m_z = p.getNum(validParam);
			if (!validParam)
			{
				std::cerr << "**Error: Incorrect parameters for 'viewdir'." << std::endl;
				return 0;
			}

			// View direction vector must not be zero-length.
			if (vdir.m_x == 0 && vdir.m_y == 0 && vdir.m_z == 0)
			{
				std::cerr << "**Error: 'viewdir' vector cannot be zero-length." << std::endl;
				return 0;
			}

			vdir.normalize();

			// View direction and up direction cannot be the same.
			if (udirSet && fabs(cross(vdir, udir).length()) < 1e-6)
			{
				std::cerr << "**Error: 'updir' and 'viewdir' vectors cannot be in the same direction." <<
					std::endl;
				return 0;
			}

			vdirSet = true;
		}
		else if (curParam == "updir")
		{
			// Up direction cannot have been already set.
			if (udirSet)
			{
				std::cerr << "**Error: 'updir' parameter is already stored." << std::endl;
				return 0;
			}

			// FORMAT: 'updir <dx> <dy> <dz>'
			udir.m_x = p.getNum(validParam);
			udir.m_y = p.getNum(validParam);
			udir.m_z = p.getNum(validParam);
			if (!validParam)
			{
				std::cerr << "**Error: Incorrect parameters for 'updir'." << std::endl;
				return 0;
			}

			// Up direction vector must not be zero-length.
			if (udir.m_x == 0 && udir.m_y == 0 && udir.m_z == 0)
			{
				std::cerr << "**Error: 'updir' vector cannot be zero-length." << std::endl;
				return 0;
			}

			udir.normalize();

			// View direction and up direction cannot be the same.
			if (vdirSet && fabs(cross(vdir, udir).length()) < 1e-6)
			{
				std::cerr << "**Error: 'updir' and 'viewdir' vectors cannot be in the same direction." <<
					std::endl;
				return 0;
			}

			udirSet = true;
		}
		else if (curParam == "fovv")
		{
			// Field of view cannot have already been set.
			if (fovvSet)
			{
				std::cerr << "**Error: 'fovv' parameter is already stored." << std::endl;
				return 0;
			}

			// FORMAT: 'fovv <degrees>'
			fovv = p.getNum(validParam);
			if (!validParam)
			{
				std::cerr << "**Error: Incorrect parameters for 'fovv'." << std::endl;
				return 0;
			}

			// Field of view must be between 180 and 0 degrees.
			if (fovv > 180 || fovv <= 0)
			{
				std::cerr << "**Error: 'fovv' must be less than 180 and greater than 0." <<
					std::endl;
				return 0;
			}

			fovvSet = true;
		}
		else if (curParam == "imsize")
		{
			// Width and height cannot have already been set.
			if (imsizeSet)
			{
				std::cout << "**Error: 'imsize' parameters is already stored." << std::endl;
				return 0;
			}

			// FORMAT: 'imsize <width-pixels> <height-pixels>'
			width = p.getNum(validParam);
			height = p.getNum(validParam);
			if (!validParam)
			{
				std::cerr << "**Error: Incorrect parameters for 'imsize'." << std::endl;
				return 0;
			}

			// Width and height must be integers.
			if (width != floor(width) || height != floor(height))
			{
				std::cerr << "**Error: 'imsize' parameters must be integers." << std::endl;
				return 0;
			}

			// Width and height must be greater than 0.
			if (width <= 0 || height <= 0)
			{
				std::cerr << "**Error: 'imsize' parameters must be greater than 0." << std::endl;
				return 0;
			}

			imsizeSet = true;
		}
		else if (curParam == "bkgcolor")
		{
			// Background color cannot have already been set.
			if (bkgcolorSet)
			{
				std::cerr << "**Error: 'bkgcolor' parameter is already stored." << std::endl;
				return 0;
			}

			// FORMAT: 'bkgcolor <red> <green> <blue>'
			bkgcolor.m_r = p.getNum(validParam);
			bkgcolor.m_g = p.getNum(validParam);
			bkgcolor.m_b = p.getNum(validParam);
			if (!validParam)
			{
				std::cerr << "**Error: Incorrect parameters for 'bkgcolor'." << std::endl;
				return 0;
			}

			// Background colors must be between 0 and 1.
			if (bkgcolor.outOfBounds())
			{
				std::cerr << "**Error: 'bkgcolor' parameters must be between 0 and 1." <<
					std::endl;
				return 0;
			}

			scene.background = bkgcolor;

			bkgcolorSet = true;
		}
		else if (curParam == "mtlcolor")
		{
			// FORMAT: 'mtlcolor <diffuse red> <diffuse green> <diffuse blue>
			// 			<specular red> <specular green> <specular blue>
			//			<ambient constant> <diffuse constant> <specular constant>
			//			<specular exponent>'
			mtlcolor[0] = p.getNum(validParam);
			mtlcolor[1] = p.getNum(validParam);
			mtlcolor[2] = p.getNum(validParam);
			mtlcolor[3] = p.getNum(validParam);
			mtlcolor[4] = p.getNum(validParam);
			mtlcolor[5] = p.getNum(validParam);
			mtlcolor[6] = p.getNum(validParam);
			mtlcolor[7] = p.getNum(validParam);
			mtlcolor[8] = p.getNum(validParam);
			mtlcolor[9] = p.getNum(validParam);
			mtlcolor[10] = p.getNum(validParam);
			mtlcolor[11] = p.getNum(validParam);
			if (!validParam)
			{
				std::cerr << "**Error: Incorrect parameters for 'mtlcolor'." << std::endl;
				return 0;
			}

			// Material colors must be between 0 and 1.
			for (int i = 0; i < 6; i++)
			{
				if (mtlcolor[i] < 0 || mtlcolor[i] > 1)
				{
					std::cerr << "**Error: 'mtlcolor' color parameters must be between 0 and 1." <<
						std::endl;
					return 0;
				}
			}
			
			// Material alpha must be between 0 and 1.
			if (mtlcolor[10] < 0 || mtlcolor[10] > 1)
			{
				std::cerr << "**Error: 'mtlcolor' alpha parameter must be between 0 and 1." << std::endl;
				return 0;
			}
			
			// Index of refraction must be greater than or equal to 1.
			if (mtlcolor[11] < 1)
			{
				std::cerr << "**Error: 'mtlcolor' eta parameter must be greater than or equal to 1." << std::endl;
				return 0;
			}
			
			scene.materials.push_back(new Material(mtlcolor));

			mtlcolorSet = true;
		}
		else if (curParam == "sphere")
		{
			// Material color must have been specified.
			if (!mtlcolorSet)
			{
				std::cerr << "**Error: 'mtlcolor' parameter must be set"
					<< "before 'sphere'." << std::endl;
				return 0;
			}
			
			// FORMAT: 'sphere <x> <y> <z> <radius>'
			sphere[0] = p.getNum(validParam);
			sphere[1] = p.getNum(validParam);
			sphere[2] = p.getNum(validParam);
			sphere[3] = p.getNum(validParam);
			if (!validParam)
			{
				std::cerr << "**Error: Incorrect parameters for 'sphere'." << std::endl;
				return 0;
			}

			// Sphere radius must be greater than 0.
			if (sphere[3] <= 0)
			{
				std::cerr << "**Error: 'sphere' radius must be greater than 0." << std::endl;
				return 0;
			}

			scene.objects.push_back(new Sphere(sphere, scene.materials.size() - 1,
				scene.textures.size() - 1));
		}
		else if (curParam == "ellipsoid")
		{
			// Material color must have been specified.
			if (!mtlcolorSet)
			{
				std::cerr << "**Error: 'mtlcolor' parameter must be set"
					<< "before 'ellipsoid'." << std::endl;
				return 0;
			}
			
			// FORMAT: 'ellipsoid <x> <y> <z> <x radius> <y radius> <z radius>'
			ellipsoid[0] = p.getNum(validParam);
			ellipsoid[1] = p.getNum(validParam);
			ellipsoid[2] = p.getNum(validParam);
			ellipsoid[3] = p.getNum(validParam);
			ellipsoid[4] = p.getNum(validParam);
			ellipsoid[5] = p.getNum(validParam);
			if (!validParam)
			{
				std::cerr << "**Error: Incorrect parameters for 'ellipsoid'." << std::endl;
				return 0;
			}

			// Ellipsoid radii must be greater than 0.
			if (ellipsoid[3] <= 0 && ellipsoid[4] <= 0 && ellipsoid[5] <= 0)
			{
				std::cerr << "**Error: 'ellipsoid' radii must be greater than 0." << std::endl;
				return 0;
			}

			scene.objects.push_back(new Ellipsoid(ellipsoid, scene.materials.size() - 1,
				scene.textures.size() - 1));
		}
		else if (curParam == "v")
		{	
			// FORMAT: 'v <x> <y> <z>'
			vertex[0] = p.getNum(validParam);
			vertex[1] = p.getNum(validParam);
			vertex[2] = p.getNum(validParam);
			if (!validParam)
			{
				std::cerr << "**Error: Incorrect parameters for 'v'." << std::endl;
				return 0;
			}
			
			scene.vertices.push_back(new Vec3(vertex));
		}
		else if (curParam == "f")
		{
			// Material color must have been specified.
			if (!mtlcolorSet)
			{
				std::cerr << "**Error: 'mtlcolor' parameter must be set"
					<< "before 'f'." << std::endl;
				return 0;
			}

			for (int i = 0; i < 9; i++)
			{
				triangle[i] = -1;
			}
			
			bool vt = false;
			bool vn = false;
			// FORMAT: 'f <v1> <v2> <v3>'
			//     or: 'f <v1>/<vt1> <v2>/<vt2> <v3>/<vt3>'
			//     or: 'f <v1>/<vt1>/<vn1> <v2>/<vt2>/<vn2> <v3>/<vt3>/<vn3>'
			triangle[0] = p.getNum(validParam) - 1;
			if (p.peekChar() == '/')
			{
				validParam = validParam && p.skipChar('/');
				if (p.peekChar() == '/')
				{
					vt = false;
					vn = true;
					validParam = validParam && p.skipChar('/');
					triangle[6] = p.getNum(validParam) - 1;
				}
				else
				{
					vt = true;
					triangle[3] = p.getNum(validParam) - 1;
					if (p.peekChar() == '/')
					{
						vn = true;
						validParam = validParam && p.skipChar('/');
						triangle[6] = p.getNum(validParam) - 1;
					}
					else
						vn = false;
				}
			}
			else
			{
				vt = false;
				vn = false;
			}

			triangle[1] = p.getNum(validParam) - 1;
			if (vt) {
				validParam = validParam && p.skipChar('/');
				triangle[4] = p.getNum(validParam) - 1;
			}
			if (vn) {
				if (!vt) validParam = validParam && p.skipChar('/');
				validParam = validParam && p.skipChar('/');
				triangle[7] = p.getNum(validParam) - 1;
			}

			triangle[2] = p.getNum(validParam) - 1;
			if (vt) {
				validParam = validParam && p.skipChar('/');
				triangle[5] = p.getNum(validParam) - 1;
			}
			if (vn) {
				if (!vt) validParam = validParam && p.skipChar('/');
				validParam = validParam && p.skipChar('/');
				triangle[8] = p.getNum(validParam) - 1;
			}
			
			if (!validParam)
			{
				std::cerr << "**Error: Incorrect formatting for 'f'." << std::endl;
				return 0;
			}

			// Vertex indices must be integers
			if (triangle[0] != floor(triangle[0]) ||
				triangle[1] != floor(triangle[1])||
				triangle[2] != floor(triangle[2]))
			{
				std::cerr << "**Error: 1st, 4th, and 7th parameters of 'f' must be integers."
					<< std::endl;
				return 0;
			}

			// Texture coordinate indices must be integers
			if (vt && (triangle[3] != floor(triangle[3]) ||
				triangle[4] != floor(triangle[4]) ||
				triangle[5] != floor(triangle[5])))
			{
				std::cerr << "**Error: 2nd, 5th, and 8th parameters of 'f' must be integers."
					<< std::endl;
				return 0;
			}

			// Vertices must exist
			if (triangle[0] < 0 || triangle[0] >= scene.vertices.size() ||
				triangle[1] < 0 || triangle[1] >= scene.vertices.size() ||
				triangle[2] < 0 || triangle[2] >= scene.vertices.size())
			{
				std::cerr << "**Error: 1st, 4th, and 7th parameters of 'f' must match 'v' entries and be positive."
					<< std::endl;
				return 0;
			}

			// Texture coordinates must exist
			if (vt && (triangle[3] < 0 || triangle[3] >= scene.textureCoords.size() ||
				triangle[4] < 0 || triangle[4] >= scene.textureCoords.size() ||
				triangle[5] < 0 || triangle[5] >= scene.textureCoords.size()))
			{
				std::cerr << "**Error: 2nd, 5th, and 8th parameters of 'f' must match 'vt' entries and be positive."
					<< std::endl;
				return 0;
			}

			// Vertex normals must exist
			if (vn && (triangle[6] < 0 || triangle[6] >= scene.vertexNorms.size() ||
				triangle[7] < 0 || triangle[7] >= scene.vertexNorms.size() ||
				triangle[8] < 0 || triangle[8] >= scene.vertexNorms.size()))
			{
				std::cerr << "**Error: 3rd, 6th, and 9th parameters of 'f' must match 'vn' entries and be positive."
					<< std::endl;
				return 0;
			}
			
			scene.objects.push_back(new Triangle(triangle, scene.materials.size() - 1,
				scene.textures.size() - 1));
		}
		else if (curParam == "vt")
		{
			// FORMAT: 'vt <u> <v>' for corresponding vertices
			vt = new double[2];
			vt[0] = p.getNum(validParam);
			vt[1] = p.getNum(validParam);
			if (!validParam)
			{
				std::cerr << "**Error: Incorrect parameters for 'vt'." << std::endl;
				return 0;
			}

			// Must be between 0 and 1
			if (vt[0] < 0 || vt[0] > 1 || vt[1] < 0 || vt[1] > 1)
			{
				std::cerr << "**Error: 'vt' parameters must be between 0 and 1." << std::endl;
				return 0;
			}

			scene.textureCoords.push_back(vt);
		}
		else if (curParam == "vn")
		{
			// FORMAT: 'vt <u> <v>' for corresponding vertices
			vn[0] = p.getNum(validParam);
			vn[1] = p.getNum(validParam);
			vn[2] = p.getNum(validParam);
			if (!validParam)
			{
				std::cerr << "**Error: Incorrect parameters for 'vn'." << std::endl;
				return 0;
			}

			// Must be between 0 and 1
			if (vn[0] == 0 && vn[1] == 0 && vn[2] == 0)
			{
				std::cerr << "**Error: 'vn' parameters cannot yield zero vector." << std::endl;
				return 0;
			}

			scene.vertexNorms.push_back(new Vec3(vn));
		}
		else if (curParam == "light")
		{	
			// FORMAT: 'light <x> <y> <z> <w> <r> <g> <b>'
			light[0] = p.getNum(validParam);
			light[1] = p.getNum(validParam);
			light[2] = p.getNum(validParam);
			light[3] = p.getNum(validParam);
			light[4] = p.getNum(validParam);
			light[5] = p.getNum(validParam);
			light[6] = p.getNum(validParam);
			if (!validParam)
			{
				std::cerr << "**Error: Incorrect parameters for 'light'." << std::endl;
				return 0;
			}
			
			// w needs to be 0 or 1.
			if (light[3] != 0.0 && light[3] != 1.0)
			{
				std::cerr << "**Error: w parameter for 'light' needs to be 0 or 1." << std::endl;
				return 0;
			}
			
			// Directional light vector cannot be zero-length.
			if (light[3] == 0.0 && light[0] == 0.0 && light[1] == 0.0 && light[2] == 0.0)
			{
				std::cerr << "**Error: 'light' directional vector cannot be zero-length." << std::endl;
				return 0;
			}
			
			// Light colors must be between 0 and 1.
			if (light[4] < 0 || light[4] > 1 ||
				light[5] < 0 || light[5] > 1 ||
				light[6] < 0 || light[6] > 1)
			{
				std::cerr << "**Error: 'light' color parameters must be between 0 and 1." <<
					std::endl;
				return 0;
			}
			
			if (light[3] == 0)
				scene.lights.push_back(new DirectionLight(light));
			else
				scene.lights.push_back(new PointLight(light));
		}
		else if (curParam == "spotlight")
		{
			// FORMAT: 'spotlight <x> <y> <z> <dx> <dy> <dz> <theta> <r> <g> <b>'
			spotlight[0] = p.getNum(validParam);
			spotlight[1] = p.getNum(validParam);
			spotlight[2] = p.getNum(validParam);
			spotlight[3] = p.getNum(validParam);
			spotlight[4] = p.getNum(validParam);
			spotlight[5] = p.getNum(validParam);
			spotlight[6] = p.getNum(validParam);
			spotlight[7] = p.getNum(validParam);
			spotlight[8] = p.getNum(validParam);
			spotlight[9] = p.getNum(validParam);
			if (!validParam)
			{
				std::cerr << "**Error: Incorrect parameters for 'spotlight'." << std::endl;
				return 0;
			}
			
			// theta must be between 0 and 180
			if (spotlight[6] < 0 && spotlight[6] > 180)
			{
				std::cerr << "**Error: theta parameter for 'spotlight' must be between 0 and 180";
			}
			
			// Spotlight direction vector cannot be zero-length.
			if (spotlight[3] == 0.0 && spotlight[4] == 0.0 && spotlight[5] == 0.0)
			{
				std::cerr << "**Error: 'spotlight' directional vector cannot be zero-length." << std::endl;
				return 0;
			}
			
			// Spotlight colors must be between 0 and 1.
			if (spotlight[7] < 0 || spotlight[7] > 1 ||
				spotlight[8] < 0 || spotlight[8] > 1 ||
				spotlight[9] < 0 || spotlight[9] > 1)
			{
				std::cerr << "**Error: 'spotlight' color parameters must be between 0 and 1." <<
					std::endl;
				return 0;
			}
			
			scene.lights.push_back(new Spotlight(spotlight));
		}
		else if (curParam == "texture")
		{
			// FORMAT: texture <texturefilename.ppm>
			textureName = p.getKeyword();

			// Put the file name in context with the input file
			textureName = Parser::swapFileName(filename, textureName);

			std::cout << "Loading texture: " << textureName << std::endl;

			if (!pTexture.loadFile(textureName))
			{
				std::cerr << "**Error: \"" << textureName << "\" not found. (texture)" << std::endl;
				return 0;
			}

			double texWidth, texHeight, texRange, texPixel;
			int texRGB, texPixelCount;
			int *texPixelArray;

			// Check for "P3" at start of file
			if (pTexture.getKeyword() != "P3")
			{
				std::cout << "**Error: " << textureName << " missing \"P3\""
					<< " at top of file." << std::endl;
				return 0;
			}

			// Check for image dimensions next
			texWidth = pTexture.getNum(validParam);
			texHeight = pTexture.getNum(validParam);
			if (!validParam || floor(texWidth) != texWidth)
			{
				std::cout << "**Error: " << textureName << " has non-integer"
					<< " width." << std::endl;
				return 0;
			}
			if (!validParam || floor(texHeight) != texHeight)
			{
				std::cout << "**Error: " << textureName << " has non-integer"
					<< " height." << std::endl;
				return 0;
			}

			// Check for color range next
			texRange = pTexture.getNum(validParam);
			if (!validParam || floor(texRange) != texRange)
			{
				std::cout << "**Error: " << textureName << " has non-integer"
					<< " range." << std::endl;
				return 0;
			}

			texPixelArray = new int[(int)(texWidth*texHeight * 3)]; // integer array to store pixel values
			texRGB = 0; // RGB modulus counter
			texPixelCount = 0; // pixel counter

			// Read out all pixel data into integer array
			while (!pTexture.endOfFile() && texPixelCount < texWidth*texHeight)
			{
				texPixel = pTexture.getNum(validParam);
				if (!validParam || floor(texPixel) != texPixel)
				{
					std::cout << "**Error: " << textureName << " has non-integer"
						<< " pixel value." << std::endl;
					delete texPixelArray;
					return 0;
				}

				texPixelArray[texPixelCount * 3 + texRGB] = (int)texPixel;
				texPixelCount += (texRGB + 1) / 3;
				texRGB = (texRGB + 1) % 3;
			}

			if (texPixelCount != texWidth*texHeight)
			{
				std::cout << "**Error: " << textureName << " does not have enough"
					<< " pixel data." << std::endl;
				delete texPixelArray;
				return 0;
			}

			std::cout << "Finished loading " << textureName << std::endl;

			scene.textures.push_back(new Texture(texWidth, texHeight, texRange, texPixelArray));
		}
		else if (curParam == "parallel")
		{
			parallel = true;
		}
		else if (curParam == "refraction")
		{
			// FORMAT: 'refraction <eta>'
			scene.eta = p.getNum(validParam);
			if (!validParam)
			{
				std::cerr << "**Error: Incorrect parameter for 'refraction'." << std::endl;
				return 0;
			}

			// Must be greater than or equal to 1
			if (scene.eta < 1)
			{
				std::cerr << "**Error: 'refraction' parameter must be greater than or equal to 1." << std::endl;
				return 0;
			}
		}
		else if (curParam[0] == 0) { /* Avoids stringstream.getline() problem. */ }
		else
		{
			std::cerr << "**Error: Unknown token found: " << curParam << std::endl;
			return 0;
		}
	}

	// Check that all required parameters have been filled.
	if (!(eyeSet && vdirSet && udirSet && fovvSet && imsizeSet &&
		bkgcolorSet))
	{
		string missParams = "";
		
		if (!eyeSet) missParams += "eye <x> <y> <z>\n";
		if (!vdirSet) missParams += "viewdir <dx> <dy> <dz>\n";
		if (!udirSet) missParams += "updir <dx> <dy> <dz>\n";
		if (!fovvSet) missParams += "fovv <degrees>\n";
		if (!imsizeSet) missParams += "imsize <width> <height>\n";
		if (!bkgcolorSet) missParams += "bkgcolor <red> <green> <blue>\n";
		
		std::cerr << "**Error: The following parameters are missing:\n" << missParams;
		
		return 0;
	}

	// Define the array based on the width and height.
	int *image = new int[3 * (int)width * (int)height];

	// Define the viewing window.
	Vec3 u = cross(vdir, udir);
	u.normalize();
	Vec3 v = cross(u, vdir);
	v.normalize();

	double d;
	if (parallel)
		d = 0.0;
	else
		d = 1.0;
	double w, h; // width and height in 3D world coordinates
	h = 2 * tan((fovv * PI / 180) / 2); // need to convert degrees to radians
	w = h * (width / height);

	Vec3 ul = eye + vdir*d + v*(h/2) - u*(w/2); // upper left corner of window
	Vec3 ur = eye + vdir*d + v*(h/2) + u*(w/2); // upper right corner of window
	Vec3 ll = eye + vdir*d - v*(h/2) - u*(w/2); // lower left corner of window
	Vec3 lr = eye + vdir*d - v*(h/2) + u*(w/2); // lower right corner of window

	// Define a 1-1 mapping between pixels and points on the viewing window.
	// Pixels are positioned by d_horz*i + d_vert*j + ul if i == row and j == column.
	int pixw = (int)width;
	int pixh = (int)height;

	Vec3 d_horz = (ur - ul) / (pixw - 1);
	Vec3 d_vert = (ll - ul) / (pixh - 1);
	
	const int total_pixels = height*width;

	// Initialize shared ray recursion stack
	vector<int> *recurseStack = new vector<int>;
	
	// Iterate over all the pixels and calculate their colors
	for (int it = 0; it < total_pixels; it++)
	{
		int i = it/(int)width;
		int j = it%(int)width;

		Vec3 ray; // ray vector to test with
		Vec3 origin; // allows for different perspectives
		Color pixelColor;
		RayPayload payload;

		if (parallel) // the ray projects in the viewing direction out of pixel [i,j] on the
					  // viewing window
		{
			origin = d_horz*j + d_vert*i + ul;
			ray = vdir;
		}
		else // the ray projects toward the center of pixel [i,j] on the viewing window.
		{
			origin = eye;
			ray = d_horz*j + d_vert*i + ul - origin;
			ray.normalize(); // normalizing allows us to remove it from some equations
		}
		
		payload.inObjects = recurseStack;
		payload.eta = scene.eta;

		traceRay(ray, origin, scene, payload);

		// Color the pixel using intersected objects and background.
		if (payload.object > -1)
		{
			Vec3 intersectionPoint = origin + ray*payload.distance;
			pixelColor = shadeRay(ray, intersectionPoint, scene, payload);
		}
		else
		{
			pixelColor = bkgcolor;
		}
		image[(i*pixw + j) * 3] = floor(pixelColor.m_r * 255);
		image[(i*pixw + j) * 3 + 1] = floor(pixelColor.m_g * 255);
		image[(i*pixw + j) * 3 + 2] = floor(pixelColor.m_b * 255);
		
		recurseStack->clear();
	}
	
	delete recurseStack;

	// Create the output file.
	int extension = filename.find_last_of('.');
	string newFilename = filename.substr(0, extension);
	ofstream outfile;

	newFilename = newFilename +  ".ppm";
	outfile.open(newFilename);

	outfile << "P3" << std::endl << width << " " << height
		<< std::endl << 255 << std::endl;
	for (int i = 0; i < 3 * width * height; i += 3)
	{
		outfile << image[i] << " " << image[i + 1] << " " << image[i + 2]
			<< std::endl;
	}

	outfile.close();
	std::cout << "Output file: " << newFilename << std::endl;

	// Free the memory used by all arrays and vectors.
	delete image;
	for (int i = 0; i < scene.objects.size(); i++)
	{
		delete scene.objects[i];
	}
	scene.objects.clear();
	
	for (int i = 0; i < scene.materials.size(); i++)
	{
		delete scene.materials[i];
	}
	scene.materials.clear();
	
	for (int i = 0; i < scene.lights.size(); i++)
	{
		delete scene.lights[i];
	}
	scene.lights.clear();
	
	for (int i = 0; i < scene.vertices.size(); i++)
	{
		delete scene.vertices[i];
	}
	scene.vertices.clear();

	for (int i = 0; i < scene.textures.size(); i++)
	{
		delete scene.textures[i];
	}
	scene.textures.clear();

	for (int i = 0; i < scene.textureCoords.size(); i++)
	{
		delete scene.textureCoords[i];
	}
	scene.textureCoords.clear();
	
	for (int i = 0; i < scene.vertexNorms.size(); i++)
	{
		delete scene.vertexNorms[i];
	}
	scene.vertexNorms.clear();

	return 0;
}
