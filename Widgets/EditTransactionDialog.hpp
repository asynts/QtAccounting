#pragma once

#include <QDialog>
#include <QVBoxLayout>
#include <QLabel>
#include <QProperty>
#include <QPushButton>

#include <optional>

#include "Persistance/Database.hpp"
#include "ui_EditTransactionDialog.h"

namespace Accounting::Widgets
{
    class EditTransactionDialog final : public QDialog {
        Q_OBJECT

    public:
        EditTransactionDialog(Persistance::TransactionObject& transaction_object, QWidget *parent = nullptr)
            : QDialog(parent)
            , m_transaction_object(transaction_object)
        {
            setModal(false);

            Ui::EditTransactionDialog ui;
            ui.setupUi(this);

            m_category_widget = ui.m_category_LineEdit;
            m_date_widget = ui.m_date_DateEdit;
            m_amount_widget = ui.m_amount_LineEdit;
            m_type_widget = ui.m_type_ComboBox;
            m_buttons_widget = ui.m_buttons_DialogButtonBox;

            m_category_interpreted.setBinding([&]() -> std::optional<QString> {
                // FIXME: We do not detect this change.
                auto value = m_category_widget->text().trimmed();

                if (value.size() >= 1) {
                    return value;
                } else {
                    return std::nullopt;
                }
            });

            // FIXME: Setup bindings for remaining values.

            m_button_enabled.setBinding([&] {
                return m_category_interpreted.value().has_value();
            });

            // FIXME: I think that I should use 'subscribe' instead, but that has an unpredictable return type.
            m_button_enabled_notifier = m_button_enabled.addNotifier([&] {
                m_buttons_widget->button(QDialogButtonBox::Ok)->setEnabled(m_button_enabled.value());
            });
        }

    private slots:
        virtual void accept() override {
            // FIXME: This has already been validated, simply update the transaction object.
        }

    private:
        Persistance::TransactionObject& m_transaction_object;

        QLineEdit *m_category_widget;
        QDateEdit *m_date_widget;
        QLineEdit *m_amount_widget;
        QComboBox *m_type_widget;
        QDialogButtonBox *m_buttons_widget;

        // FIXME: Do I need to call 'Q_PROPERTY' macro for private properties?
        QProperty<std::optional<QString>> m_category_interpreted;
        QProperty<bool> m_button_enabled;
        QPropertyNotifier m_button_enabled_notifier;
    };
}
