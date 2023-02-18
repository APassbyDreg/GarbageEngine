#include "ConvexVolumn.h"

#include "Runtime/core/Log/LogSystem.h"

#define DELTA 1e-6

namespace GE
{
    ConvexVolumn ConvexVolumn::ViewFrustumFromVP(float4x4 vp)
    {
        ConvexVolumn frustum;
        // near
        frustum.Add({
            -(vp[0][3] - vp[0][2]),
            -(vp[1][3] - vp[1][2]),
            -(vp[2][3] - vp[2][2]),
            vp[3][3] - vp[3][2],
        });
        // far
        frustum.Add({-vp[0][2], -vp[1][2], -vp[2][2], vp[3][2]});
        // left
        frustum.Add({
            -(vp[0][3] + vp[0][0]),
            -(vp[1][3] + vp[1][0]),
            -(vp[2][3] + vp[2][0]),
            vp[3][3] + vp[3][0],
        });
        // right
        frustum.Add({
            -(vp[0][3] - vp[0][0]),
            -(vp[1][3] - vp[1][0]),
            -(vp[2][3] - vp[2][0]),
            vp[3][3] - vp[3][0],
        });
        // top
        frustum.Add({
            -(vp[0][3] - vp[0][1]),
            -(vp[1][3] - vp[1][1]),
            -(vp[2][3] - vp[2][1]),
            vp[3][3] - vp[3][1],
        });
        // bottom
        frustum.Add({
            -(vp[0][3] + vp[0][1]),
            -(vp[1][3] + vp[1][1]),
            -(vp[2][3] + vp[2][1]),
            vp[3][3] + vp[3][1],
        });

        return frustum;
    }

    VolumnPointRelation ConvexVolumn::TestPoint(float3 point)
    {
        bool any_on  = false;
        bool any_out = false;
        for (auto&& plane : planes)
        {
            switch (plane.TestPoint(point))
            {
                case PlanePointRelation::On:
                    any_on = true;
                    break;
                case PlanePointRelation::Front:
                    any_out = true;
                    break;
                default:
                    break;
            }
        }

        if (any_out)
            return VolumnPointRelation::Outside;
        else if (any_on)
            return VolumnPointRelation::On;
        else
            return VolumnPointRelation::Inside;
    }

    VolumnPointRelation ConvexVolumn::TestPointInclusive(float3 point)
    {
        bool any_out = false;
        for (auto&& plane : planes)
        {
            switch (plane.TestPoint(point))
            {
                case PlanePointRelation::Front:
                    any_out = true;
                    break;
                default:
                    break;
            }
        }

        if (any_out)
            return VolumnPointRelation::Outside;
        else
            return VolumnPointRelation::Inside;
    }

    VolumnPointRelation ConvexVolumn::TestPointExclusive(float3 point)
    {
        bool any_out = false;
        for (auto&& plane : planes)
        {
            switch (plane.TestPoint(point))
            {
                case PlanePointRelation::On:
                case PlanePointRelation::Front:
                    any_out = true;
                    break;
                default:
                    break;
            }
        }

        if (any_out)
            return VolumnPointRelation::Outside;
        else
            return VolumnPointRelation::Inside;
    }

    static inline float3 GetPVertex(const Bounds3f& b, const float3& normal)
    {
        float3 p = b.Max();
        if (normal.x >= 0)
            p.x = b.minX();
        if (normal.y >= 0)
            p.y = b.minY();
        if (normal.z >= 0)
            p.z = b.minZ();
        return p;
    }

    static inline float3 GetNVertex(const Bounds3f& b, const float3& normal)
    {
        float3 p = b.Min();
        if (normal.x >= 0)
            p.x = b.maxX();
        if (normal.y >= 0)
            p.y = b.maxY();
        if (normal.z >= 0)
            p.z = b.maxZ();
        return p;
    }

    // from http://www.lighthouse3d.com/tutorials/view-frustum-culling/geometric-approach-testing-boxes-ii/
    VolumnBoxRelation ConvexVolumn::TestAABBInclusive(Bounds3f aabb)
    {
        VolumnBoxRelation result = VolumnBoxRelation::Inside;
        bool              any_in, any_out;
        for (auto&& plane : planes)
        {
            auto&& normal = plane.Normal();
            if (plane.TestPointInclusive(GetPVertex(aabb, normal)) == PlanePointRelation::Front)
            {
                return VolumnBoxRelation::Outside;
            }
            else if (plane.TestPointInclusive(GetNVertex(aabb, normal)) == PlanePointRelation::Front)
            {
                result = VolumnBoxRelation::Intersect;
            }
        }
        return result;
    }
    VolumnBoxRelation ConvexVolumn::TestAABBExclusive(Bounds3f aabb)
    {
        VolumnBoxRelation result = VolumnBoxRelation::Inside;
        bool              any_in, any_out;
        for (auto&& plane : planes)
        {
            auto&& normal = plane.Normal();
            if (plane.TestPointExclusive(GetPVertex(aabb, normal)) == PlanePointRelation::Front)
            {
                return VolumnBoxRelation::Outside;
            }
            else if (plane.TestPointExclusive(GetNVertex(aabb, normal)) == PlanePointRelation::Front)
            {
                result = VolumnBoxRelation::Intersect;
            }
        }
        return result;
    }
} // namespace GE