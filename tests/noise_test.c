/* noise.h - v0.2 - public domain data structures - nickscha 2025

A C89 standard compliant, single header, nostdlib (no C Standard Library) Noise Generation (NOISE).

This Test class defines cases to verify that we don't break the excepted behaviours in the future upon changes.

LICENSE

  Placed in the public domain and also MIT licensed.
  See end of file for detailed license information.

*/
#include "../noise.h"     /* Noise Generation */
#include "../deps/test.h" /* Simple Testing framework    */

#define PERF_STATS_ENABLE
#define PERF_DISBALE_INTERMEDIATE_PRINT
#include "../deps/perf.h" /* Simple Performance profiler */
#include <stdio.h>

/* Export PPM */
void noise_export_ppm(const char *filename, float *data, int width, int height)
{
  int i, j;

  FILE *f = fopen(filename, "wb");

  if (!f)
  {
    fprintf(stderr, "Failed to open file\n");
    return;
  }

  fprintf(f, "P6\n%d %d\n255\n", width, height);

  for (j = 0; j < height; ++j)
  {
    for (i = 0; i < width; ++i)
    {
      float v = data[j * width + i];
      unsigned char c;

      if (v < 0)
      {
        v = 0;
      }

      if (v > 1)
      {
        v = 1;
      }

      c = (unsigned char)(v * 255);

      fputc(c, f);
      fputc(c, f);
      fputc(c, f);
    }
  }
  fclose(f);

  printf("[noise] %s exported.\n", filename);
}

#define WIDTH 512
#define HEIGHT 512
float heightmap[WIDTH * HEIGHT];

void noise_normalize_heightmap(void)
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

  PERF_PROFILE_WITH_NAME({
  for (y = 0; y < HEIGHT; ++y)
  {
    for (x = 0; x < WIDTH; ++x)
    {
      float n = function(x, y);
      heightmap[y * WIDTH + x] = n;
    }
  } }, filename);

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

void noise_profile(void)
{
  int x, y;

  float m2[2][2] = {
      {0.80f, -0.60f},
      {0.60f, 0.80f}};

  for (y = 0; y < HEIGHT; ++y)
  {
    for (x = 0; x < WIDTH; ++x)
    {
      /* Perlin Noise */
      PERF_PROFILE_WITH_NAME({ float n = noise_perlin_2_fbm((float)x, (float)y, 0.010f, 4, 2.0f, 0.5f); (void) n; }, "perlin_2_fbm_4_octaves");
      PERF_PROFILE_WITH_NAME({ float n = noise_perlin_2_fbm((float)x, (float)y, 0.010f, 8, 2.0f, 0.5f); (void) n; }, "perlin_2_fbm_8_octaves");
      PERF_PROFILE_WITH_NAME({ float n = noise_perlin_2_fbm_rotation((float)x, (float)y, 0.010f, 4, 2.0f, 0.5f, m2); (void) n; }, "perlin_2_fbm_rotation_4_octaves");
      PERF_PROFILE_WITH_NAME({ float n = noise_perlin_2_fbm_rotation((float)x, (float)y, 0.010f, 8, 2.0f, 0.5f, m2); (void) n; }, "perlin_2_fbm_rotation_8_octaves");

      /* Simplex Noise */
      PERF_PROFILE_WITH_NAME({ float n = noise_simplex_2_fbm((float)x, (float)y, 0.010f, 4, 2.0f, 0.5f); (void) n; }, "simplex_2_fbm_4_octaves");
      PERF_PROFILE_WITH_NAME({ float n = noise_simplex_2_fbm((float)x, (float)y, 0.010f, 8, 2.0f, 0.5f); (void) n; }, "simplex_2_fbm_8_octaves");
      PERF_PROFILE_WITH_NAME({ float n = noise_simplex_2_fbm_rotation((float)x, (float)y, 0.010f, 4, 2.0f, 0.5f, m2); (void) n; }, "simplex_2_fbm_rotation_4_octaves");
      PERF_PROFILE_WITH_NAME({ float n = noise_simplex_2_fbm_rotation((float)x, (float)y, 0.010f, 8, 2.0f, 0.5f, m2); (void) n; }, "simplex_2_fbm_rotation_8_octaves");
    }
  }
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

  /* Print collected performance profiling metrics */
  noise_profile();
  perf_print_stats();

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
