#pragma once

#include "GE_pch.h"

#include "Math.h"

#include "Transforms.h"

namespace GE
{
#define GETTER(selector, dim_name, dim_idx) \
    inline TBase& selector##dim_name() \
    { \
        if constexpr (dim > dim_idx) \
        { \
            return *((TBase*)&selector##imuns + dim_idx); \
        } \
        else \
        { \
            return 0; \
        } \
    }

    template<int dim>
    class TransformTypeOf
    {
        using ttype = float4x4;
    };

    template<typename TStorage, typename TBase, int dim>
    class Bounds
    {
    public:
        /* -------------------------- constructors -------------------------- */

        Bounds() : isvalid(false) {}
        Bounds(TStorage point) : minimuns(point), maximuns(point), isvalid(true) {}
        Bounds(TStorage vmin, TStorage vmax) : minimuns(vmin), maximuns(vmax) { RefreshState(); }

        /* -------------------------- item getters -------------------------- */

        GETTER(min, X, 0);
        GETTER(min, Y, 1);
        GETTER(min, Z, 2);
        GETTER(max, X, 0);
        GETTER(max, Y, 1);
        GETTER(max, Z, 2);
        TStorage& Min() { return minimuns; }
        TStorage& Max() { return maximuns; }

        /* ---------------------------- operators --------------------------- */
        // Union
        inline Bounds  operator+(Bounds&& rhs) { return {min(minimuns, rhs.minimuns), max(maximuns, rhs.maximuns)}; }
        inline Bounds& operator+=(Bounds&& rhs)
        {
            minimuns = min(minimuns, rhs.minimuns);
            maximuns = max(maximuns, rhs.maximuns);
            RefreshState();
            return *this;
        }
        // Intersection
        inline Bounds operator^(Bounds&& rhs) { return {max(minimuns, rhs.minimuns), min(maximuns, rhs.maximuns)}; }
        inline Bounds operator^=(Bounds&& rhs)
        {
            minimuns = max(minimuns, rhs.minimuns);
            maximuns = min(maximuns, rhs.maximuns);
            RefreshState();
            return *this;
        }

        /* ---------------------- additional infomation --------------------- */

        inline bool IsValid() { return isvalid; }
        inline void RefreshState() { CheckValid(); }

    private:
        TStorage minimuns, maximuns;
        bool     isvalid = false;

    private:
        inline void CheckValid() { isvalid = min(minimuns, maximuns) == minimuns; }
    };

#undef GETTER

    typedef Bounds<float3, float, 3> Bounds3f;
    typedef Bounds<float2, float, 2> Bounds2f;
    typedef Bounds<int3, int, 3>     Bounds3i;
    typedef Bounds<int2, int, 2>     Bounds2i;

    inline Bounds3f Transform(Bounds3f box, float4x4 mat)
    {
        const std::vector<float3> directions = {
            {1, 0, 0}, {0, 1, 0}, {0, 0, 1}, {1, 1, 0}, {1, 0, 1}, {0, 1, 1}, {1, 1, 1}};
        const float3 v      = box.Max() - box.Min();
        const float3 b      = box.Min();
        Bounds3f     result = Bounds3f(Math::HomogeneousTransform(b, mat));
        for (auto&& dir : directions)
        {
            result += Math::HomogeneousTransform(b + v * dir, mat);
        }
        return result;
    }

    inline Bounds2f Transform(Bounds2f box, float3x3 mat)
    {
        const std::vector<float2> directions = {{1, 0}, {0, 1}, {1, 1}};
        const float2              v          = box.Max() - box.Min();
        const float2              b          = box.Min();
        Bounds2f                  result     = Bounds2f(Math::HomogeneousTransform(b, mat));
        for (auto&& dir : directions)
        {
            result += Math::HomogeneousTransform(b + v * dir, mat);
        }
        return result;
    }

    template<typename TBounds, typename TTransform>
    class TransformedBounds
    {
    public:
        TBounds    bounds;
        TTransform transform;

        operator TBounds() const { return Transform(bounds, transform); }

        inline TBounds GetMergedBounds() { return Transform(bounds, transform); }
    };

    typedef TransformedBounds<Bounds3f, float4x4> TransformedBounds3f;
    typedef TransformedBounds<Bounds2f, float3x3> TransformedBounds2f;
} // namespace GE