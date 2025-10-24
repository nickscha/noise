/* noise.h - v0.3 - public domain data structures - nickscha 2025

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

static unsigned char noise_permutations[512];
static unsigned int noise_lcg_state;
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

NOISE_API NOISE_INLINE float noise_smoothstep(float a, float b, float x)
{
  float t;

  if (x <= a)
  {
    return 0.0f;
  }

  if (x >= b)
  {
    return 1.0f;
  }

  t = (x - a) / (b - a);

  return t * t * (3.0f - 2.0f * t);
}

NOISE_API NOISE_INLINE float noise_floor(float x)
{
  int i = (int)x;
  return (x < 0.0f && (float)i != x) ? (float)(i - 1) : (float)i;
}

NOISE_API NOISE_INLINE float noise_lerp(float a, float b, float t)
{
  return a + t * (b - a);
}

NOISE_API NOISE_INLINE float noise_fade(float t)
{
  return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
}

NOISE_API NOISE_INLINE float noise_fract(float x)
{
  return x - noise_floor(x);
}

NOISE_API NOISE_INLINE float noise_dot2(const float g[2], float x, float y)
{
  return g[0] * x + g[1] * y;
}

NOISE_API NOISE_INLINE float noise_dot3(const float g[3], float x, float y, float z)
{
  return g[0] * x + g[1] * y + g[2] * z;
}

NOISE_API NOISE_INLINE float noise_hash(float n)
{
  float f = noise_fract(n * 0.3183099f);
  return noise_fract(n * 17.0f * f);
}

NOISE_API NOISE_INLINE unsigned noise_lcg_next(void)
{
  noise_lcg_state = noise_lcg_state * 1664525u + 1013904223u;
  return noise_lcg_state;
}

NOISE_API NOISE_INLINE void noise_seed(unsigned int seed)
{
  int i;
  noise_lcg_state = seed;

  for (i = 0; i < 256; ++i)
  {
    noise_permutations[i] = (unsigned char)i;
  }

  for (i = 255; i > 0; --i)
  {
    unsigned r = noise_lcg_next() % (unsigned int)(i + 1);
    noise_swap_byte(&noise_permutations[i], &noise_permutations[(int)r]);
  }

  for (i = 0; i < 256; ++i)
  {
    noise_permutations[256 + i] = noise_permutations[i];
  }
}

/* #############################################################################
 * # Perlin Noise functions
 * #############################################################################
 */
NOISE_API NOISE_INLINE float noise_perlin_2(float x, float y, float frequency)
{
  int X, Y, aa, ab, ba, bb;
  float xf, yf, u, v, x1, x2, y1;
  float floor_x, floor_y;

  x *= frequency;
  y *= frequency;

  floor_x = noise_floor(x);
  floor_y = noise_floor(y);

  X = (int)floor_x & 255;
  Y = (int)floor_y & 255;
  xf = x - floor_x;
  yf = y - floor_y;
  u = noise_fade(xf);
  v = noise_fade(yf);

  aa = noise_permutations[noise_permutations[X] + Y];
  ab = noise_permutations[noise_permutations[X] + Y + 1];
  ba = noise_permutations[noise_permutations[X + 1] + Y];
  bb = noise_permutations[noise_permutations[X + 1] + Y + 1];

  x1 = noise_lerp(noise_dot2(noise_gradient_2_lut[aa & 7], xf, yf),
                  noise_dot2(noise_gradient_2_lut[ba & 7], xf - 1, yf), u);
  x2 = noise_lerp(noise_dot2(noise_gradient_2_lut[ab & 7], xf, yf - 1),
                  noise_dot2(noise_gradient_2_lut[bb & 7], xf - 1, yf - 1), u);
  y1 = noise_lerp(x1, x2, v);

  return y1 * 0.70710678f; /* normalize -1 to 1 */
}

NOISE_API NOISE_INLINE float noise_perlin_3(float x, float y, float z, float freq)
{
  int X, Y, Z, aaa, aba, aab, abb, baa, bba, bab, bbb;
  float xf, yf, zf, u, v, w, x1, x2, y1, y2;
  float floor_x, floor_y, floor_z;

  x *= freq;
  y *= freq;
  z *= freq;

  floor_x = noise_floor(x);
  floor_y = noise_floor(y);
  floor_z = noise_floor(z);

  X = (int)floor_x & 255;
  Y = (int)floor_y & 255;
  Z = (int)floor_z & 255;
  xf = x - floor_x;
  yf = y - floor_y;
  zf = z - floor_z;
  u = noise_fade(xf);
  v = noise_fade(yf);
  w = noise_fade(zf);

  aaa = noise_permutations[noise_permutations[noise_permutations[X] + Y] + Z];
  aba = noise_permutations[noise_permutations[noise_permutations[X] + Y + 1] + Z];
  aab = noise_permutations[noise_permutations[noise_permutations[X] + Y] + Z + 1];
  abb = noise_permutations[noise_permutations[noise_permutations[X] + Y + 1] + Z + 1];
  baa = noise_permutations[noise_permutations[noise_permutations[X + 1] + Y] + Z];
  bba = noise_permutations[noise_permutations[noise_permutations[X + 1] + Y + 1] + Z];
  bab = noise_permutations[noise_permutations[noise_permutations[X + 1] + Y] + Z + 1];
  bbb = noise_permutations[noise_permutations[noise_permutations[X + 1] + Y + 1] + Z + 1];

  x1 = noise_lerp(noise_dot3(noise_gradient_3_lut[aaa & 15], xf, yf, zf),
                  noise_dot3(noise_gradient_3_lut[baa & 15], xf - 1, yf, zf), u);
  x2 = noise_lerp(noise_dot3(noise_gradient_3_lut[aba & 15], xf, yf - 1, zf),
                  noise_dot3(noise_gradient_3_lut[bba & 15], xf - 1, yf - 1, zf), u);
  y1 = noise_lerp(x1, x2, v);

  x1 = noise_lerp(noise_dot3(noise_gradient_3_lut[aab & 15], xf, yf, zf - 1),
                  noise_dot3(noise_gradient_3_lut[bab & 15], xf - 1, yf, zf - 1), u);
  x2 = noise_lerp(noise_dot3(noise_gradient_3_lut[abb & 15], xf, yf - 1, zf - 1),
                  noise_dot3(noise_gradient_3_lut[bbb & 15], xf - 1, yf - 1, zf - 1), u);
  y2 = noise_lerp(x1, x2, v);

  return noise_lerp(y1, y2, w) * 0.70710678f; /* normalize -1 to 1 */
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

/* #############################################################################
 * # Simplex Noise functions
 * #############################################################################
 */
#define NOISE_SIMPLEX_F2 0.366025403f /* (sqrt(3)-1)/2 */
#define NOISE_SIMPLEX_G2 0.211324865f /* (3-sqrt(3))/6 */
#define NOISE_SIMPLEX_F3 (1.0f / 3.0f)
#define NOISE_SIMPLEX_G3 (1.0f / 6.0f)

NOISE_API NOISE_INLINE float noise_simplex_2(float x, float y, float frequency)
{
  int i, j, gi0, gi1, gi2;
  float n0, n1, n2; /* noise contributions from the three corners */
  float s, t, xs, ys, x0, y0, x1, y1, x2, y2, t0, t1, t2;
  int ii, jj;
  int i1, j1;
  int idx;

  x *= frequency;
  y *= frequency;

  /* Skew the input space to determine which simplex cell we're in */
  s = (x + y) * NOISE_SIMPLEX_F2;
  xs = x + s;
  ys = y + s;
  i = (int)noise_floor(xs);
  j = (int)noise_floor(ys);

  t = (float)(i + j) * NOISE_SIMPLEX_G2;
  x0 = x - (float)i + t; /* unskew the cell origin back to (x,y) space */
  y0 = y - (float)j + t;

  if (x0 > y0)
  {
    i1 = 1;
    j1 = 0;
  }
  else
  {
    i1 = 0;
    j1 = 1;
  }

  /* Offsets for the other corners */
  x1 = x0 - (float)i1 + NOISE_SIMPLEX_G2;
  y1 = y0 - (float)j1 + NOISE_SIMPLEX_G2;
  x2 = x0 - 1.0f + 2.0f * NOISE_SIMPLEX_G2;
  y2 = y0 - 1.0f + 2.0f * NOISE_SIMPLEX_G2;

  /* Work out the hashed gradient indices of the three simplex corners */
  ii = i & 255;
  jj = j & 255;

  /* Using permutation table to pick gradients */
  idx = (int)noise_permutations[ii + noise_permutations[jj]];
  gi0 = idx & 7; /* use 8 2D gradients */
  idx = (int)noise_permutations[ii + i1 + noise_permutations[jj + j1]];
  gi1 = idx & 7;
  idx = (int)noise_permutations[ii + 1 + noise_permutations[jj + 1]];
  gi2 = idx & 7;

  /* Calculate the contribution from the three corners */
  t0 = 0.5f - x0 * x0 - y0 * y0;

  if (t0 < 0.0f)
  {
    n0 = 0.0f;
  }
  else
  {
    t0 = t0 * t0;
    n0 = t0 * t0 * noise_dot2(noise_gradient_2_lut[gi0], x0, y0);
  }

  t1 = 0.5f - x1 * x1 - y1 * y1;

  if (t1 < 0.0f)
  {
    n1 = 0.0f;
  }
  else
  {
    t1 = t1 * t1;
    n1 = t1 * t1 * noise_dot2(noise_gradient_2_lut[gi1], x1, y1);
  }

  t2 = 0.5f - x2 * x2 - y2 * y2;

  if (t2 < 0.0f)
  {
    n2 = 0.0f;
  }
  else
  {
    t2 = t2 * t2;
    n2 = t2 * t2 * noise_dot2(noise_gradient_2_lut[gi2], x2, y2);
  }

  /* Add contributions and scale the result to cover approx [-1,1] */
  return 70.0f * (n0 + n1 + n2);
}

NOISE_API NOISE_INLINE float noise_simplex_3(float x, float y, float z, float frequency)
{
  float n0, n1, n2, n3;
  float s, t;
  float xs, ys, zs;
  int i, j, k;
  float x0, y0, z0;
  int i1, j1, k1;
  int i2, j2, k2;
  float x1, y1, z1, x2, y2, z2, x3, y3, z3;
  float t0, t1, t2, t3;
  int ii, jj, kk;
  int gi0, gi1, gi2, gi3;
  int idx;

  x *= frequency;
  y *= frequency;
  z *= frequency;

  /* Skew the input space to determine which simplex cell we're in */
  s = (x + y + z) * NOISE_SIMPLEX_F3;
  xs = x + s;
  ys = y + s;
  zs = z + s;
  i = (int)noise_floor(xs);
  j = (int)noise_floor(ys);
  k = (int)noise_floor(zs);

  t = (float)(i + j + k) * NOISE_SIMPLEX_G3;
  x0 = x - (float)i + t;
  y0 = y - (float)j + t;
  z0 = z - (float)k + t;

  /* Rank x0, y0, z0 to find simplex corner offsets */
  if (x0 >= y0)
  {
    if (y0 >= z0)
    {
      /* X Y Z order */
      i1 = 1;
      j1 = 0;
      k1 = 0;
      i2 = 1;
      j2 = 1;
      k2 = 0;
    }
    else if (x0 >= z0)
    {
      /* X Z Y order */
      i1 = 1;
      j1 = 0;
      k1 = 0;
      i2 = 1;
      j2 = 0;
      k2 = 1;
    }
    else
    {
      /* Z X Y order */
      i1 = 0;
      j1 = 0;
      k1 = 1;
      i2 = 1;
      j2 = 0;
      k2 = 1;
    }
  }
  else
  { /* x0 < y0 */
    if (y0 < z0)
    {
      /* Z Y X order */
      i1 = 0;
      j1 = 0;
      k1 = 1;
      i2 = 0;
      j2 = 1;
      k2 = 1;
    }
    else if (x0 < z0)
    {
      /* Y Z X order */
      i1 = 0;
      j1 = 1;
      k1 = 0;
      i2 = 0;
      j2 = 1;
      k2 = 1;
    }
    else
    {
      /* Y X Z order */
      i1 = 0;
      j1 = 1;
      k1 = 0;
      i2 = 1;
      j2 = 1;
      k2 = 0;
    }
  }

  /* Offsets for corners */
  x1 = x0 - (float)i1 + NOISE_SIMPLEX_G3;
  y1 = y0 - (float)j1 + NOISE_SIMPLEX_G3;
  z1 = z0 - (float)k1 + NOISE_SIMPLEX_G3;
  x2 = x0 - (float)i2 + 2.0f * NOISE_SIMPLEX_G3;
  y2 = y0 - (float)j2 + 2.0f * NOISE_SIMPLEX_G3;
  z2 = z0 - (float)k2 + 2.0f * NOISE_SIMPLEX_G3;
  x3 = x0 - 1.0f + 3.0f * NOISE_SIMPLEX_G3;
  y3 = y0 - 1.0f + 3.0f * NOISE_SIMPLEX_G3;
  z3 = z0 - 1.0f + 3.0f * NOISE_SIMPLEX_G3;

  /* Work out hashed gradient indices of the four simplex corners */
  ii = i & 255;
  jj = j & 255;
  kk = k & 255;

  idx = (int)noise_permutations[ii + noise_permutations[jj + noise_permutations[kk]]];
  gi0 = idx & 15; /* use 16 3D gradients */
  idx = (int)noise_permutations[ii + i1 + noise_permutations[jj + j1 + noise_permutations[kk + k1]]];
  gi1 = idx & 15;
  idx = (int)noise_permutations[ii + i2 + noise_permutations[jj + j2 + noise_permutations[kk + k2]]];
  gi2 = idx & 15;
  idx = (int)noise_permutations[ii + 1 + noise_permutations[jj + 1 + noise_permutations[kk + 1]]];
  gi3 = idx & 15;

  /* Calculate the contribution from the four corners */
  t0 = 0.6f - x0 * x0 - y0 * y0 - z0 * z0;

  if (t0 < 0.0f)
  {
    n0 = 0.0f;
  }
  else
  {
    t0 = t0 * t0;
    n0 = t0 * t0 * noise_dot3(noise_gradient_3_lut[gi0], x0, y0, z0);
  }

  t1 = 0.6f - x1 * x1 - y1 * y1 - z1 * z1;

  if (t1 < 0.0f)
  {
    n1 = 0.0f;
  }
  else
  {
    t1 = t1 * t1;
    n1 = t1 * t1 * noise_dot3(noise_gradient_3_lut[gi1], x1, y1, z1);
  }

  t2 = 0.6f - x2 * x2 - y2 * y2 - z2 * z2;

  if (t2 < 0.0f)
  {
    n2 = 0.0f;
  }
  else
  {
    t2 = t2 * t2;
    n2 = t2 * t2 * noise_dot3(noise_gradient_3_lut[gi2], x2, y2, z2);
  }

  t3 = 0.6f - x3 * x3 - y3 * y3 - z3 * z3;

  if (t3 < 0.0f)
  {
    n3 = 0.0f;
  }
  else
  {
    t3 = t3 * t3;
    n3 = t3 * t3 * noise_dot3(noise_gradient_3_lut[gi3], x3, y3, z3);
  }

  /* Sum up and scale to cover the range roughly [-1,1] */
  return 32.0f * (n0 + n1 + n2 + n3);
}

NOISE_API NOISE_INLINE float noise_simplex_2_fbm(float x, float y, float frequency, int octaves, float lacunarity, float gain)
{
  int i;
  float sum = 0.0f;
  float amp = 1.0f;
  float f = frequency;
  float norm = 0.0f;

  for (i = 0; i < octaves; ++i)
  {
    sum += amp * noise_simplex_2(x, y, f);
    norm += amp;
    f *= lacunarity;
    amp *= gain;
  }

  return sum / norm;
}

NOISE_API NOISE_INLINE float noise_simplex_3_fbm(float x, float y, float z, float frequency, int octaves, float lacunarity, float gain)
{
  int i;
  float sum = 0.0f;
  float amp = 1.0f;
  float f = frequency;
  float norm = 0.0f;

  for (i = 0; i < octaves; ++i)
  {
    sum += amp * noise_simplex_3(x, y, z, f);
    norm += amp;
    f *= lacunarity;
    amp *= gain;
  }

  return sum / norm;
}

NOISE_API NOISE_INLINE float noise_simplex_2_fbm_rotation(
    float x, float y,
    float frequency,
    int octaves,
    float lacunarity,
    float gain,
    float rotation[2][2])
{
  int i;
  float sum = 0.0f;
  float amp = 1.0f;
  float norm = 0.0f;
  float p[2];
  float tmp[2];

  p[0] = x * frequency;
  p[1] = y * frequency;

  for (i = 0; i < octaves; ++i)
  {
    /* sample noise */
    sum += amp * noise_simplex_2(p[0], p[1], 1.0f);
    norm += amp;

    /* rotate and scale */
    noise_m2x2_mul(rotation, p, tmp);
    p[0] = tmp[0] * lacunarity;
    p[1] = tmp[1] * lacunarity;

    amp *= gain;
  }

  return sum / norm;
}

NOISE_API NOISE_INLINE float noise_simplex_3_fbm_rotation(
    float x, float y, float z,
    float frequency,
    int octaves,
    float lacunarity,
    float gain,
    float rotation[3][3])
{
  int i;
  float sum = 0.0f;
  float amp = 1.0f;
  float norm = 0.0f;
  float p[3];
  float tmp[3];

  p[0] = x * frequency;
  p[1] = y * frequency;
  p[2] = z * frequency;

  for (i = 0; i < octaves; ++i)
  {
    /* sample noise */
    sum += amp * noise_simplex_3(p[0], p[1], p[2], 1.0f);
    norm += amp;

    /* rotate and scale */
    noise_m3x3_mul(rotation, p, tmp);
    p[0] = tmp[0] * lacunarity;
    p[1] = tmp[1] * lacunarity;
    p[2] = tmp[2] * lacunarity;

    amp *= gain;
  }

  return sum / norm;
}

/* #############################################################################
 * # Value Noise functions
 * #############################################################################
 */
NOISE_API NOISE_INLINE float noise_value_2(float x, float y, float frequency)
{
  float px, py; /* integer lattice point */
  float wx, wy; /* fractional part */
  float ux, uy; /* fade curve */
  float a, b, c, d;
  float k0, k1, k2, k4;

  /* scale input by frequency */
  x *= frequency;
  y *= frequency;

  px = noise_floor(x);
  py = noise_floor(y);

  wx = noise_fract(x);
  wy = noise_fract(y);

  /* fade curve */
  ux = wx * wx * wx * (wx * (wx * 6.0f - 15.0f) + 10.0f);
  uy = wy * wy * wy * (wy * (wy * 6.0f - 15.0f) + 10.0f);

  a = noise_hash(px + 317.0f * py + 0.0f);
  b = noise_hash(px + 317.0f * py + 1.0f);
  c = noise_hash(px + 317.0f * (py + 1.0f) + 0.0f);
  d = noise_hash(px + 317.0f * (py + 1.0f) + 1.0f);

  k0 = a;
  k1 = b - a;
  k2 = c - a;
  k4 = a - b - c + d;

  return -1.0f + 2.0f * (k0 + k1 * ux + k2 * uy + k4 * ux * uy);
}

NOISE_API NOISE_INLINE float noise_value_2_fbm(float x, float y, float frequency, int octaves, float lacunarity, float gain)
{
  int i;
  float sum = 0.0f;
  float amp = 1.0f;
  float f = frequency;
  float norm = 0.0f;

  for (i = 0; i < octaves; ++i)
  {
    sum += amp * noise_value_2(x, y, f);
    norm += amp;

    f *= lacunarity;
    amp *= gain;
  }

  return sum / norm;
}

NOISE_API NOISE_INLINE float noise_value_2_fbm_rotation(float x, float y, float frequency, int octaves, float lacunarity, float gain, float rotation[2][2])
{
  int i;
  float sum = 0.0f;
  float amp = 1.0f;
  float norm = 0.0f;
  float p[2], tmp[2];

  /* initial point scaled by frequency */
  p[0] = x * frequency;
  p[1] = y * frequency;

  for (i = 0; i < octaves; ++i)
  {
    /* sample value noise at frequency 1.0 */
    sum += amp * noise_value_2(p[0], p[1], 1.0f);
    norm += amp;

    /* rotate and scale for next octave */
    noise_m2x2_mul(rotation, p, tmp);
    p[0] = tmp[0] * lacunarity;
    p[1] = tmp[1] * lacunarity;

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
