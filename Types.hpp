#pragma once

#include <QString>
#include <QStringView>
#include <QRandomGenerator64>
#include <QMetaObject>
#include <QDebug>
#include <QDateTime>

namespace Accounting
{
    // This is similar to Base64, but avoids special characters and characters that could be confused by humans.
    // We don't actually need to provide a function to parse a Base58 value, since we never use it.
    inline QString to_base_58(quint64 value) {
        QStringView alphabet = u"123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz";
        Q_ASSERT(alphabet.size() == 58);

        QString result;
        while (value != 0) {
            result.append(alphabet[value % 58]);
            value /= 58;
        }

        std::reverse(result.begin(), result.end());

        return result;
    }

    struct Id {
        QString m_value;

        static Id create_random();
    };

    inline Id Id::create_random() {
        // Generate a 48 bit value.
        auto value = QRandomGenerator64::global()
            ->bounded(Q_INT64_C(1) << 48);

        return Id{ to_base_58(value) };
    }

    inline QDebug operator<<(QDebug debug, const Id& value) {
        debug.nospace() << value.m_value;
        return debug;
    }

    inline QDataStream& operator<<(QDataStream& out, const Id& value) {
        out << value.m_value;
        return out;
    }

    inline QDataStream& operator>>(QDataStream& in, Id& value) {
        in >> value.m_value;
        return in;
    }

    struct Transaction {
        Id m_id;
        std::optional<Id> m_previous_version_id;
        QDateTime m_timestamp;

        QString m_category;
        std::optional<QString> m_description;
        QDate m_date;
        qreal m_amount;
    };

    inline QDebug operator<<(QDebug debug, const Transaction& value) {
        debug.nospace() << "Transaction("
                        << "id=" << value.m_id << ", ";

        if (value.m_previous_version_id.has_value()) {
            debug.nospace() << "previous_version_id=" << value.m_previous_version_id.value() << ", ";
        } else {
            debug.nospace() << "previous_version_id=(null), ";
        }

        debug.nospace() << "date=" << value.m_date << ", ";
        debug.nospace() << "category=" << value.m_category << ", ";
        debug.nospace() << "amount=" << value.m_amount << ", ";

        if (value.m_description.has_value()) {
            debug.nospace() << "description=" << value.m_description.value() << ", ";
        } else {
            debug.nospace() << "description=(null), ";
        }

        debug.nospace() << "timestamp=" << value.m_timestamp << ", ";

        return debug;
    }

    inline QDataStream& operator<<(QDataStream& out, const Transaction& value) {
        out << value.m_id;

        if (value.m_previous_version_id.has_value()) {
            out << true;
            out << value.m_previous_version_id.value();
        } else {
            out << false;
        }

        out << value.m_timestamp;
        out << value.m_category;

        if (value.m_description.has_value()) {
            out << true;
            out << value.m_description.value();
        } else {
            out << false;
        }

        out << value.m_date;
        out << value.m_amount;

        return out;
    }

    inline QDataStream& operator>>(QDataStream& in, Transaction& value) {
        in >> value.m_id;

        bool has_previous_version_id;
        in >> has_previous_version_id;
        if (has_previous_version_id) {
            in >> value.m_previous_version_id.value();
        } else {
            value.m_previous_version_id = std::nullopt;
        }

        in >> value.m_timestamp;
        in >> value.m_category;

        bool has_description;
        in >> has_description;
        if (has_description) {
            in >> value.m_description.value();
        } else {
            value.m_description = std::nullopt;
        }

        in >> value.m_date;
        in >> value.m_amount;

        return in;
    }
}

Q_DECLARE_METATYPE(Accounting::Id);
Q_DECLARE_METATYPE(Accounting::Transaction);
