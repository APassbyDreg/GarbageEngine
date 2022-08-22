#pragma once

#include "GE_pch.h"

#include "Runtime/core/ECS.h"

namespace GE
{
#define GE_SYSTEM_COMMON(sys) \
    std::string        GetName() const override { return #sys; } \
    static std::string GetNameStatic() { return #sys; }

    class SystemBase
    {
    public:
        SystemBase(entt::registry& reg, entt::entity entityID) : m_entityID(entityID), m_srcReg(reg) {}

        virtual void        OnUpdate(double dt) = 0;
        virtual std::string GetName()           = 0;

    private:
        entt::entity    m_entityID;
        entt::registry& m_srcReg;
    };
} // namespace GE