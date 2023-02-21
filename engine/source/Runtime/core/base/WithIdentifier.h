#pragma once

#include "GE_pch.h"

#include "Singleton.h"

#include "../Math/Math.h"
#include "../Math/Random.h"

namespace GE
{
    using UniqueIdentifier = uint64;

    class IdentifierManager : public Singleton<IdentifierManager>
    {
    public:
        static UniqueIdentifier GetUnusedID()
        {
            UniqueIdentifier id = Math::Random::RandInt<uint64>();
            while (GetInstance().used_ids.find(id) != GetInstance().used_ids.end())
            {
                id = Math::Random::RandInt<uint64>();
            }
            GetInstance().used_ids.insert(id);
            return id;
        }

        static bool HasID(UniqueIdentifier id)
        {
            return GetInstance().used_ids.find(id) != GetInstance().used_ids.end();
        }

        static void RemoveID(UniqueIdentifier id)
        {
            if (HasID(id))
            {
                GetInstance().used_ids.erase(id);
            }
        }

    private:
        std::set<UniqueIdentifier> used_ids;
    };

    class WithIdentifier
    {
    public:
        WithIdentifier() { id = IdentifierManager::GetUnusedID(); }
        ~WithIdentifier() { IdentifierManager::RemoveID(id); }

        const UniqueIdentifier& GetID() const { return id; }

    private:
        UniqueIdentifier id;
    };
} // namespace GE