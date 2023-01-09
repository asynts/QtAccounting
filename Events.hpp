#pragma once

#include <QEvent>

#include "Util.hpp"
#include "Persistance/Database.hpp"

namespace Accounting
{
    class FutureReadyEvent : public QEvent {
    public:
        explicit FutureReadyEvent(ResultEnum result)
            : QEvent(Type)
            , m_result(result) { }

        static constexpr QEvent::Type Type = static_cast<QEvent::Type>(QEvent::User + 1);

        ResultEnum m_result;
    };

    class DatabaseLoadedEvent : public QEvent {
    public:
        explicit DatabaseLoadedEvent(Persistance::Database&& database)
            : QEvent(Type)
            , m_database(std::move(database)) { }

        static constexpr QEvent::Type Type = static_cast<QEvent::Type>(QEvent::User + 2);

        Persistance::Database m_database;
    };
}
