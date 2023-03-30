#pragma once

#include "GE_pch.h"

namespace GE
{
    template<typename T>
    class Serializable
    {
    public:
        virtual T    Serialize() const          = 0;
        virtual void Deserialize(const T& data) = 0;
    };
} // namespace GE