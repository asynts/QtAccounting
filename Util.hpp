#pragma once

#include <algorithm>

#include <QString>
#include <QDataStream>

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
}
