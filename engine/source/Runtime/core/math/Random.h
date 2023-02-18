#pragma once

#include "GE_pch.h"
#include <type_traits>

namespace GE
{
    class RandomEngine
    {
    public:
        template<class T>
        requires std::is_integral_v<T> inline T RandInt(T min, T max)
        {
            std::uniform_int_distribution<T> dist {min, max};
            return dist(random_engine);
        }

        template<class T>
        requires std::is_floating_point_v<T> inline T Rand(T min, T max)
        {
            std::uniform_real_distribution<T> dist {min, max};
            return dist(random_engine);
        }

        inline int64_t RandInt() { return default_int_dist(random_engine); }
        inline double  Rand() { return default_real_dist(random_engine); }

    private:
        std::random_device                     random_device;
        std::mt19937                           random_engine {random_device()};
        std::uniform_int_distribution<int64_t> default_int_dist;
        std::uniform_real_distribution<double> default_real_dist;
    };

    namespace Math
    {
        class Random
        {
        public:
            template<class T>
            requires std::is_integral_v<T> static inline T RandInt(T min, T max)
            {
                return GetRandomEngine().RandInt(min, max);
            }
            template<class T>
            requires std::is_floating_point_v<T> static inline T Rand(T min, T max)
            {
                return GetRandomEngine().Rand(min, max);
            }

            static inline int64_t RandInt() { return GetRandomEngine().RandInt(); }
            static inline double  Rand() { return GetRandomEngine().Rand(); }

        private:
            static const Random& GetRandomEngine()
            {
                static Random rand;
                return rand;
            }
        };
    } // namespace Math
} // namespace GE