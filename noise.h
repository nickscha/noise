/* noise.h - v0.1 - public domain data structures - nickscha 2025

A C89 standard compliant, single header, nostdlib (no C Standard Library) Noise Generation (NOISE).

LICENSE

  Placed in the public domain and also MIT licensed.
  See end of file for detailed license information.

*/
#ifndef NOISE_H
#define NOISE_H

/* #############################################################################
 * # COMPILER SETTINGS
 * #############################################################################
 */
/* Check if using C99 or later (inline is supported) */
#if __STDC_VERSION__ >= 199901L
#define NOISE_INLINE inline
#elif defined(__GNUC__) || defined(__clang__)
#define NOISE_INLINE __inline__
#elif defined(_MSC_VER)
#define NOISE_INLINE __inline
#else
#define NOISE_INLINE
#endif

#define NOISE_API static

#define NOISE_FLOOR(x) ((float)((int)(x)) - (((x) < (float)((int)(x))) ? 1.0f : 0.0f))
#define NOISE_FADE(t) ((t) * (t) * (t) * ((t) * ((t) * 6.0f - 15.0f) + 10.0f))
#define NOISE_LERP(a, b, t) ((a) + (t) * ((b) - (a)))
#define NOISE_DOT_2(g, x, y) ((g)[0] * (x) + (g)[1] * (y))
#define NOISE_DOT_3(g, x, y, z) ((g)[0] * (x) + (g)[1] * (y) + (g)[2] * (z))

static unsigned char noise_permutations[512];
static int noise_perlin_initialized = 0;
static unsigned noise_lcg_state;
static float noise_gradient_2_lut[8][2] = {{1, 1}, {-1, 1}, {1, -1}, {-1, -1}, {1, 0}, {-1, 0}, {0, 1}, {0, -1}};
static float noise_gradient_3_lut[16][3] = {{1, 1, 0}, {-1, 1, 0}, {1, -1, 0}, {-1, -1, 0}, {1, 0, 1}, {-1, 0, 1}, {1, 0, -1}, {-1, 0, -1}, {0, 1, 1}, {0, -1, 1}, {0, 1, -1}, {0, -1, -1}, {1, 1, 0}, {-1, 1, 0}, {0, -1, 1}, {0, -1, -1}};

NOISE_API NOISE_INLINE void noise_swap_byte(unsigned char *a, unsigned char *b)
{
  unsigned char t = *a;
  *a = *b;
  *b = t;
}

NOISE_API NOISE_INLINE void noise_m2x2_mul(float m[2][2], float v[2], float out[2])
{
  out[0] = m[0][0] * v[0] + m[0][1] * v[1];
  out[1] = m[1][0] * v[0] + m[1][1] * v[1];
}

NOISE_API NOISE_INLINE void noise_m3x3_mul(float m[3][3], float v[3], float out[3])
{
  out[0] = m[0][0] * v[0] + m[0][1] * v[1] + m[0][2] * v[2];
  out[1] = m[1][0] * v[0] + m[1][1] * v[1] + m[1][2] * v[2];
  out[2] = m[2][0] * v[0] + m[2][1] * v[1] + m[2][2] * v[2];
}

NOISE_API NOISE_INLINE unsigned noise_lcg_next(void)
{
  noise_lcg_state = noise_lcg_state * 1664525u + 1013904223u;
  return noise_lcg_state;
}

/* #############################################################################
 * # Perlin Noise functions
 * #############################################################################
 */
NOISE_API NOISE_INLINE void noise_perlin_seed(unsigned seed)
{
  int i;
  noise_lcg_state = seed;

  for (i = 0; i < 256; ++i)
  {
    noise_permutations[i] = (unsigned char)i;
  }

  for (i = 255; i > 0; --i)
  {
    unsigned r = noise_lcg_next() % (unsigned)(i + 1);
    noise_swap_byte(&noise_permutations[i], &noise_permutations[(int)r]);
  }

  for (i = 0; i < 256; ++i)
  {
    noise_permutations[256 + i] = noise_permutations[i];
  }

  noise_perlin_initialized = 1;
}

NOISE_API NOISE_INLINE float noise_perlin_2(float x, float y, float frequency)
{
  int X, Y, aa, ab, ba, bb;
  float xf, yf, u, v, x1, x2, y1;

  if (!noise_perlin_initialized)
  {
    noise_perlin_seed(0xDEADBEEFu);
  }

  x *= frequency;
  y *= frequency;
  X = (int)NOISE_FLOOR(x) & 255;
  Y = (int)NOISE_FLOOR(y) & 255;
  xf = x - NOISE_FLOOR(x);
  yf = y - NOISE_FLOOR(y);
  u = NOISE_FADE(xf);
  v = NOISE_FADE(yf);

  aa = noise_permutations[X + noise_permutations[Y]];
  ab = noise_permutations[X + noise_permutations[Y + 1]];
  ba = noise_permutations[X + 1 + noise_permutations[Y]];
  bb = noise_permutations[X + 1 + noise_permutations[Y + 1]];

  x1 = NOISE_LERP(NOISE_DOT_2(noise_gradient_2_lut[aa & 7], xf, yf),
                  NOISE_DOT_2(noise_gradient_2_lut[ba & 7], xf - 1, yf), u);
  x2 = NOISE_LERP(NOISE_DOT_2(noise_gradient_2_lut[ab & 7], xf, yf - 1),
                  NOISE_DOT_2(noise_gradient_2_lut[bb & 7], xf - 1, yf - 1), u);
  y1 = NOISE_LERP(x1, x2, v);

  return y1 * 0.7071f; /* normalize -1 to 1 */
}

NOISE_API NOISE_INLINE float noise_perlin_3(float x, float y, float z, float freq)
{
  int X, Y, Z, aaa, aba, aab, abb, baa, bba, bab, bbb;
  float xf, yf, zf, u, v, w, x1, x2, y1, y2;

  if (!noise_perlin_initialized)
  {
    noise_perlin_seed(0xDEADBEEFu);
  }

  x *= freq;
  y *= freq;
  z *= freq;
  X = (int)NOISE_FLOOR(x) & 255;
  Y = (int)NOISE_FLOOR(y) & 255;
  Z = (int)NOISE_FLOOR(z) & 255;
  xf = x - NOISE_FLOOR(x);
  yf = y - NOISE_FLOOR(y);
  zf = z - NOISE_FLOOR(z);
  u = NOISE_FADE(xf);
  v = NOISE_FADE(yf);
  w = NOISE_FADE(zf);

  aaa = noise_permutations[X + noise_permutations[Y + noise_permutations[Z]]];
  aba = noise_permutations[X + noise_permutations[Y + 1 + noise_permutations[Z]]];
  aab = noise_permutations[X + noise_permutations[Y + noise_permutations[Z + 1]]];
  abb = noise_permutations[X + noise_permutations[Y + 1 + noise_permutations[Z + 1]]];
  baa = noise_permutations[X + 1 + noise_permutations[Y + noise_permutations[Z]]];
  bba = noise_permutations[X + 1 + noise_permutations[Y + 1 + noise_permutations[Z]]];
  bab = noise_permutations[X + 1 + noise_permutations[Y + noise_permutations[Z + 1]]];
  bbb = noise_permutations[X + 1 + noise_permutations[Y + 1 + noise_permutations[Z + 1]]];

  x1 = NOISE_LERP(NOISE_DOT_3(noise_gradient_3_lut[aaa & 15], xf, yf, zf),
                  NOISE_DOT_3(noise_gradient_3_lut[baa & 15], xf - 1, yf, zf), u);
  x2 = NOISE_LERP(NOISE_DOT_3(noise_gradient_3_lut[aba & 15], xf, yf - 1, zf),
                  NOISE_DOT_3(noise_gradient_3_lut[bba & 15], xf - 1, yf - 1, zf), u);
  y1 = NOISE_LERP(x1, x2, v);

  x1 = NOISE_LERP(NOISE_DOT_3(noise_gradient_3_lut[aab & 15], xf, yf, zf - 1),
                  NOISE_DOT_3(noise_gradient_3_lut[bab & 15], xf - 1, yf, zf - 1), u);
  x2 = NOISE_LERP(NOISE_DOT_3(noise_gradient_3_lut[abb & 15], xf, yf - 1, zf - 1),
                  NOISE_DOT_3(noise_gradient_3_lut[bbb & 15], xf - 1, yf - 1, zf - 1), u);
  y2 = NOISE_LERP(x1, x2, v);

  return NOISE_LERP(y1, y2, w) * 0.7071f; /* normalize -1 to 1 */
}

NOISE_API NOISE_INLINE float noise_perlin_2_fbm(float x, float y, float frequency, int octaves, float lacunarity, float gain)
{
  int i;
  float sum = 0, amp = 1, f = frequency, norm = 0;

  for (i = 0; i < octaves; ++i)
  {
    sum += amp * noise_perlin_2(x, y, f);
    norm += amp;
    f *= lacunarity;
    amp *= gain;
  }

  return sum / norm;
}

NOISE_API NOISE_INLINE float noise_perlin_2_fbm_rotation(float x, float y, float frequency, int octaves, float lacunarity, float gain, float rotation[2][2])
{
  int i;
  float sum = 0.0f, amp = 1.0f, norm = 0.0f;
  float p[2];
  p[0] = x * frequency;
  p[1] = y * frequency;

  for (i = 0; i < octaves; ++i)
  {
    float tmp[2];

    /* sample noise */
    sum += amp * noise_perlin_2(p[0], p[1], 1.0f);
    norm += amp;

    /* rotate then scale */
    noise_m2x2_mul(rotation, p, tmp);
    p[0] = tmp[0] * lacunarity;
    p[1] = tmp[1] * lacunarity;

    amp *= gain;
  }

  return sum / norm;
}

NOISE_API NOISE_INLINE float noise_perlin_3_fbm(float x, float y, float z, float frequency, int octaves, float lacunarity, float gain)
{
  int i;
  float sum = 0, amp = 1, f = frequency, norm = 0;

  for (i = 0; i < octaves; ++i)
  {
    sum += amp * noise_perlin_3(x, y, z, f);
    norm += amp;
    f *= lacunarity;
    amp *= gain;
  }

  return sum / norm;
}

NOISE_API NOISE_INLINE float noise_perlin_3_fbm_rotation(float x, float y, float z, float frequency, int octaves, float lacunarity, float gain, float rotation[3][3])
{
  int i;
  float sum = 0.0f, amp = 1.0f, norm = 0.0f;
  float p[3];

  p[0] = x * frequency;
  p[1] = y * frequency;
  p[2] = z * frequency;

  for (i = 0; i < octaves; ++i)
  {
    float tmp[3];

    /* sample noise */
    sum += amp * noise_perlin_3(p[0], p[1], p[2], 1.0f);
    norm += amp;

    /* rotate then scale */
    noise_m3x3_mul(rotation, p, tmp);
    p[0] = tmp[0] * lacunarity;
    p[1] = tmp[1] * lacunarity;
    p[2] = tmp[2] * lacunarity;

    amp *= gain;
  }

  return sum / norm;
}

#endif /* NOISE_H */

/*
   ------------------------------------------------------------------------------
   This software is available under 2 licenses -- choose whichever you prefer.
   ------------------------------------------------------------------------------
   ALTERNATIVE A - MIT License
   Copyright (c) 2025 nickscha
   noise_permutationsission is hereby granted, free of charge, to any person obtaining a copy of
   this software and associated documentation files (the "Software"), to deal in
   the Software without restriction, including without limitation the rights to
   use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
   of the Software, and to noise_permutationsit persons to whom the Software is furnished to do
   so, subject to the following conditions:
   The above copyright notice and this noise_permutationsission notice shall be included in all
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
