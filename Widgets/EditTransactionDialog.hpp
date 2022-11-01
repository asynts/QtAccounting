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
        EditTransactionDialog(Persistance::TransactionData transaction_data, QWidget *parent = nullptr)
            : QDialog(parent)
            , m_old_transaction_data(transaction_data)
        {
            setModal(false);

            Ui::EditTransactionDialog ui;
            ui.setupUi(this);

            m_category_widget = ui.m_category_LineEdit;
            m_category_widget->setText(transaction_data.m_category);

            m_date_widget = ui.m_date_DateEdit;
            m_date_widget->setDate(transaction_data.m_date);

            m_amount_widget = ui.m_amount_LineEdit;
            auto amount_validator = new QDoubleValidator(this);
            amount_validator->setBottom(0.00);
            m_amount_widget->setValidator(amount_validator);
            m_amount_widget->setText(QString::number(std::abs(transaction_data.m_amount), 'f', 2));

            m_type_widget = ui.m_type_ComboBox;
            m_type_widget->addItem("Expense");
            m_type_widget->addItem("Income");
            if (transaction_data.m_amount < 0) {
                m_type_widget->setCurrentIndex(0);
            } else {
                m_type_widget->setCurrentIndex(1);
            }

            m_buttons_widget = ui.m_buttons_DialogButtonBox;

            validate();

            connect(m_category_widget, &QLineEdit::textChanged,
                    this, &EditTransactionDialog::validate);

            connect(m_date_widget, &QDateEdit::dateChanged,
                    this, &EditTransactionDialog::validate);

            connect(m_amount_widget, &QLineEdit::textChanged,
                    this, &EditTransactionDialog::validate);

            connect(m_type_widget, &QComboBox::currentIndexChanged,
                    this, &EditTransactionDialog::validate);
        }

    private slots:
        void validate() {
            bool is_valid = true;

            auto category = m_category_widget->text().trimmed();
            if (category.size() == 0) {
                is_valid = false;
            }

            if (!m_amount_widget->hasAcceptableInput()) {
                is_valid = false;
            }

            m_buttons_widget->button(QDialogButtonBox::Ok)->setEnabled(is_valid);
        }

        virtual void accept() override {
            qreal amount = m_amount_widget->text().trimmed().toDouble();

            if (m_type_widget->currentIndex() == 0) {
                amount *= -1;
            }

            Persistance::TransactionData new_transaction_data = m_old_transaction_data;
            new_transaction_data.m_amount = amount;
            new_transaction_data.m_timestamp_created = QDateTime::currentDateTimeUtc();
            new_transaction_data.m_category = m_category_widget->text().trimmed();
            emit onComplete(new_transaction_data);

            done(QDialog::Accepted);
        }

    signals:
        void onComplete(Persistance::TransactionData transaction_data);

    private:
        Persistance::TransactionData m_old_transaction_data;

        QLineEdit *m_category_widget;
        QDateEdit *m_date_widget;
        QLineEdit *m_amount_widget;
        QComboBox *m_type_widget;
        QDialogButtonBox *m_buttons_widget;
    };
}
