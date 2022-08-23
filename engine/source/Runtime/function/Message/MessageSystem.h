#pragma once

#include "GE_pch.h"

#include "MsgTypes.h"

#include "Runtime/core/Base/Singleton.h"

namespace GE
{

    /**
     * @brief
     *
     * @tparam MsgBody some data
     * @tparam MsgResult a subclass of MsgResultBase
     */
    template<typename MsgBody, typename MsgResult>
    class GE_API MessageDispatcher : public Singleton<MessageDispatcher<MsgBody, MsgResult>>
    {
    public:
        using ListenerFn = std::function<std::shared_ptr<MsgResult>(MsgBody)>;
        // using MsgCallback = std::function<void(std::vector<std::shared_ptr<MsgResult>>)>;

        MessageDispatcher() {};
        ~MessageDispatcher() {};

        inline std::vector<std::shared_ptr<MsgResultBase>> Dispatch(MsgBody data)
        {
            auto                                        rets = DispatchNoRetrun(data);
            std::vector<std::shared_ptr<MsgResultBase>> res  = {};
            for (auto& ret : rets)
            {
                res.push_back(ret.get());
            }
            return res;
        }

        inline std::vector<std::future<std::shared_ptr<MsgResultBase>>> DispatchNoRetrun(MsgBody data)
        {
            std::vector<std::future<std::shared_ptr<MsgResultBase>>> rets;
            for (auto& cb : m_listeners)
            {
                rets.push_back(std::async(cb, data));
            }
            return rets;
        }

        inline void RegisterListener(ListenerFn listener_fn, int priority = 0)
        {
            uint offset = std::lower_bound(m_priorities.begin(), m_priorities.end(), priority) - m_priorities.begin();

            m_listeners.insert(m_listeners.begin() + offset, listener_fn);
            m_priorities.insert(m_priorities.begin() + offset, priority);
        }

    private:
        std::vector<ListenerFn> m_listeners;
        std::vector<int>        m_priorities;
    };
} // namespace GE