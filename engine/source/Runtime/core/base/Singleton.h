#pragma once

#include <type_traits>

namespace GE
{
    template<typename T>
    class Singleton
    {
    protected:
        Singleton() = default;

    public:
        static T& getInstance()
        {
            static T instance;
            return instance;
        }
        virtual ~Singleton() noexcept = default;
        Singleton(const Singleton&)   = delete;
        Singleton& operator=(const Singleton&) = delete;
    };
} // namespace GE
