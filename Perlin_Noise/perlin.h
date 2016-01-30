#ifndef _PERLIN
#define _PERLIN

#include <math.h>

class Perlin {
	double values[256][256];

	void perlin();

	double generatePerlin(double x, double y);
	double generateInterpolateNoise(double x, double y);
	double smoothing(double x, double y);
	double interpolate(double a, double b, double x);
	double noise(int x, int y);

public:
	Perlin() { perlin(); }

	double const getValue(int x, int y) const { return (x >= 0 && y >= 0 && x < 256 && y < 256) ? values[x][y] : 0.0f; }
};

#endif // _PERLIN