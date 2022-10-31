#pragma once

#include <optional>

#include <QRandomGenerator64>
#include <QDebug>
#include <QDateTime>

#include "Util.hpp"

namespace Accounting::Persistance
{
    // FIXME: Can I move this into 'Util.hpp'?
    template <typename T>
    inline QDataStream& operator<<(QDataStream& out, const std::optional<T>& value) {
        if (value.has_value()) {
            out << true
                << value.value();
        } else {
            out << false;
        }

        return out;
    }

    // FIXME: Can I move this into 'Util.hpp'?
    template<typename T>
    inline QDataStream& operator>>(QDataStream& in, std::optional<T>& value) {
        bool has_value;
        in >> has_value;

        if (has_value) {
            T new_value;
            in >> new_value;

            value = std::move(new_value);
        }

        return in;
    }

    // FIXME: Can I move this into 'Util.hpp'?
    template<typename T>
    inline QDebug& operator<<(QDebug& debug, const std::optional<T>& value) {
        if (value.has_value()) {
            debug.nospace() << value.value();
        } else {
            debug.nospace() << "(null)";
        }

        return debug;
    }

    const quint64 DATABASE_VERSION = 0;

    struct PersistantId {
        QString m_value;

        static PersistantId create_random();

        auto operator<=>(const PersistantId&) const = default;
    };

    inline PersistantId PersistantId::create_random() {
        // Generate a 48 bit value.
        auto value = QRandomGenerator64::global()->bounded(Q_INT64_C(1) << 48);

        return PersistantId{ to_base_58(value) };
    }

    inline QDebug& operator<<(QDebug& debug, const PersistantId& value) {
        debug.nospace() << value.m_value;

        return debug;
    }

    inline QDataStream& operator<<(QDataStream& out, const PersistantId& value) {
        out << "Accounting::Persistance::PersistantId";

        out << value.m_value;

        return out;
    }

    inline QDataStream& operator>>(QDataStream& in, PersistantId& value) {
        QString prefix;
        in >> prefix;
        Q_ASSERT(prefix == "Accounting::Persistance::PersistantId");

        in >> value.m_value;

        return in;
    }

    struct QualifiedId {
        PersistantId m_id;
        int m_version;

        auto operator<=>(const QualifiedId&) const = default;
    };

    inline QDebug& operator<<(QDebug& debug, const QualifiedId& value) {
        debug.nospace() << "(" << value.m_id << ", " << value.m_version << ")";

        return debug;
    }

    inline QDataStream& operator<<(QDataStream& out, const QualifiedId &value) {
        out << "Accounting::Persistance::QualifiedId";

        out << value.m_id
            << value.m_version;

        return out;
    }

    inline QDataStream& operator>>(QDataStream& in, QualifiedId &value) {
        QString prefix;
        in >> prefix;
        Q_ASSERT(prefix == "Accounting::Persistance::QualifiedId");

        in >> value.m_id
           >> value.m_version;

        return in;
    }

    struct Transaction {
        QualifiedId m_id;
        QDateTime m_timestamp_last_modified;
        QString m_category;
        QDate m_date;
        qreal m_amount;
    };

    inline QDebug& operator<<(QDebug& debug, const Transaction& value) {
        debug.nospace() << "Transaction("
                        << "id=" << value.m_id << ", "
                        << "date=" << value.m_date << ", "
                        << "category=" << value.m_category << ", "
                        << "amount=" << value.m_amount << ", "
                        << "timestamp=" << value.m_timestamp_last_modified << ")";

        return debug;
    }

    inline QDataStream& operator<<(QDataStream& out, const Transaction& value) {
        out << "Accounting::Persistance::Transaction";

        out << value.m_id
            << value.m_timestamp_last_modified
            << value.m_category
            << value.m_date
            << value.m_amount;

        return out;
    }

    inline QDataStream& operator>>(QDataStream& in, Transaction& value) {
        QString prefix;
        in >> prefix;
        Q_ASSERT(prefix == "Accounting::Persistance::Transaction");

        in >> value.m_id
           >> value.m_timestamp_last_modified
           >> value.m_category
           >> value.m_date
           >> value.m_amount;

        return in;
    }

    struct Bill {
        QualifiedId m_id;
        QDateTime m_timestamp_last_modified;
        QDate m_date;
        QList<PersistantId> m_transaction_ids;
    };

    inline QDebug& operator<<(QDebug& debug, const Bill& value) {
        debug.nospace() << "Bill("
                        << value.m_id << ", "
                        << value.m_timestamp_last_modified << ", "
                        << value.m_date << ", "
                        << value.m_transaction_ids << ")";

        return debug;
    }

    inline QDataStream& operator<<(QDataStream& out, const Bill& value) {
        out << "Accounting::Persistance::Bill";

        out << value.m_id
            << value.m_timestamp_last_modified
            << value.m_date
            << value.m_transaction_ids;

        return out;
    }

    inline QDataStream& operator>>(QDataStream& in, Bill& value) {
        QString prefix;
        in >> prefix;
        Q_ASSERT(prefix == "Accounting::Persistance::Bill");

        in >> value.m_id
           >> value.m_timestamp_last_modified
           >> value.m_date
           >> value.m_transaction_ids;

        return in;
    }

    class Database final : public QObject {
        Q_OBJECT

    public:
        QMap<QualifiedId, Transaction> m_transactions;
        QMap<QualifiedId, Bill> m_bills;

        // Persistant ids are used to reference other entities.
        // These entities themselves are immutable but can change by being re-created with a new version.
        //
        // Qualified ids also include the version and therefore pin the entity.
        QMap<PersistantId, QualifiedId> m_version_lookup;

        std::optional<PersistantId> m_pending_bill_id;

    signals:
        void billChanged(QualifiedId new_qualified_bill_id, const Bill& bill);

        void transactionChanged(
                Persistance::QualifiedId qualified_bill_id,
                Persistance::QualifiedId new_qualified_transaction_id,
                const Persistance::Transaction& transaction);
    };

    inline QDebug& operator<<(QDebug& debug, const Database& value) {
        debug.nospace() << "Database("
                        << value.m_transactions << ", "
                        << value.m_bills << ", "
                        << value.m_version_lookup << ", "
                        << value.m_pending_bill_id << ")";

        return debug;
    }

    inline QDataStream& operator<<(QDataStream& out, const Database& value) {
        out << "Accounting::Persistance::Database";
        out << DATABASE_VERSION;

        out << value.m_transactions
            << value.m_bills
            << value.m_version_lookup
            << value.m_pending_bill_id;

        return out;
    }

    inline QDataStream& operator>>(QDataStream& in, Database& value) {
        QString prefix;
        in >> prefix;
        Q_ASSERT(prefix == "Accounting::Persistance::Database");

        qint64 database_version;
        in >> database_version;
        Q_ASSERT(database_version == DATABASE_VERSION);

        in >> value.m_transactions
           >> value.m_bills
           >> value.m_version_lookup
           >> value.m_pending_bill_id;

        return in;
    }
}

Q_DECLARE_METATYPE(Accounting::Persistance::PersistantId);
Q_DECLARE_METATYPE(Accounting::Persistance::QualifiedId);
