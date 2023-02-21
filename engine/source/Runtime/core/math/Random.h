#pragma once

#include "GE_pch.h"
#include <limits>
#include <type_traits>

namespace GE
{
    class RandomEngine
    {
    public:
        template<class T>
        requires std::is_integral_v<T> inline T RandInt(T min = std::numeric_limits<T>::min(),
                                                        T max = std::numeric_limits<T>::max())
        {
            std::uniform_int_distribution<T> dist {min, max};
            return dist(random_engine);
        }

        template<class T>
        requires std::is_floating_point_v<T> inline T Rand(T min = 0.0, T max = 1.0)
        {
            std::uniform_real_distribution<T> dist {min, max};
            return dist(random_engine);
        }

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
            requires std::is_integral_v<T> static inline T RandInt(T min = std::numeric_limits<T>::min(),
                                                                   T max = std::numeric_limits<T>::max())
            {
                return GetRandomEngine().RandInt(min, max);
            }
            template<class T>
            requires std::is_floating_point_v<T> static inline T Rand(T min = 0.0, T max = 1.0)
            {
                return GetRandomEngine().Rand(min, max);
            }

        private:
            static RandomEngine& GetRandomEngine()
            {
                static RandomEngine rand;
                return rand;
            }
        };
    } // namespace Math
} // namespace GE