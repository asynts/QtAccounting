#pragma once

#include <QEvent>

#include "Util.hpp"

namespace Accounting
{
    class FutureReadyEvent : public QEvent {
    public:
        explicit FutureReadyEvent(ResultEnum result)
            : QEvent(FutureReadyEvent::Type)
            , m_result(result) { }

        static constexpr QEvent::Type Type = static_cast<QEvent::Type>(QEvent::User + 1);

        ResultEnum m_result;
    };
}
