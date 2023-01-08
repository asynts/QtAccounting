#pragma once

#include <algorithm>

#include <QString>
#include <QRandomGenerator64>
#include <QWidget>
#include <QComboBox>
#include <QMetaEnum>

namespace Accounting
{
    constexpr const char *ACCOUNTING_ALLOCATION_TAG = "Accounting";

    // Credit: https://github.com/skeeto/hash-prospector#three-round-functions
    inline quint32 hash_triple32(quint32 x)
    {
        x ^= x >> 17;
        x *= 0xed5ad4bb;
        x ^= x >> 11;
        x *= 0xac4c1b51;
        x ^= x >> 15;
        x *= 0x31848bab;
        x ^= x >> 14;
        return x;
    }
    inline quint32 hash_triple32_inverse(quint32 x)
    {
        x ^= x >> 14 ^ x >> 28;
        x *= 0x32b21703;
        x ^= x >> 15 ^ x >> 30;
        x *= 0x469e0db1;
        x ^= x >> 11 ^ x >> 22;
        x *= 0x79a85073;
        x ^= x >> 17;
        return x;
    }

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

    // Ids must be generated in sequence to avoid collisions.
    inline QString hash_and_stringify_id(quint32 sequential_id) {
        // We hash the value to ensure that it looks random.
        auto hashed_id = hash_triple32(sequential_id);

        // We must not truncate the value like I did before, otherwise, collisions could occur.

        // Use Base58 to turn into a relatively short string.
        return to_base_58(hashed_id);
    }

    // This is useful for debugging.
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

    template<typename EnumType>
    QString enum_type_to_string(int value) {
        return enum_type_to_string<EnumType>(static_cast<EnumType>(value));
    }

    class Promise {
    public:
        explicit Promise(std::function<void()> fulfill, std::function<void()> reject)
            : m_fulfill(fulfill)
            , m_reject(reject) { }

        void fulfill()
        {
            Q_ASSERT(m_fulfill != nullptr);
            m_fulfill();
            m_fulfill = nullptr;
        }

        void reject()
        {
            Q_ASSERT(m_reject != nullptr);
            m_reject();
            m_reject = nullptr;
        }

    private:
        std::function<void()> m_fulfill;
        std::function<void()> m_reject;
    };
}
