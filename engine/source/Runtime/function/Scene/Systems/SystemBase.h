#pragma once

#include "GE_pch.h"

#include "Runtime/core/ECS.h"

namespace GE
{
    class Entity;

#define GE_SYSTEM_COMMON(sys) \
    std::string        GetName() const override { return #sys; } \
    static std::string GetNameStatic() { return #sys; } \
    sys(std::shared_ptr<Entity> e) : m_entity(e) {}

    class SystemBase
    {
    public:
        SystemBase(std::shared_ptr<Entity> e) : m_entity(e) {}

        virtual void OnUpdate(double dt) {};
        virtual void OnEvent() {} // TODO: create scene event syetem

        virtual std::string GetName() = 0;

    private:
        std::shared_ptr<Entity> m_entity;
    };
} // namespace GE