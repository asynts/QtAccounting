#pragma once

#include <algorithm>

#include <QString>
#include <QRandomGenerator64>
#include <QWidget>
#include <QComboBox>
#include <QMetaEnum>

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

    // FIXME: Rename to 'generate_random_id'.
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

    // This assumes that the enum is 0-indexed.
    template<typename EnumType>
    void fill_QComboBox_with_enum(QComboBox *combo_box) {
        combo_box->clear();

        auto meta_enum = QMetaEnum::fromType<EnumType>();
        for (int key = 0; key < meta_enum.keyCount(); ++key) {
            combo_box->addItem(meta_enum.key(key));
        }
    }

    template<typename EnumType>
    EnumType enum_type_from_string(QString key) {
        bool ok;
        auto value = QMetaEnum::fromType<EnumType>().keyToValue(key.toUtf8(), &ok);
        Q_ASSERT(ok);

        return static_cast<EnumType>(value);
    }

    template<typename EnumType>
    QString enum_type_to_string(EnumType value) {
        return QMetaEnum::fromType<EnumType>().valueToKey(static_cast<int>(value));
    }

}
