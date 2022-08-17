#pragma once

/* ------------------------ register components here ------------------------ */
#include "Camera.h"
#include "Tag.h"
#include "Transform.h"

namespace GE
{
    template<typename T>
    json SerializeComponent(const T& component)
    {
        return component.Serialize();
    }

    template<typename T>
    T DeserializeComponent(const json& data)
    {
        return T::Deserialize(data);
    }

    template<typename T>
    void InspectComponent(T& component)
    {
        component.Inspect();
    }
} // namespace GE