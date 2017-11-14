//
// main.cpp
//
// Code by Caleb Biasco (biasc007) for Assignment-1A of CSCI5607

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
#include "DirectionLight.h"
#include "PointLight.h"
#include "Spotlight.h"

using std::string;
using std::vector;
using std::ofstream;
using std::min;

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

	Parser p;
	
	const double PI = 4*atan(1.0);

	// Get the filename and load it.
	string filename = argv[1];
	std::cout << "Input file: " << filename << std::endl;
	if (!p.loadFile(filename))
	{
		std::cerr << "**Error: \"" << filename << "\" not found." << std::endl;
		return 0;
	}

	// Initialize the scene variables and prepare for reading.
	Vec3 origin;
	Vec3 vdir;
	Vec3 udir;
	double fovv;
	double width, height;
	Color bkgcolor;
	Material mtlcolor;
	Color lightcolor;

	vector<Object*> objects;
	vector<Light*> lights;

	string curParam;
	bool validParam = true;
	double sphere[4];
	double ellipsoid[6];
	double light[4];
	double spotlight[7];
	bool originSet = false;
	bool vdirSet = false;
	bool udirSet = false;
	bool fovvSet = false;
	bool imsizeSet = false;
	bool bkgcolorSet = false;
	bool mtlcolorSet = false;

	// Read the file for parameters until the end of the file.
	while (!p.endOfFile())
	{
		curParam = p.getKeyword();
		
		if (curParam == "eye")
		{
			// The view origin cannot have been already set.
			if (originSet)
			{
				std::cerr << "**Error: 'eye' parameter is already stored." << std::endl;
				return 0;
			}

			// FORMAT: 'eye <x> <y> <z>'
			origin.m_x = p.getNum(validParam);
			origin.m_y = p.getNum(validParam);
			origin.m_z = p.getNum(validParam);
			if (!validParam)
			{
				std::cerr << "**Error: Incorrect parameters for 'eye'." << std::endl;
				return 0;
			}

			originSet = true;
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
				std::cerr << "**Error: 'eye' and 'viewdir' vectors cannot be in the same " <<
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
				std::cerr << "**Error: 'eye' and 'viewdir' vectors cannot be in the same " <<
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

			bkgcolorSet = true;
		}
		else if (curParam == "mtlcolor")
		{
			// FORMAT: 'mtlcolor <diffuse red> <diffuse green> <diffuse blue>
			// 			<specular red> <specular green> <specular blue>
			//			<ambient constant> <diffuse constant> <specular constant>
			//			<specular exponent>'
			mtlcolor.m_dr = p.getNum(validParam);
			mtlcolor.m_dg = p.getNum(validParam);
			mtlcolor.m_db = p.getNum(validParam);
			mtlcolor.m_sr = p.getNum(validParam);
			mtlcolor.m_sg = p.getNum(validParam);
			mtlcolor.m_sb = p.getNum(validParam);
			mtlcolor.m_ka = p.getNum(validParam);
			mtlcolor.m_kd = p.getNum(validParam);
			mtlcolor.m_ks = p.getNum(validParam);
			mtlcolor.m_n = p.getNum(validParam);
			if (!validParam)
			{
				std::cerr << "**Error: Incorrect parameters for 'mtlcolor'." << std::endl;
				return 0;
			}

			// Material colors must be between 0 and 1.
			if (mtlcolor.outOfBounds())
			{
				std::cerr << "**Error: 'mtlcolor' color parameters must be between 0 and 1." <<
					std::endl;
				return 0;
			}

			mtlcolorSet = true;
		}
		else if (curParam == "sphere")
		{
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

			// Material color must have been specified.
			if (!mtlcolorSet)
			{
				std::cerr << "**Error: 'mtlcolor' parameter must be set before 'sphere'." <<
					std::endl;
				return 0;
			}

			objects.push_back(new Sphere(sphere[0], sphere[1], sphere[2], sphere[3], mtlcolor));
		}
		else if (curParam == "ellipsoid")
		{
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

			// Material color must have been specified.
			if (!mtlcolorSet)
			{
				std::cerr << "**Error: 'mtlcolor' parameter must be set before 'ellipsoid'." <<
					std::endl;
				return 0;
			}

			objects.push_back(new Ellipsoid(ellipsoid[0], ellipsoid[1], ellipsoid[2],
				ellipsoid[3], ellipsoid[4], ellipsoid[5], mtlcolor));
		}
		else if (curParam == "light")
		{	
			// FORMAT: 'light <x> <y> <z> <w> <r> <g> <b>'
			light[0] = p.getNum(validParam);
			light[1] = p.getNum(validParam);
			light[2] = p.getNum(validParam);
			light[3] = p.getNum(validParam);
			lightcolor.m_r = p.getNum(validParam);
			lightcolor.m_g = p.getNum(validParam);
			lightcolor.m_b = p.getNum(validParam);
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
			if (lightcolor.outOfBounds())
			{
				std::cerr << "**Error: 'light' color parameters must be between 0 and 1." <<
					std::endl;
				return 0;
			}
			
			if (light[3] == 0)
				lights.push_back(new DirectionLight(light[0], light[1], light[2], lightcolor));
			else
				lights.push_back(new PointLight(light[0], light[1], light[2], lightcolor));
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
			lightcolor.m_r = p.getNum(validParam);
			lightcolor.m_g = p.getNum(validParam);
			lightcolor.m_b = p.getNum(validParam);
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
			
			// Light colors must be between 0 and 1.
			if (lightcolor.outOfBounds())
			{
				std::cerr << "**Error: 'spotlight' color parameters must be between 0 and 1." <<
					std::endl;
				return 0;
			}
			
			lights.push_back(new Spotlight(spotlight[0], spotlight[1], spotlight[2],
			spotlight[3], spotlight[4], spotlight[5], spotlight[6], lightcolor));
		}
		else if (curParam[0] == 0) { /* Avoids stringstream.getline() problem. */ }
		else
		{
			std::cerr << "**Error: Unknown token found: " << curParam << std::endl;
			return 0;
		}
	}

	// Check that all required parameters have been filled.
	if (!(originSet && vdirSet && udirSet && fovvSet && imsizeSet &&
		bkgcolorSet))
	{
		string missParams = "";
		
		if (!originSet) missParams += "eye <x> <y> <z>\n";
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

	double d = 1.0; // arbitrary distance from viewing window
	double w, h; // width and height in 3D world coordinates
	h = 2 * d * tan((fovv * PI / 180) / 2); // need to convert degrees to radians
	w = h * (width / height);

	Vec3 ul = origin + vdir*d + v*(h/2) - u*(w/2); // upper left corner of window
	Vec3 ur = origin + vdir*d + v*(h/2) + u*(w/2); // upper right corner of window
	Vec3 ll = origin + vdir*d - v*(h/2) - u*(w/2); // lower left corner of window
	Vec3 lr = origin + vdir*d - v*(h/2) + u*(w/2); // lower right corner of window

	// Define a 1-1 mapping between pixels and points on the viewing window.
	// Pixels are positioned by d_horz*i + d_vert*j + ul if i == row and j == column.
	int pixw = (int)width;
	int pixh = (int)height;

	Vec3 d_horz = (ur - ul) / (pixw - 1);
	Vec3 d_vert = (ll - ul) / (pixh - 1);

	// Iterate over all the pixels and calculate their colors
	Vec3 ray; // ray vector to test with
	double distToObject;
	int closestObject;
	double closestDist;
	Color pixelColor;

	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			closestObject = -1;

			// The ray projects toward the center of pixel [i,j] on the viewing window.
			ray = d_horz*j + d_vert*i + ul - origin;
			ray.normalize(); // normalizing allows us to remove it from some equations

			for (int k = 0; k < objects.size(); k++)
			{
				distToObject = objects[k]->intersection(ray, origin);

				// If the distance is less than 0, the object is either not intersected
				// or it is behind the origin.
				if (distToObject >= 0)
				{
					if (closestObject == -1)
					{
						closestObject = k;
						closestDist = distToObject;
					}
					else if (distToObject < closestDist)
					{
						closestObject = k;
						closestDist = distToObject;
					}
				}
			}

			// Color the pixel using intersected objects and background.
			if (closestObject > -1)
			{
				pixelColor = objects[closestObject]->shade(ray, origin, closestDist, lights, objects, closestObject);
			}
			else
			{
				pixelColor = bkgcolor;
			}
			image[(i*pixw + j) * 3] = floor(pixelColor.m_r * 255);
			image[(i*pixw + j) * 3 + 1] = floor(pixelColor.m_g * 255);
			image[(i*pixw + j) * 3 + 2] = floor(pixelColor.m_b * 255);
		}
	}

	// Create the output file.
	string newFilename = filename;
	ofstream outfile;

	int extension = newFilename.find_last_of('.');
	newFilename.replace(filename.find_last_of('.'), 5, ".ppm\0");
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

	// Free the memory used by the image array and object vector.
	delete image;
	for (int i = 0; i < objects.size(); i++)
	{
		delete objects[i];
	}
	objects.clear();
	
	for (int i = 0; i < lights.size(); i++)
	{
		delete lights[i];
	}
	lights.clear();

	return 0;
}