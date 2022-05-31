#pragma once

#include "GE_pch.h"

#include "core/base/Singleton.h"

namespace GE
{
    struct MsgResultBase
    {
        bool success = true;
    };

    /**
     * @brief
     *
     * @tparam MsgBody some data
     * @tparam MsgResult a subclass of MsgResultBase
     */
    template<typename MsgBody, typename MsgResult>
    class MessageDispatcher : public Singleton<MessageDispatcher>
    {
    public:
        using ListenerFn = std::function<std::shared_ptr<MsgResult>(MsgBody)>;
        // using MsgCallback = std::function<void(std::vector<std::shared_ptr<MsgResult>>)>;

        MessageDispatcher();
        ~MessageDispatcher();

        inline std::vector<std::shared_ptr<MsgResultBase>> SendMessage(MsgBody data)
        {
            std::vector<std::future<std::shared_ptr<MsgResultBase>>> rets;
            for (auto& cb : m_listeners)
            {
                rets.push_back(std::async(cb, data));
            }
            std::vector<std::shared_ptr<MsgResultBase>> res;
            for (auto& ret : rets)
            {
                res.push_back(ret.get());
            }
            return res;
        }

        inline void RegisterListener(ListenerFn listener_fn) { m_listeners.push_back(listener_fn); }

    private:
        std::vector<ListenerFn> m_listeners;

        // TODO: add async messaging
    };
} // namespace GE