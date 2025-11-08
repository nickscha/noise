/* noise.h - v0.3 - public domain data structures - nickscha 2025

A C89 standard compliant, single header, nostdlib (no C Standard Library) Noise Generation (NOISE).

This Test class defines cases to verify that we don't break the excepted behaviours in the future upon changes.

LICENSE

  Placed in the public domain and also MIT licensed.
  See end of file for detailed license information.

*/
#include "../noise.h"     /* Noise Generation */
#include "../deps/test.h" /* Simple Testing framework    */

#include <stdio.h>
#include <stdlib.h>

#define WIDTH 512
#define HEIGHT 512
static float heightmap[WIDTH * HEIGHT];

unsigned int img_size = (unsigned int)(WIDTH * HEIGHT * 3);
unsigned char *img;

static void noise_export_ppm(char *filename, float *data, int width, int height)
{
  FILE *f = fopen(filename, "wb");
  int j, i;

  unsigned char *p;

  if (!img)
  {
    img = (unsigned char *)malloc(img_size);
  }

  if (!f)
  {
    fprintf(stderr, "Failed to open file %s\n", filename);
    return;
  }

  fprintf(f, "P6\n%d %d\n255\n", width, height);

  if (!img)
  {
    fprintf(stderr, "Failed to allocate image buffer\n");
    fclose(f);
    return;
  }

  p = img;

  for (j = 0; j < height; ++j)
  {
    for (i = 0; i < width; ++i)
    {
      float v = data[j * width + i];
      unsigned char c;

      /* clamp to [0,1] */
      if (v < 0.0f)
      {
        v = 0.0f;
      }
      if (v > 1.0f)
      {
        v = 1.0f;
      }

      c = (unsigned char)(v * 255.0f);

      /* grayscale: R = G = B = c */
      *p++ = c;
      *p++ = c;
      *p++ = c;
    }
  }

  fwrite(img, 1, img_size, f);
  fclose(f);

  printf("[noise] %s exported.\n", filename);
}

static void noise_normalize_heightmap(void)
{
  int x, y;
  float min = heightmap[0], max = heightmap[0];
  float range;

  for (y = 0; y < HEIGHT; ++y)
  {
    for (x = 0; x < WIDTH; ++x)
    {
      float h = heightmap[y * WIDTH + x];

      if (h < min)
      {
        min = h;
      }

      if (h > max)
      {
        max = h;
      }
    }
  }

  range = max - min;

  if (range < 1e-6f)
  {
    range = 1.0f; /* avoid division by zero */
  }

  for (y = 0; y < HEIGHT; ++y)
  {
    for (x = 0; x < WIDTH; ++x)
    {
      heightmap[y * WIDTH + x] = (heightmap[y * WIDTH + x] - min) / range;
    }
  }
}

typedef float (*test_noise_function)(int x, int y);

void noise_test_run(char *filename, test_noise_function function)
{
  int x, y;

  for (y = 0; y < HEIGHT; ++y)
  {
    for (x = 0; x < WIDTH; ++x)
    {
      float n = function(x, y);
      heightmap[y * WIDTH + x] = n;
    }
  }

  noise_normalize_heightmap();
  noise_export_ppm(filename, heightmap, WIDTH, HEIGHT);
}

float noise_perlin_2_stub(int x, int y)
{
  return noise_perlin_2((float)x, (float)y, 0.010f);
}

float noise_perlin_2_fbm_stub(int x, int y)
{
  return noise_perlin_2_fbm((float)x, (float)y, 0.010f, 4, 2.0f, 0.5f);
}

float noise_perlin_2_fbm_rotation_stub(int x, int y)
{
  float m2[2][2] = {
      {0.80f, -0.60f},
      {0.60f, 0.80f}};

  return noise_perlin_2_fbm_rotation((float)x, (float)y, 0.010f, 9, 1.9f, 0.55f, m2);
}

float noise_perlin_3_fbm_rotation_stub(int x, int y)
{
  float m3[3][3] = {
      {0.00f, 0.80f, 0.60f},
      {-0.80f, 0.36f, -0.48f},
      {-0.60f, -0.48f, 0.64f}};

  return noise_perlin_3_fbm_rotation((float)x, (float)y, 0.0f, 0.010f, 9, 1.9f, 0.55f, m3);
}

float noise_simplex_2_stub(int x, int y)
{
  return noise_simplex_2((float)x, (float)y, 0.010f);
}

float noise_simplex_2_fbm_stub(int x, int y)
{
  return noise_simplex_2_fbm((float)x, (float)y, 0.010f, 4, 2.0f, 0.5f);
}

float noise_simplex_2_fbm_rotation_stub(int x, int y)
{
  float m2[2][2] = {
      {0.80f, -0.60f},
      {0.60f, 0.80f}};

  return noise_simplex_2_fbm_rotation((float)x, (float)y, 0.010f, 9, 1.9f, 0.55f, m2);
}

float noise_simplex_2_domain_warp_stub(int x, int y)
{
  return noise_simplex_2_domain_warp((float)x, (float)y, 0.010f, 5.0f);
}

float noise_simplex_2_domain_warp_fbm_stub(int x, int y)
{
  return noise_simplex_2_domain_warp_fbm((float)x, (float)y, 0.010f, 4, 2.0f, 0.5f, -20.0f);
}

float noise_simplex_2_domain_warp_fbm_rotation_stub(int x, int y)
{
  float m2[2][2] = {
      {0.80f, -0.60f},
      {0.60f, 0.80f}};

  return noise_simplex_2_domain_warp_fbm_rotation((float)x, (float)y, 0.010f, 3, 2.0f, 0.5f, -20.0f, m2);
}

float noise_value_2_stub(int x, int y)
{
  return noise_value_2((float)x, (float)y, 0.010f);
}

float noise_value_2_fbm_stub(int x, int y)
{
  return noise_value_2_fbm((float)x, (float)y, 0.010f, 4, 2.0f, 0.5f);
}

float noise_value_2_fbm_rotation_stub(int x, int y)
{
  float m2[2][2] = {
      {0.80f, -0.60f},
      {0.60f, 0.80f}};

  return noise_value_2_fbm_rotation((float)x, (float)y, 0.010f, 9, 1.9f, 0.55f, m2);
}

float noise_value_2_terrain_stub(int x, int y)
{
  float m2[2][2] = {
      {0.80f, -0.60f},
      {0.60f, 0.80f}};

  float e = noise_value_2_fbm_rotation((float)x / 2000.0f + 1.0f, (float)y / 2000.0f - 2.0f, 1.0f, 9, 1.9f, 0.55f, m2);

  e = 600.0f * e + 600.0f;

  /* cliffs */
  e += 90.0f * noise_smoothstep(552.0f, 594.0f, e);

  return e;
}

void noise_test_erosion(void)
{
  int x, y;

  noise_seed(1234);

  /* Generate base terrain */
  for (y = 0; y < HEIGHT; ++y)
  {
    for (x = 0; x < WIDTH; ++x)
    {
      heightmap[y * WIDTH + x] = noise_simplex_2_fbm((float)x * 0.01f, (float)y * 0.01f, 1.0f, 5, 2.0f, 0.5f);
    }
  }

  /* Apply erosions */
  noise_erosion_thermal(heightmap, WIDTH, HEIGHT, 0.02f, 20);
  noise_erosion_hydraulic(heightmap, WIDTH, HEIGHT, 20, 0.05f, 0.1f, 0.05f, 0.4f, 0.2f);
  noise_erosion_wind(heightmap, WIDTH, HEIGHT, 1.0f, 0.5f, 0.02f, 10);

  noise_normalize_heightmap();
  noise_export_ppm("erosion.ppm", heightmap, WIDTH, HEIGHT);
}

int main(void)
{
  /* Setup the PRNG seeding */
  noise_lcg_state = 1337;
  noise_seed(1337);

  /* Perlin Noise */
  noise_test_run("perlin_2.ppm", noise_perlin_2_stub);
  noise_test_run("perlin_2_fbm.ppm", noise_perlin_2_fbm_stub);
  noise_test_run("perlin_2_fbm_rotation.ppm", noise_perlin_2_fbm_rotation_stub);
  noise_test_run("perlin_3_fbm_rotation.ppm", noise_perlin_3_fbm_rotation_stub);

  /* Simplex Noise */
  noise_test_run("simplex_2.ppm", noise_simplex_2_stub);
  noise_test_run("simplex_2_fbm.ppm", noise_simplex_2_fbm_stub);
  noise_test_run("simplex_2_fbm_rotation.ppm", noise_simplex_2_fbm_rotation_stub);
  noise_test_run("simplex_2_domain_warp.ppm", noise_simplex_2_domain_warp_stub);
  noise_test_run("simplex_2_domain_warp_fbm.ppm", noise_simplex_2_domain_warp_fbm_stub);
  noise_test_run("simplex_2_domain_warp_fbm_rotation.ppm", noise_simplex_2_domain_warp_fbm_rotation_stub);

  /* Value Noise */
  noise_test_run("value_2.ppm", noise_value_2_stub);
  noise_test_run("value_2_fbm.ppm", noise_value_2_fbm_stub);
  noise_test_run("value_2_fbm_rotation.ppm", noise_value_2_fbm_rotation_stub);
  noise_test_run("value_2_terrain.ppm", noise_value_2_terrain_stub);

  /* Erosion simulation */
  noise_test_erosion();

  if (img)
  {
    free(img);
  }

  return 0;
}

/*
   -----------------------------------------------------------------------------
   This software is available under 2 licenses -- choose whichever you prefer.
   ------------------------------------------------------------------------------
   ALTERNATIVE A - MIT License
   Copyright (c) 2025 nickscha
   Permission is hereby granted, free of charge, to any person obtaining a copy of
   this software and associated documentation files (the "Software"), to deal in
   the Software without restriction, including without limitation the rights to
   use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
   of the Software, and to permit persons to whom the Software is furnished to do
   so, subject to the following conditions:
   The above copyright notice and this permission notice shall be included in all
   copies or substantial portions of the Software.
   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
   SOFTWARE.
   ------------------------------------------------------------------------------
   ALTERNATIVE B - Public Domain (www.unlicense.org)
   This is free and unencumbered software released into the public domain.
   Anyone is free to copy, modify, publish, use, compile, sell, or distribute this
   software, either in source code form or as a compiled binary, for any purpose,
   commercial or non-commercial, and by any means.
   In jurisdictions that recognize copyright laws, the author or authors of this
   software dedicate any and all copyright interest in the software to the public
   domain. We make this dedication for the benefit of the public at large and to
   the detriment of our heirs and successors. We intend this dedication to be an
   overt act of relinquishment in perpetuity of all present and future rights to
   this software under copyright law.
   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
   ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
   WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
   ------------------------------------------------------------------------------
*/
