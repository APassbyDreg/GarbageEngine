#pragma once

#include "GE_pch.h"

#include "glm/glm.hpp"

namespace GE
{
    /* ----------------------------- GLM aliases ---------------------------- */
    typedef glm::vec2 float2;
    typedef glm::vec3 float3;
    typedef glm::vec4 float4;

    typedef glm::ivec2 int2;
    typedef glm::ivec3 int3;
    typedef glm::ivec4 int4;

    typedef glm::uvec2 uint2;
    typedef glm::uvec3 uint3;
    typedef glm::uvec4 uint4;

    typedef glm::dvec2 double2;
    typedef glm::dvec3 double3;
    typedef glm::dvec4 double4;

    typedef glm::mat2 float2x2;
    typedef glm::mat3 float3x3;
    typedef glm::mat4 float4x4;

    typedef glm::quat quat;

    /* ------------------------------- aliases ------------------------------ */
    typedef unsigned char byte;

    typedef unsigned char  uchar;
    typedef unsigned short ushort;
    typedef unsigned int   uint;

    typedef unsigned char      uint8;
    typedef unsigned short     uint16;
    typedef unsigned int       uint32;
    typedef unsigned long long uint64;

    typedef char      int8;
    typedef short     int16;
    typedef int       int32;
    typedef long long int64;

} // namespace GE