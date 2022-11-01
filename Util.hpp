#pragma once

#include <algorithm>

#include <QString>
#include <QRandomGenerator64>
#include <QWidget>

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

    inline QString generate_id() {
        // Generate 48 bit value.
        auto value = QRandomGenerator64::global()->bounded(Q_INT64_C(1) << 48);

        return to_base_58(value);
    }

    // This is useful for background.
    inline void debug_set_widget_background_red(QWidget *widget) {
        QPalette palette;
        palette.setColor(QPalette::Window, Qt::red);
        widget->setAutoFillBackground(true);
        widget->setPalette(palette);
    }
}
