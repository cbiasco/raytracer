#pragma once
//
// Color.h
//
// Code by Caleb Biasco (biasc007) for Assignment-1A of CSCI5607

/*
 * The Color class holds RGB values and pertinent operations on them.
 */

class Color
{
public:
	Color();
	Color(double r, double g, double b);
	virtual ~Color();

	// Returns true if any color component is less than 0 or greater
	// than 1.
	bool outOfBounds();

	// Accessors/Mutators
	double getR();
	void setR(double r);
	double getG();
	void setG(double g);
	double getB();
	void setB(double b);

private:
	double m_r; // red
	double m_b; // blue
	double m_g; // green
};

