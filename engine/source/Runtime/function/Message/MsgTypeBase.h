#pragma once

#include "GE_pch.h"

namespace GE
{
    class MsgResultBase
    {
    public:
        MsgResultBase(bool _success) : success(_success) {}
        bool success = true;

        static std::shared_ptr<MsgResultBase> Success()
        {
            static std::shared_ptr<MsgResultBase> result = std::make_shared<MsgResultBase>(true);
            return result;
        }
        static std::shared_ptr<MsgResultBase> Fail()
        {
            static std::shared_ptr<MsgResultBase> result = std::make_shared<MsgResultBase>(false);
            return result;
        }
    };

    class MsgBodyBase
    {
    public:
        virtual std::string getName() const { return "GE_BASIC_MSG"; }
        virtual std::string toString() const { return ""; }
    };
} // namespace GE

#define GE_MSG_BODY_COMMON_IMPL(classname) \
    virtual std::string getName() const override { return #classname; }