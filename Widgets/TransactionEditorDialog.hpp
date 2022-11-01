#pragma once

#include <QDialog>
#include <QPushButton>

#include "Persistance/Database.hpp"

#include "ui_TransactionEditorDialog.h"

namespace Accounting::Widgets
{
    class TransactionEditorDialog final : public QDialog {
        Q_OBJECT

    public:
        TransactionEditorDialog(Persistance::TransactionObject *transaction_object, QWidget *parent = nullptr)
            : QDialog(parent)
        {
            m_ui.setupUi(this);

            if (transaction_object == nullptr) {
                setWindowTitle("New Transaction");
            } else {
                setWindowTitle(QString("Edit Transaction '%1'").arg(transaction_object->id()));
            }

            {
                m_ui.m_category_QComboBox->addItem("Groceries");
                m_ui.m_category_QComboBox->setCurrentIndex(0);

                connect(m_ui.m_category_QComboBox, &QComboBox::currentTextChanged,
                        this, &TransactionEditorDialog::slotValidate);
            }

            {
                m_ui.m_type_QComboBox->addItem("Expense");
                m_ui.m_type_QComboBox->addItem("Income");
                m_ui.m_type_QComboBox->setCurrentIndex(0);

                connect(m_ui.m_type_QComboBox, &QComboBox::currentIndexChanged,
                        this, &TransactionEditorDialog::slotValidate);
            }

            {
                auto *validator = new QDoubleValidator(this);
                validator->setBottom(0.00);
                m_ui.m_amount_QLineEdit->setValidator(validator);
                m_ui.m_amount_QLineEdit->setText("0.00");

                connect(m_ui.m_amount_QLineEdit, &QLineEdit::textChanged,
                        this, &TransactionEditorDialog::slotValidate);
            }

            {
                m_ui.m_date_QDateEdit->setDate(QDate::currentDate());

                connect(m_ui.m_date_QDateEdit, &QDateEdit::dateChanged,
                        this, &TransactionEditorDialog::slotValidate);
            }

            if (transaction_object != nullptr) {
                slotLoad(transaction_object->data());
            } else {
                slotValidate();
            }
        }

    signals:
        void signalComplete(Persistance::TransactionData);

    private slots:
        void slotLoad(Persistance::TransactionData transaction_data) {
            m_old_transaction_data = transaction_data;

            m_ui.m_category_QComboBox->setCurrentText(transaction_data.m_category);

            if (transaction_data.m_amount <= 0.00) {
                m_ui.m_type_QComboBox->setCurrentIndex(0); // Expense
            } else {
                m_ui.m_type_QComboBox->setCurrentIndex(1); // Income
            }

            m_ui.m_amount_QLineEdit->setText(QString::number(std::abs(transaction_data.m_amount), 'f', 2));

            m_ui.m_date_QDateEdit->setDate(transaction_data.m_date);

            slotValidate();
        }

        std::optional<Persistance::TransactionData> slotValidate() {
            bool is_valid = true;

            if (m_ui.m_category_QComboBox->currentText().trimmed().isEmpty()) {
                is_valid = false;
            }

            if (!m_ui.m_amount_QLineEdit->hasAcceptableInput()) {
                is_valid = false;
            }

            if (!m_ui.m_date_QDateEdit->hasAcceptableInput()) {
                is_valid = false;
            }

            m_ui.m_buttons_QDialogButtonBox->button(QDialogButtonBox::Ok)->setEnabled(is_valid);

            if (is_valid) {
                QString id;
                if (m_old_transaction_data.has_value()) {
                    id = m_old_transaction_data.value().m_id;
                } else {
                    id = generate_id();
                }

                return Persistance::TransactionData{
                    .m_id = id,
                    .m_timestamp_created = QDateTime::currentDateTimeUtc(),
                    .m_date = m_ui.m_date_QDateEdit->date(),
                    .m_amount = m_ui.m_amount_QLineEdit->text().toDouble(),
                    .m_category = m_ui.m_category_QComboBox->currentText().trimmed(),
                };
            } else {
                return std::nullopt;
            }
        }

        virtual void accept() override {
            // The "Accept" button will be disabled util the validation is successful.
            emit signalComplete(slotValidate().value());

            done(QDialog::Accepted);
        }

    private:
        Ui::TransactionEditorDialog m_ui;
        std::optional<Persistance::TransactionData> m_old_transaction_data;
    };
}
