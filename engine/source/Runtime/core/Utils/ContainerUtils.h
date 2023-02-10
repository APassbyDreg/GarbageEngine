#pragma once

#include "GE_pch.h"
#include <ranges>

namespace GE
{
    namespace StdUtils
    {
        template<class T, class TFilter>
        std::vector<T> Filter(std::vector<T>& vec, TFilter cond)
        {
            namespace views = std::ranges::views;
            auto&& values   = vec | views::filter(cond);
            return {values.begin(), values.end()};
        }

        template<class TKey, class TVal, class TFilter>
        std::map<TKey, TVal> FilterKey(std::map<TKey, TVal>& map, TFilter cond)
        {
            namespace views = std::ranges::views;
            auto&& values   = map | views::filter([&](std::pair<TKey, TVal> p) { return cond(p.first); });
            return {values.begin(), values.end()};
        }

        template<class T, class TIdentity>
        void Sort(std::vector<T>& vec, TIdentity identity)
        {
            std::sort(vec.begin(), vec.end(), [=](T& lhs, T& rhs) { return identity(lhs) < identity(rhs); });
        }

        template<class TKey, class TVal>
        bool Exists(std::map<TKey, TVal>& map, TKey key)
        {
            return map.find(key) != map.end();
        }

        template<class T>
        std::vector<T> Merge(std::vector<T>& a, std::vector<T>& b)
        {
            std::vector<T> vec;
            std::merge(a.begin(), a.end(), b.begin(), b.end(), std::back_inserter(vec));
            return vec;
        }

        template<class T>
        void MergeTo(std::vector<T>& dst, std::vector<T>& src)
        {
            dst.reserve(src.size() + dst.size());
            dst.insert(dst.end(), src.begin(), src.end());
        }
    } // namespace StdUtils
} // namespace GE