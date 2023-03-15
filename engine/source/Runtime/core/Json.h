#pragma once

#include "GE_pch.h"

#include "Math/Math.h"

#include "nlohmann/json.hpp"

namespace GE
{
    using json = nlohmann::json;

    namespace JsonUtils
    {
        static float3 LoadFloat3(const json& data)
        {
            return {data[0].get<float>(), data[1].get<float>(), data[2].get<float>()};
        }

        static json DumpFloat3(float3 v)
        {
            json data;
            data[0] = v.x;
            data[1] = v.y;
            data[2] = v.z;
            return data;
        }

        static float4 LoadFloat4(const json& data)
        {
            return {data[0].get<float>(), data[1].get<float>(), data[2].get<float>(), data[3].get<float>()};
        }

        static json DumpFloat4(float4 v)
        {
            json data;
            data[0] = v.x;
            data[1] = v.y;
            data[2] = v.z;
            data[3] = v.w;
            return data;
        }
    } // namespace JsonUtils
} // namespace GE