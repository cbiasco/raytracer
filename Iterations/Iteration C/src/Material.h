#pragma once
//
// Material.h
//
// Code by Caleb Biasco (biasc007) for Assignment-1 of CSCI5607

/*
 * The Material class holds color values that correspond specifically to
 * materials for objects in a scene.
 */
 
 #include "Color.h"

class Material
{
public:
	Material();
	Material(double dr, double dg, double db, double sr, double sg, double sb,
			double ka, double kd, double ks, double n);
	Material(double mtl[10]);
	virtual ~Material();
	
	// Returns just the ambient color
	Color getAmbient();
	
	// Returns the ambience of a given color
	Color getAmbient(Color c);
	
	// Returns just the diffuse color
	Color getDiffuse();
	
	// Returns just the specular color
	Color getSpecular();

	// Returns true if any color component is less than 0 or greater
	// than 1.
	bool outOfBounds();

	double m_dr; // diffuse red
	double m_db; // diffuse blue
	double m_dg; // diffuse green
	double m_sr; // specular red
	double m_sb; // specular blue
	double m_sg; // specular green
	double m_ka; // ambient constant
	double m_kd; // diffuse constant
	double m_ks; // specular constant
	double m_n; // specular exponent
};

