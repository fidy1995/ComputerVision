#include "perlin.h"
#include <iostream>
#include <time.h>

using namespace std;

void Perlin::perlin(){
	for (int x = 0; x < 256; x++) {
		for (int y = 0; y < 256; y++) {
			values[x][y] = 0.0;
		}
	}
	srand((unsigned)time(NULL));
	double step = 1.0f / 32.0f;
	for (int x = 0; x < 256; x++) {
		for (int y = 0; y < 256; y++) {
			values[x][y] = generatePerlin(step * x, step * y);
		}
	}
}

double Perlin::generatePerlin(double x, double y) {
	double ret = 0.0f;
	double persist = 0.7f;
	int iterTime = 7;

	for (int i = 0; i < iterTime; i++) {
		double freq = pow(2.0, i);
		double amplitude = pow(persist, i);
		ret += generateInterpolateNoise(freq * x, freq * y) * amplitude;
	}
	return ret;
}

double Perlin::generateInterpolateNoise(double x, double y) {
	int xint = x * 32;
	int yint = y * 32;
	x = x - xint;
	y = y - yint;

	double v1 = smoothing(xint, yint);
	double v2 = smoothing(xint + 1, yint);
	double v3 = smoothing(xint, yint + 1);
	double v4 = smoothing(xint + 1, yint + 1);

	double i1 = interpolate(v1, v2, x);
	double i2 = interpolate(v3, v4, x);

	return interpolate(i1, i2, y);
}

double Perlin::smoothing(double x, double y) {
	double corners = (noise(x - 1, y - 1) + noise(x - 1, y + 1) + noise(x + 1, y - 1) + noise(x + 1, y + 1)) / 16;
	double sides = (noise(x - 1, y) + noise(x, y - 1) + noise(x + 1, y) + noise(x, y + 1)) / 8;
	double center = noise(x, y) / 4;
	return corners + sides + center;
}

double Perlin::interpolate(double a, double b, double x) {
	double ft = x * 3.1415926;
	double f = (1 - cos(ft)) * 0.5f;
	return a * (1 - f) + b * f;
}

double Perlin::noise(int x, int y) {
	int n;
	n = x + y * 17;
	n = (n << 13) ^ n;
	return (1.0f - (double)((n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0f);

}