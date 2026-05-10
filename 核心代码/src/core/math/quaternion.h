/**
 * 四元数 - 3D旋转
 */

#ifndef XUANYIN_QUATERNION_H
#define XUANYIN_QUATERNION_H

#include "vec3.h"

typedef struct Quaternion {
    float x, y, z, w;
} Quaternion;

#define QUAT_IDENTITY ((Quaternion){0.0f, 0.0f, 0.0f, 1.0f})

// 四元数运算函数声明
Quaternion quat_from_axis_angle(Vec3 axis, float angle);
Quaternion quat_from_euler(float pitch, float yaw, float roll);
Quaternion quat_mul(Quaternion a, Quaternion b);
Quaternion quat_normalize(Quaternion q);
Vec3 quat_rotate(Quaternion q, Vec3 v);
Quaternion quat_slerp(Quaternion a, Quaternion b, float t);

#endif // XUANYIN_QUATERNION_H