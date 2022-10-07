#pragma once

#include "GE_pch.h"

#include "Math.h"

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

} // namespace GE