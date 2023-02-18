#pragma once

#include "GE_pch.h"

// #define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include "glm/exponential.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/norm.hpp"
#include "glm/gtx/string_cast.hpp"
#include "glm/gtx/transform.hpp"

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

    template<template<typename, glm::precision> class matType, typename T, glm::precision P, typename CharT>
    struct std::formatter<matType<T, P>, CharT> : std::formatter<std::string>
    {
        // parse is inherited from formatter<string>.
        template<typename FormatContext>
        auto format(matType<T, P> v, FormatContext& ctx) const
        {
            return std::formatter<std::string>::format(glm::to_string(v), ctx);
        }
    };

    /* --------------------------- basic concepts --------------------------- */
    template<typename T>
    concept Linear = requires(std::remove_reference_t<T> a, std::remove_reference_t<T> b)
    {
        a + b;
        a - b;
        a * 1.0;
        b * 1.0;
    };

    /* ------------------------------ basic ops ----------------------------- */
    template<std::equality_comparable T>
    inline T Clamp(T value, T min, T max)
    {
        return value < min ? min : value > max ? max : value;
    }

    template<std::equality_comparable T>
    inline T Min(T a, T b)
    {
        return a < b ? a : b;
    }

    template<std::equality_comparable T>
    inline T Max(T a, T b)
    {
        return a > b ? a : b;
    }

    template<Linear T>
    inline T Lerp(T a, T b, double t)
    {
        return a + (b - a) * t;
    }

    template<uint64 R, class T = int64>
    static inline T RoundTo(T value)
    {
        return round(value / float(R)) * T(R);
    }
} // namespace GE