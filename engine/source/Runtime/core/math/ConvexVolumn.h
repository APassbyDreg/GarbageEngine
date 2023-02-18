#pragma once

#include "GE_pch.h"

#include "Bounds.h"
#include "Math.h"

namespace GE
{
    enum class PlanePointRelation
    {
        Front,
        Back,
        On
    };

    // a plane represent by ax + by + cz = w, normal points to direction (a, b, c)
    // implementation mainly from unreal engine
    class GE_API Plane
    {
    public:
        /* -------------------------- constructors -------------------------- */
        Plane() : normal(1, 0, 0), offset(0) {}
        Plane(float3 n, float o)
        {
            float norm = glm::l2Norm(n);
            normal     = n / norm;
            offset     = o / norm;
        }
        Plane(float4 v)
        {
            float3 n    = float3(v);
            float  norm = glm::l2Norm(n);
            normal      = n / norm;
            offset      = v.w / norm;
        }
        Plane(float x, float y, float z, float o)
        {
            float3 n    = float3(x, y, z);
            float  norm = glm::l2Norm(n);
            normal      = n / norm;
            offset      = o / norm;
        }

        inline const float3 Normal() const { return normal; }

        inline Plane Flip() const { return Plane(-normal, -offset); }

        /* ----------------------- test against point ----------------------- */
        inline PlanePointRelation TestPoint(float3 point) const
        {
            float v = glm::dot(point, normal) - offset;
            if (v > 0)
                return PlanePointRelation::Front;
            else if (v < 0)
                return PlanePointRelation::Back;
            else
                return PlanePointRelation::On;
        }
        inline PlanePointRelation TestPointInclusive(float3 point) const
        {
            float v = glm::dot(point, normal) - offset;
            if (v > 0)
                return PlanePointRelation::Front;
            else
                return PlanePointRelation::Back;
        }
        inline PlanePointRelation TestPointExclusive(float3 point) const
        {
            float v = glm::dot(point, normal) - offset;
            if (v >= 0)
                return PlanePointRelation::Front;
            else
                return PlanePointRelation::Back;
        }

    private:
        float3 normal;
        float  offset;
    };

    enum class VolumnPointRelation
    {
        Inside,
        Outside,
        On
    };

    enum class VolumnBoxRelation
    {
        Inside,
        Outside,
        Intersect
    };

    // volumn surrounded by planes whose normal points outside
    class GE_API ConvexVolumn
    {
    public:
        inline void Add(Plane p) { planes.push_back(p); }
        inline uint NumPlanes() const { return planes.size(); }

        static ConvexVolumn ViewFrustumFromVP(float4x4 vp);

        /* ---------------------- test against a point ---------------------- */
        VolumnPointRelation TestPoint(float3 point);
        VolumnPointRelation TestPointInclusive(float3 point);
        VolumnPointRelation TestPointExclusive(float3 point);

        /* ------------------------ test against aabb ----------------------- */
        VolumnBoxRelation TestAABBInclusive(Bounds3f aabb);
        VolumnBoxRelation TestAABBExclusive(Bounds3f aabb);

    private:
        std::vector<Plane> planes = {};
    };
} // namespace GE