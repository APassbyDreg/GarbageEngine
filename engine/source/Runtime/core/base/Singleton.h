#pragma once

#include "GE_pch.h"
#include <concepts>
#include <cstddef>
#include <memory>
#include <type_traits>
#include <typeindex>

namespace GE
{
    class SingletonBase
    {
    public:
        virtual ~SingletonBase() = default;
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
                      [](const std::shared_ptr<SingletonBase>& a, const std::shared_ptr<SingletonBase>& b) {
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
            static std::shared_ptr<T> strong_instance = std::make_shared<T>();
            static std::weak_ptr<T>   weak_instance   = strong_instance;

            auto&& singletons = GetInstance().m_singletons;
            if (strong_instance != nullptr)
            {
                singletons.emplace_back(strong_instance);
                strong_instance = nullptr;
            }

            return dynamic_cast<T&>(*(weak_instance.lock()));
        }

        static SingletonManager& GetInstance()
        {
            static SingletonManager instance;
            return instance;
        }

        std::vector<std::shared_ptr<SingletonBase>> m_singletons;
    };

    template<typename T, int DestroyPriority = 0>
    class Singleton : public SingletonBase
    {
    protected:
        static bool s_isAlive;

    public:
        Singleton() : SingletonBase(DestroyPriority) { s_isAlive = true; }
        virtual ~Singleton() { s_isAlive = false; }

        static T&   GetInstance() { return SingletonManager::GetSingletonInstance<T>(); }
        static bool IsAlive() { return s_isAlive; }

        Singleton(const Singleton&)            = delete;
        Singleton& operator=(const Singleton&) = delete;
    };

    template<typename T, int DestroyPriority>
    bool Singleton<T, DestroyPriority>::s_isAlive = false;
} // namespace GE
