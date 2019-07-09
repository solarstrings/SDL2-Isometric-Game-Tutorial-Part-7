#ifndef PERLIN_HEADER
#define PERLIN_HEADER

double rawnoise(int n);

double noise1d(int x, int octave, int seed);

double noise2d(int x, int y, int octave, int seed);

double noise3d(int x, int y, int z, int octave, int seed);

double interpolate(double a, double b, double x);

double smooth1d(double x, int octave, int seed);

double smooth2d(double x, double y, int octave, int seed);

double smooth3d(double x, double y, double z, int octave, int seed);

double pnoise1d(double x, double persistence, int octaves, int seed);

double pnoise2d(double x, double y, double persistence, int octaves, int seed);

double pnoise3d(double x, double y, double z, double persistence, int octaves, int seed);

#endif
