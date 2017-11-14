#pragma once
//
// Color.h
//
// Code by Caleb Biasco (biasc007) for Assignment-1 of CSCI5607

/*
 * The Color class holds red, green, and blue values and checks
 * that they fit within certain bounds.
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
	
	// Clamps the colors to 1 or 0
	Color clampColor();
	
	// Component-wise operations
	friend Color operator+ (const Color& a, const Color& b);
	friend Color operator- (const Color& a, const Color& b);
	friend Color operator* (const Color& a, double d);
	friend Color operator* (double d, const Color& b);
	friend Color operator* (const Color& a, const Color& b);
	friend Color operator/ (const Color& a, double d);
	friend Color operator/ (double d, const Color& b);
	friend Color operator/ (const Color& a, const Color& b);

	double m_r; // red
	double m_b; // blue
	double m_g; // green
	
private:
	static inline double clamp(double d, double min, double max)
	{
		return (d < min) ? min : ((d > max) ? max : d);
	}
	
};

