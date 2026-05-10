/**
 * 三维向量 - 国风游戏专用数学库
 * 支持SSE/AVX加速运算
 */

#ifndef XUANYIN_VEC3_H
#define XUANYIN_VEC3_H

#include <stdint.h>
#include <math.h>

#ifdef __SSE__
#include <xmmintrin.h>
#endif

#ifdef __AVX__
#include <immintrin.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

// 三维向量结构体
typedef struct Vec3 {
    union {
        struct {
            float x, y, z;
        };
        float data[3];
#ifdef __SSE__
        __m128 sse;  // SSE加速
#endif
    };
} Vec3;

// 常量定义
#define VEC3_ZERO ((Vec3){0.0f, 0.0f, 0.0f})
#define VEC3_ONE ((Vec3){1.0f, 1.0f, 1.0f})
#define VEC3_UP ((Vec3){0.0f, 1.0f, 0.0f})
#define VEC3_RIGHT ((Vec3){1.0f, 0.0f, 0.0f})
#define VEC3_FORWARD ((Vec3){0.0f, 0.0f, 1.0f})

// ========== 基本运算 ==========

// 创建向量
static inline Vec3 vec3_create(float x, float y, float z) {
    Vec3 v = {x, y, z};
    return v;
}

// 向量加法
static inline Vec3 vec3_add(Vec3 a, Vec3 b) {
#ifdef __SSE__
    __m128 va = _mm_loadu_ps(a.data);
    __m128 vb = _mm_loadu_ps(b.data);
    __m128 result = _mm_add_ps(va, vb);
    Vec3 v;
    _mm_storeu_ps(v.data, result);
    v.data[3] = 0.0f;  // 清除第4个分量
    return v;
#else
    Vec3 v = {a.x + b.x, a.y + b.y, a.z + b.z};
    return v;
#endif
}

// 向量减法
static inline Vec3 vec3_sub(Vec3 a, Vec3 b) {
#ifdef __SSE__
    __m128 va = _mm_loadu_ps(a.data);
    __m128 vb = _mm_loadu_ps(b.data);
    __m128 result = _mm_sub_ps(va, vb);
    Vec3 v;
    _mm_storeu_ps(v.data, result);
    v.data[3] = 0.0f;
    return v;
#else
    Vec3 v = {a.x - b.x, a.y - b.y, a.z - b.z};
    return v;
#endif
}

// 向量点乘
static inline float vec3_dot(Vec3 a, Vec3 b) {
#ifdef __SSE3__
    __m128 va = _mm_loadu_ps(a.data);
    __m128 vb = _mm_loadu_ps(b.data);
    __m128 mul = _mm_mul_ps(va, vb);
    __m128 shuf = _mm_movehdup_ps(mul);
    __m128 sums = _mm_add_ps(mul, shuf);
    shuf = _mm_movehl_ps(shuf, sums);
    sums = _mm_add_ss(sums, shuf);
    float result;
    _mm_store_ss(&result, sums);
    return result;
#else
    return a.x * b.x + a.y * b.y + a.z * b.z;
#endif
}

// 向量叉乘
static inline Vec3 vec3_cross(Vec3 a, Vec3 b) {
    Vec3 v = {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
    return v;
}

// 向量缩放
static inline Vec3 vec3_scale(Vec3 v, float s) {
#ifdef __SSE__
    __m128 vv = _mm_loadu_ps(v.data);
    __m128 vs = _mm_set1_ps(s);
    __m128 result = _mm_mul_ps(vv, vs);
    Vec3 r;
    _mm_storeu_ps(r.data, result);
    r.data[3] = 0.0f;
    return r;
#else
    Vec3 r = {v.x * s, v.y * s, v.z * s};
    return r;
#endif
}

// 向量长度平方
static inline float vec3_length_sq(Vec3 v) {
    return vec3_dot(v, v);
}

// 向量长度
static inline float vec3_length(Vec3 v) {
    return sqrtf(vec3_length_sq(v));
}

// 向量归一化
static inline Vec3 vec3_normalize(Vec3 v) {
    float len = vec3_length(v);
    if (len > 0.000001f) {
        float inv_len = 1.0f / len;
        return vec3_scale(v, inv_len);
    }
    return VEC3_ZERO;
}

// 向量距离
static inline float vec3_distance(Vec3 a, Vec3 b) {
    Vec3 diff = vec3_sub(a, b);
    return vec3_length(diff);
}

// 线性插值
static inline Vec3 vec3_lerp(Vec3 a, Vec3 b, float t) {
    t = t < 0.0f ? 0.0f : (t > 1.0f ? 1.0f : t);
    Vec3 v = {
        a.x + (b.x - a.x) * t,
        a.y + (b.y - a.y) * t,
        a.z + (b.z - a.z) * t
    };
    return v;
}

// 球面线性插值
Vec3 vec3_slerp(Vec3 a, Vec3 b, float t);

// 向量反射
Vec3 vec3_reflect(Vec3 v, Vec3 n);

// 向量折射
Vec3 vec3_refract(Vec3 v, Vec3 n, float eta);

// 检查向量是否近似相等
static inline int vec3_nearly_equal(Vec3 a, Vec3 b, float epsilon) {
    float dx = fabsf(a.x - b.x);
    float dy = fabsf(a.y - b.y);
    float dz = fabsf(a.z - b.z);
    return (dx <= epsilon && dy <= epsilon && dz <= epsilon);
}

// 向量最大值
static inline Vec3 vec3_max(Vec3 a, Vec3 b) {
    Vec3 v = {
        a.x > b.x ? a.x : b.x,
        a.y > b.y ? a.y : b.y,
        a.z > b.z ? a.z : b.z
    };
    return v;
}

// 向量最小值
static inline Vec3 vec3_min(Vec3 a, Vec3 b) {
    Vec3 v = {
        a.x < b.x ? a.x : b.x,
        a.y < b.y ? a.y : b.y,
        a.z < b.z ? a.z : b.z
    };
    return v;
}

// 向量绝对值
static inline Vec3 vec3_abs(Vec3 v) {
    Vec3 r = {
        fabsf(v.x),
        fabsf(v.y),
        fabsf(v.z)
    };
    return r;
}

// 向量与标量比较
static inline Vec3 vec3_gt(Vec3 a, Vec3 b) {
    Vec3 v = {
        a.x > b.x ? 1.0f : 0.0f,
        a.y > b.y ? 1.0f : 0.0f,
        a.z > b.z ? 1.0f : 0.0f
    };
    return v;
}

// 向量分量相乘
static inline Vec3 vec3_mul(Vec3 a, Vec3 b) {
    Vec3 v = {a.x * b.x, a.y * b.y, a.z * b.z};
    return v;
}

// 打印向量
static inline void vec3_print(const char* name, Vec3 v) {
    printf("%s: (%.3f, %.3f, %.3f)\n", name, v.x, v.y, v.z);
}

#ifdef __cplusplus
}
#endif

#endif // XUANYIN_VEC3_H