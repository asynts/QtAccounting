#pragma once

#include <QString>
#include <QStringView>
#include <QRandomGenerator64>
#include <QMetaObject>
#include <QDebug>

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

    class Id {
    public:
        const QString m_value;

        static Id create_random();
    };

    inline Id Id::create_random() {
        // Generate a 48 bit value.
        auto value = QRandomGenerator64::global()
            ->bounded(Q_INT64_C(1) << 48);

        return Id{ to_base_58(value) };
    }

    inline QDebug operator<<(QDebug debug, const Id &value) {
        debug.nospace() << value.m_value;
        return debug;
    }
}

Q_DECLARE_METATYPE(Accounting::Id);
