#pragma once

#include "GE_pch.h"
#include <concepts>
#include <memory>
#include <type_traits>
#include <typeindex>

namespace GE
{
    class SingletonBase
    {
    public:
        virtual ~SingletonBase() {}
        SingletonBase(int destroy_priority) : destroy_priority(destroy_priority) {}
        const int destroy_priority;
    };

    class SingletonManager
    {
    public:
        ~SingletonManager()
        {
            std::sort(m_singletons.begin(),
                      m_singletons.end(),
                      [](const std::unique_ptr<SingletonBase>& a, const std::unique_ptr<SingletonBase>& b) {
                          return a->destroy_priority < b->destroy_priority;
                      });
            while (!m_singletons.empty())
            {
                m_singletons.pop_back();
            }
        }

        template<std::derived_from<SingletonBase> T>
        static T& GetSingletonInstance()
        {
            static bool created    = false;
            static int  idx        = -1;
            auto&&      singletons = GetInstance().m_singletons;
            if (!created)
            {
                idx = singletons.size();
                singletons.emplace_back(std::make_unique<T>());
                created = true;
            }
            SingletonBase& instance = *singletons[idx];
            return dynamic_cast<T&>(instance);
        }

        static SingletonManager& GetInstance()
        {
            static SingletonManager instance;
            return instance;
        }

        std::vector<std::unique_ptr<SingletonBase>> m_singletons;
    };

    template<typename T, int DestroyPriority = 0>
    class Singleton : public SingletonBase
    {
    protected:
        Singleton() : SingletonBase(DestroyPriority) {}

    public:
        static T& GetInstance() { return SingletonManager::GetSingletonInstance<T>(); }
        virtual ~Singleton() noexcept          = default;
        Singleton(const Singleton&)            = delete;
        Singleton& operator=(const Singleton&) = delete;
    };
} // namespace GE
