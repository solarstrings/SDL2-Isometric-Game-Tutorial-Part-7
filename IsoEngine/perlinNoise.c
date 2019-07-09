/*
    C Code for Perlin Noise was found here:
    https://github.com/czinn/perlin

    Good articles about perlin noise (old - but good!, had to use wayback machine to find them)

    Hugo Elias's - Perlin Noise
    http://webstaff.itn.liu.se/~stegu/TNM022-2005/perlinnoiselinks/perlin-noise-math-faq.html

    Matt Zucker - The Perlin noise math FAQ
    https://web.archive.org/web/20101124044214/http://webstaff.itn.liu.se/~stegu/TNM022-2005/perlinnoiselinks/perlin-noise-math-faq.html

    Libnoise
    http://libnoise.sourceforge.net/glossary/index.html
*/

#include <stdio.h>
#include <math.h>

double rawnoise(int n) {
    n = (n << 13) ^ n;
    return (1.0 - ((n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);
}

double noise1d(int x, int octave, int seed) {
    return rawnoise(x * 1619 + octave * 3463 + seed * 13397);
}

double noise2d(int x, int y, int octave, int seed) {
    return rawnoise(x * 1619 + y * 31337 + octave * 3463 + seed * 13397);
}

double noise3d(int x, int y, int z, int octave, int seed) {
    return rawnoise(x * 1919 + y * 31337 + z * 7669 + octave * 3463 + seed * 13397);
}

double interpolate(double a, double b, double x) {
    double f = (1 - cos(x * 3.141593)) * 0.5;

    return a * (1 - f) + b * f;
}

double smooth1d(double x, int octave, int seed) {
    int intx = (int)x;
    double fracx = x - intx;

    double v1 = noise1d(intx, octave, seed);
    double v2 = noise1d(intx + 1, octave, seed);

    return interpolate(v1, v2, fracx);
}

double smooth2d(double x, double y, int octave, int seed) {
    int intx = (int)x;
    double fracx = x - intx;
    int inty = (int)y;
    double fracy = y - inty;

    double v1 = noise2d(intx, inty, octave, seed);
    double v2 = noise2d(intx + 1, inty, octave, seed);
    double v3 = noise2d(intx, inty + 1, octave, seed);
    double v4 = noise2d(intx + 1, inty + 1, octave, seed);

    double i1 = interpolate(v1, v2, fracx);
    double i2 = interpolate(v3, v4, fracx);

    return interpolate(i1, i2, fracy);
}

double smooth3d(double x, double y, double z, int octave, int seed) {
    int intx = (int)x;
    double fracx = x - intx;
    int inty = (int)y;
    double fracy = y - inty;
    int intz = (int)z;
    double fracz = z - intz;


    double v1 = noise3d(intx, inty, intz, octave, seed);
    double v2 = noise3d(intx + 1, inty, intz, octave, seed);
    double v3 = noise3d(intx, inty + 1, intz, octave, seed);
    double v4 = noise3d(intx + 1, inty + 1, intz, octave, seed);
    double v5 = noise3d(intx, inty, intz + 1, octave, seed);
    double v6 = noise3d(intx + 1, inty, intz + 1, octave, seed);
    double v7 = noise3d(intx, inty + 1, intz + 1, octave, seed);
    double v8 = noise3d(intx + 1, inty + 1, intz + 1, octave, seed);

    double i1 = interpolate(v1, v2, fracx);
    double i2 = interpolate(v3, v4, fracx);
    double i3 = interpolate(v5, v6, fracx);
    double i4 = interpolate(v7, v8, fracx);

    double j1 = interpolate(i1, i2, fracy);
    double j2 = interpolate(i3, i4, fracy);

    return interpolate(j1, j2, fracz);
}

double pnoise1d(double x, double persistence, int octaves, int seed) {
   double total = 0.0;
   double frequency = 1.0;
   double amplitude = 1.0;
   int i = 0;

   for(i = 0; i < octaves; i++) {
       total += smooth1d(x * frequency, i, seed) * amplitude;
       frequency /= 2;
       amplitude *= persistence;
   }

   return total;
}

double pnoise2d(double x, double y, double persistence, int octaves, int seed) {
   double total = 0.0;
   double frequency = 1.0;
   double amplitude = 1.0;
   int i = 0;

   for(i = 0; i < octaves; i++) {
       total += smooth2d(x * frequency, y * frequency, i, seed) * amplitude;
       frequency /= 2;
       amplitude *= persistence;
   }

   return total;
}

double pnoise3d(double x, double y, double z, double persistence, int octaves, int seed) {
   double total = 0.0;
   double frequency = 1.0;
   double amplitude = 1.0;
   int i = 0;

   for(i = 0; i < octaves; i++) {
       total += smooth3d(x * frequency, y * frequency, z * frequency, i, seed) * amplitude;
       frequency /= 2;
       amplitude *= persistence;
   }

   return total;
}
