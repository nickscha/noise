/* ========================================================================== */
/* nostdlib Math Primitives                                                   */
/* ========================================================================== */
/* ========================================================================== */
/* C89 nostdlib vector and matrix types                                       */
/* ========================================================================== */

typedef struct
{
    float x, y;
} vec2;
typedef struct
{
    float x, y, z;
} vec3;
typedef struct
{
    float x, y, z, w;
} vec4;

typedef struct
{
    vec2 c[2];
} mat2; /* Column-major */
typedef struct
{
    vec3 c[3];
} mat3; /* Column-major */

float floorf_custom(float x)
{
    int i = (int)x;
    if (x < 0.0f && x != (float)i)
    {
        return (float)(i - 1);
    }
    return (float)i;
}

float fractf_custom(float x)
{
    return x - floorf_custom(x);
}

float sqrtf_custom(float x)
{
    /* Babylonian method for square root */
    float guess = x;
    int i;
    if (x < 0.0f)
        return 0.0f;
    if (x == 0.0f)
        return 0.0f;
    for (i = 0; i < 10; ++i)
    {
        guess = 0.5f * (guess + x / guess);
    }
    return guess;
}

float minf_custom(float a, float b)
{
    return a < b ? a : b;
}

float maxf_custom(float a, float b)
{
    return a > b ? a : b;
}

float clampf_custom(float x, float minVal, float maxVal)
{
    return maxf_custom(minVal, minf_custom(x, maxVal));
}

vec2 smoothstepd_custom(float a, float b, float x)
{
    vec2 result;
    float ir;
    if (x < a)
    {
        result.x = 0.0f;
        result.y = 0.0f;
        return result;
    }
    if (x > b)
    {
        result.x = 1.0f;
        result.y = 0.0f;
        return result;
    }
    ir = 1.0f / (b - a);
    x = (x - a) * ir;
    result.x = x * x * (3.0f - 2.0f * x);
    result.y = 6.0f * x * (1.0f - x) * ir;
    return result;
}

/* ========================================================================== */
/* Vector and Matrix Math Helpers                                             */
/* ========================================================================== */

/* vec2 operations */
vec2 vec2_add(vec2 a, vec2 b)
{
    vec2 r = {a.x + b.x, a.y + b.y};
    return r;
}
vec2 vec2_mul_scalar(vec2 v, float s)
{
    vec2 r = {v.x * s, v.y * s};
    return r;
}
vec2 vec2_mul_vec2(vec2 a, vec2 b)
{
    vec2 r = {a.x * b.x, a.y * b.y};
    return r;
}
vec2 vec2_sub(vec2 a, vec2 b)
{
    vec2 r = {a.x - b.x, a.y - b.y};
    return r;
}
vec2 vec2_floor(vec2 v)
{
    vec2 r = {floorf_custom(v.x), floorf_custom(v.y)};
    return r;
}
vec2 vec2_fract(vec2 v)
{
    vec2 r = {fractf_custom(v.x), fractf_custom(v.y)};
    return r;
}

/* vec3 operations */
vec3 vec3_add(vec3 a, vec3 b)
{
    vec3 r = {a.x + b.x, a.y + b.y, a.z + b.z};
    return r;
}
vec3 vec3_mul_scalar(vec3 v, float s)
{
    vec3 r = {v.x * s, v.y * s, v.z * s};
    return r;
}
vec3 vec3_mul_vec3(vec3 a, vec3 b)
{
    vec3 r = {a.x * b.x, a.y * b.y, a.z * b.z};
    return r;
}
vec3 vec3_floor(vec3 v)
{
    vec3 r = {floorf_custom(v.x), floorf_custom(v.y), floorf_custom(v.z)};
    return r;
}
vec3 vec3_fract(vec3 v)
{
    vec3 r = {fractf_custom(v.x), fractf_custom(v.y), fractf_custom(v.z)};
    return r;
}
float vec3_length(vec3 v) { return sqrtf_custom(v.x * v.x + v.y * v.y + v.z * v.z); }
vec3 vec3_normalize(vec3 v)
{
    float len = vec3_length(v);
    if (len > 0.0f)
    {
        return vec3_mul_scalar(v, 1.0f / len);
    }
    return v;
}

/* Matrix operations */
vec2 mat2_mul_vec2(mat2 m, vec2 v)
{
    vec2 r;
    r.x = m.c[0].x * v.x + m.c[1].x * v.y;
    r.y = m.c[0].y * v.x + m.c[1].y * v.y;
    return r;
}

mat2 mat2_mul_mat2(mat2 a, mat2 b)
{
    mat2 r;
    r.c[0] = mat2_mul_vec2(a, b.c[0]);
    r.c[1] = mat2_mul_vec2(a, b.c[1]);
    return r;
}

mat2 mat2_mul_scalar(mat2 m, float s)
{
    mat2 r;
    r.c[0] = vec2_mul_scalar(m.c[0], s);
    r.c[1] = vec2_mul_scalar(m.c[1], s);
    return r;
}

vec3 mat3_mul_vec3(mat3 m, vec3 v)
{
    vec3 r;
    r.x = m.c[0].x * v.x + m.c[1].x * v.y + m.c[2].x * v.z;
    r.y = m.c[0].y * v.x + m.c[1].y * v.y + m.c[2].y * v.z;
    r.z = m.c[0].z * v.x + m.c[1].z * v.y + m.c[2].z * v.z;
    return r;
}

mat3 mat3_mul_mat3(mat3 a, mat3 b)
{
    mat3 r;
    r.c[0] = mat3_mul_vec3(a, b.c[0]);
    r.c[1] = mat3_mul_vec3(a, b.c[1]);
    r.c[2] = mat3_mul_vec3(a, b.c[2]);
    return r;
}

mat3 mat3_mul_scalar(mat3 m, float s)
{
    mat3 r;
    r.c[0] = vec3_mul_scalar(m.c[0], s);
    r.c[1] = vec3_mul_scalar(m.c[1], s);
    r.c[2] = vec3_mul_scalar(m.c[2], s);
    return r;
}

/* ========================================================================== */
/* Hashes (low quality, as in original)                                       */
/* ========================================================================== */

float hash1v2(vec2 p)
{
    p = vec2_mul_scalar(p, 0.3183099f);
    p = vec2_fract(p);
    p = vec2_mul_scalar(p, 50.0f);
    return fractf_custom(p.x * p.y * (p.x + p.y));
}

float hash1f(float n)
{
    return fractf_custom(n * 17.0f * fractf_custom(n * 0.3183099f));
}

vec2 hash2v2(vec2 p)
{
    const vec2 k = {0.3183099f, 0.3678794f};
    float n = 111.0f * p.x + 113.0f * p.y;
    vec2 fr_kn = {fractf_custom(k.x * n), fractf_custom(k.y * n)};
    vec2 r = {n * fr_kn.x, n * fr_kn.y};
    return vec2_fract(r);
}

/* ========================================================================== */
/* Noises                                                                     */
/* ========================================================================== */

vec4 noised3d(vec3 x)
{
    vec3 p = vec3_floor(x);
    vec3 w = vec3_fract(x);
    vec3 u, du;
    float n, a, b, c, d, e, f, g, h;
    float k0, k1, k2, k3, k4, k5, k6, k7;
    vec4 result;
    vec3 deriv;

#if 1 /* Quintic interpolation */
    vec3 w2 = vec3_mul_vec3(w, w);
    vec3 w3 = vec3_mul_vec3(w2, w);
    vec3 w4 = vec3_mul_vec3(w3, w);
    vec3 w5 = vec3_mul_vec3(w4, w);
    u.x = 6.0f * w5.x - 15.0f * w4.x + 10.0f * w3.x;
    u.y = 6.0f * w5.y - 15.0f * w4.y + 10.0f * w3.y;
    u.z = 6.0f * w5.z - 15.0f * w4.z + 10.0f * w3.z;
    du.x = 30.0f * (w4.x - 2.0f * w3.x + w2.x);
    du.y = 30.0f * (w4.y - 2.0f * w3.y + w2.y);
    du.z = 30.0f * (w4.z - 2.0f * w3.z + w2.z);
#else /* Cubic interpolation */
    vec3 w2 = vec3_mul_vec3(w, w);
    u.x = w2.x * (3.0f - 2.0f * w.x);
    u.y = w2.y * (3.0f - 2.0f * w.y);
    u.z = w2.z * (3.0f - 2.0f * w.z);
    du.x = 6.0f * w.x * (1.0f - w.x);
    du.y = 6.0f * w.y * (1.0f - w.y);
    du.z = 6.0f * w.z * (1.0f - w.z);
#endif

    n = p.x + 317.0f * p.y + 157.0f * p.z;

    a = hash1f(n + 0.0f);
    b = hash1f(n + 1.0f);
    c = hash1f(n + 317.0f);
    d = hash1f(n + 318.0f);
    e = hash1f(n + 157.0f);
    f = hash1f(n + 158.0f);
    g = hash1f(n + 474.0f);
    h = hash1f(n + 475.0f);

    k0 = a;
    k1 = b - a;
    k2 = c - a;
    k3 = e - a;
    k4 = a - b - c + d;
    k5 = a - c - e + g;
    k6 = a - b - e + f;
    k7 = -a + b + c - d + e - f - g + h;

    result.x = -1.0f + 2.0f * (k0 + k1 * u.x + k2 * u.y + k3 * u.z + k4 * u.x * u.y + k5 * u.y * u.z + k6 * u.z * u.x + k7 * u.x * u.y * u.z);

    deriv.x = k1 + k4 * u.y + k6 * u.z + k7 * u.y * u.z;
    deriv.y = k2 + k5 * u.z + k4 * u.x + k7 * u.z * u.x;
    deriv.z = k3 + k6 * u.x + k5 * u.y + k7 * u.x * u.y;

    result.y = 2.0f * du.x * deriv.x;
    result.z = 2.0f * du.y * deriv.y;
    result.w = 2.0f * du.z * deriv.z;

    return result;
}

float noise3d(vec3 x)
{
    vec4 n = noised3d(x);
    return n.x;
}

vec3 noised2d(vec2 x)
{
    vec2 p = vec2_floor(x);
    vec2 w = vec2_fract(x);
    vec2 u, du;
    float a, b, c, d;
    float k0, k1, k2, k4;
    vec3 result;
    vec2 deriv;

#if 1 /* Quintic interpolation */
    vec2 w2 = vec2_mul_vec2(w, w);
    vec2 w3 = vec2_mul_vec2(w2, w);
    vec2 w4 = vec2_mul_vec2(w3, w);
    vec2 w5 = vec2_mul_vec2(w4, w);
    u.x = 6.0f * w5.x - 15.0f * w4.x + 10.0f * w3.x;
    u.y = 6.0f * w5.y - 15.0f * w4.y + 10.0f * w3.y;
    du.x = 30.0f * (w4.x - 2.0f * w3.x + w2.x);
    du.y = 30.0f * (w4.y - 2.0f * w3.y + w2.y);
#else /* Cubic interpolation */
    vec2 w2 = vec2_mul_vec2(w, w);
    u.x = w2.x * (3.0f - 2.0f * w.x);
    u.y = w2.y * (3.0f - 2.0f * w.y);
    du.x = 6.0f * w.x * (1.0f - w.x);
    du.y = 6.0f * w.y * (1.0f - w.y);
#endif

    a = hash1v2(vec2_add(p, (vec2){0.0f, 0.0f}));
    b = hash1v2(vec2_add(p, (vec2){1.0f, 0.0f}));
    c = hash1v2(vec2_add(p, (vec2){0.0f, 1.0f}));
    d = hash1v2(vec2_add(p, (vec2){1.0f, 1.0f}));

    k0 = a;
    k1 = b - a;
    k2 = c - a;
    k4 = a - b - c + d;

    result.x = -1.0f + 2.0f * (k0 + k1 * u.x + k2 * u.y + k4 * u.x * u.y);

    deriv.x = k1 + k4 * u.y;
    deriv.y = k2 + k4 * u.x;

    result.y = 2.0f * du.x * deriv.x;
    result.z = 2.0f * du.y * deriv.y;

    return result;
}

float noise2d(vec2 x)
{
    vec3 n = noised2d(x);
    return n.x;
}

/* ========================================================================== */
/* FBM constructions                                                          */
/* ========================================================================== */

/* Note: GLSL mat3 constructor is column-major. This C struct mimics that. */
const mat3 m3 = {{{0.00f, -0.80f, -0.60f}, {0.80f, 0.36f, -0.48f}, {-0.60f, -0.48f, 0.64f}}};
const mat3 m3i = {{{0.00f, 0.80f, 0.60f}, {-0.80f, 0.36f, -0.48f}, {-0.60f, -0.48f, 0.64f}}};
const mat2 m2 = {{{0.80f, -0.60f}, {0.60f, 0.80f}}};
const mat2 m2i = {{{0.80f, 0.60f}, {-0.60f, 0.80f}}};

float fbm_4_v2(vec2 x)
{
    float f = 1.9f;
    float s = 0.55f;
    float a = 0.0f;
    float b = 0.5f;
    int i;
    for (i = 0; i < 4; i++)
    {
        float n = noise2d(x);
        a += b * n;
        b *= s;
        x = mat2_mul_vec2(m2, x);
        x = vec2_mul_scalar(x, f);
    }
    return a;
}

float fbm_4_v3(vec3 x)
{
    float f = 2.0f;
    float s = 0.5f;
    float a = 0.0f;
    float b = 0.5f;
    int i;
    for (i = 0; i < 4; i++)
    {
        float n = noise3d(x);
        a += b * n;
        b *= s;
        x = mat3_mul_vec3(m3, x);
        x = vec3_mul_scalar(x, f);
    }
    return a;
}

vec4 fbmd_7_v3(vec3 x)
{
    float f = 1.92f;
    float s = 0.5f;
    float a = 0.0f;
    float b = 0.5f;
    vec3 d = {0.0f, 0.0f, 0.0f};
    mat3 m = {{{1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}}};
    int i;
    vec4 result;

    for (i = 0; i < 7; i++)
    {
        vec4 n = noised3d(x);
        vec3 n_yzw = {n.y, n.z, n.w};
        a += b * n.x;
        d = vec3_add(d, vec3_mul_scalar(mat3_mul_vec3(m, n_yzw), b));
        b *= s;
        x = mat3_mul_vec3(m3, x);
        x = vec3_mul_scalar(x, f);
        m = mat3_mul_mat3(m3i, m);
        m = mat3_mul_scalar(m, f);
    }
    result.x = a;
    result.y = d.x;
    result.z = d.y;
    result.w = d.z;
    return result;
}

float fbm_9_v2(vec2 x)
{
    float f = 1.9f;
    float s = 0.55f;
    float a = 0.0f;
    float b = 0.5f;
    int i;
    for (i = 0; i < 9; i++)
    {
        float n = noise2d(x);
        a += b * n;
        b *= s;
        x = mat2_mul_vec2(m2, x);
        x = vec2_mul_scalar(x, f);
    }
    return a;
}

vec3 fbmd_9_v2(vec2 x)
{
    float f = 1.9f;
    float s = 0.55f;
    float a = 0.0f;
    float b = 0.5f;
    vec2 d = {0.0f, 0.0f};
    mat2 m = {{{1.0f, 0.0f}, {0.0f, 1.0f}}};
    int i;
    vec3 result;

    for (i = 0; i < 9; i++)
    {
        vec3 n = noised2d(x);
        vec2 n_yz = {n.y, n.z};
        a += b * n.x;
        d = vec2_add(d, vec2_mul_scalar(mat2_mul_vec2(m, n_yz), b));
        b *= s;
        x = mat2_mul_vec2(m2, x);
        x = vec2_mul_scalar(x, f);
        m = mat2_mul_mat2(m2i, m);
        m = mat2_mul_scalar(m, f);
    }
    result.x = a;
    result.y = d.x;
    result.z = d.y;
    return result;
}

/* ========================================================================== */
/* Terrain functions                                                          */
/* ========================================================================== */

/**
 * @brief Generates a terrain height and a "steepness" factor.
 * @param p The 2D position on the map.
 * @return A vec2 where x is the height and y is the steepness factor [0, 1].
 */
vec2 terrainMap(vec2 p)
{
    float e, a, abs_val;
    vec2 p_scaled;
    vec2 result;

    p_scaled.x = p.x / 2000.0f + 1.0f;
    p_scaled.y = p.y / 2000.0f - 2.0f;

    e = fbm_9_v2(p_scaled);

    abs_val = e + 0.12f;
    if (abs_val < 0)
        abs_val = -abs_val;

    /* smoothstep(0.12, 0.13, abs_val) */
    a = clampf_custom((abs_val - 0.12f) / (0.13f - 0.12f), 0.0f, 1.0f);
    a = a * a * (3.0f - 2.0f * a);
    a = 1.0f - a;

    e = 600.0f * e + 600.0f;

    /* cliff: e += 90.0*smoothstep(552.0, 594.0, e) */
    a = clampf_custom((e - 552.0f) / (594.0f - 552.0f), 0.0f, 1.0f);
    e += 90.0f * (a * a * (3.0f - 2.0f * a));

    result.x = e;
    result.y = a;
    return result;
}

/**
 * @brief Generates terrain height and its analytical normal vector.
 * @param p The 2D position on the map.
 * @return A vec4 where x is the height, and yzw is the normalized normal vector.
 */
vec4 terrainMapD(vec2 p)
{
    vec3 e;
    vec2 p_scaled;
    vec2 c;
    vec3 normal;
    vec4 result;

    p_scaled.x = p.x / 2000.0f + 1.0f;
    p_scaled.y = p.y / 2000.0f - 2.0f;

    e = fbmd_9_v2(p_scaled);

    e.x = 600.0f * e.x + 600.0f;
    e.y = 600.0f * e.y;
    e.z = 600.0f * e.z;

    /* cliff with chain rule for derivative */
    c = smoothstepd_custom(550.0f, 600.0f, e.x);
    e.x = e.x + 90.0f * c.x;
    e.y = e.y + 90.0f * c.y * e.y;
    e.z = e.z + 90.0f * c.y * e.z;

    e.y /= 2000.0f;
    e.z /= 2000.0f;

    normal.x = -e.y;
    normal.y = 1.0f;
    normal.z = -e.z;
    normal = vec3_normalize(normal);

    result.x = e.x;
    result.y = normal.x;
    result.z = normal.y;
    result.w = normal.z;

    return result;
}
