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
        explicit TransactionEditorDialog(Persistance::BillObject *parent_bill_object, Persistance::TransactionObject *transaction_object, QWidget *parent = nullptr)
            : QDialog(parent)
            , m_old_transaction_object(transaction_object)
            , m_parent_bill_object(parent_bill_object)
        {
            m_ui.setupUi(this);

            if (transaction_object == nullptr) {
                setWindowTitle("New Transaction");
            } else {
                setWindowTitle(QString("Edit Transaction '%1'").arg(transaction_object->id()));
            }

            {
                m_ui.m_category_QComboBox->addItem("Groceries");

                if (transaction_object == nullptr) {
                    m_ui.m_category_QComboBox->setCurrentIndex(0);
                } else {
                    m_ui.m_category_QComboBox->setCurrentText(transaction_object->category());
                }

                connect(m_ui.m_category_QComboBox, &QComboBox::currentTextChanged,
                        this, &TransactionEditorDialog::slotValidate);
            }

            {
                m_ui.m_type_QComboBox->addItem("Expense");
                m_ui.m_type_QComboBox->addItem("Income");

                if (transaction_object == nullptr) {
                    m_ui.m_type_QComboBox->setCurrentIndex(0); // Expense
                } else if (transaction_object->amount() <= 0.00){
                    m_ui.m_type_QComboBox->setCurrentIndex(0); // Expense
                } else {
                    m_ui.m_type_QComboBox->setCurrentIndex(1); // Income
                }

                connect(m_ui.m_type_QComboBox, &QComboBox::currentIndexChanged,
                        this, &TransactionEditorDialog::slotValidate);
            }

            {
                auto *validator = new QDoubleValidator(this);
                validator->setBottom(0.00);
                m_ui.m_amount_QLineEdit->setValidator(validator);

                if (transaction_object == nullptr) {
                    m_ui.m_amount_QLineEdit->setText("0.00");
                } else {
                    m_ui.m_amount_QLineEdit->setText(QString::number(std::abs(transaction_object->amount()), 'f', 2));
                }

                connect(m_ui.m_amount_QLineEdit, &QLineEdit::textChanged,
                        this, &TransactionEditorDialog::slotValidate);
            }

            {
                if (transaction_object == nullptr) {
                    m_ui.m_date_QDateEdit->setDate(QDate::currentDate());
                } else {
                    m_ui.m_date_QDateEdit->setDate(transaction_object->date());
                }

                connect(m_ui.m_date_QDateEdit, &QDateEdit::dateChanged,
                        this, &TransactionEditorDialog::slotValidate);
            }

            slotValidate();
        }

    private slots:
        bool slotValidate() {
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

            return is_valid;
        }

        virtual void accept() override {
            auto amount = std::abs(m_ui.m_amount_QLineEdit->text().toDouble());
            auto date = m_ui.m_date_QDateEdit->date();
            auto category = m_ui.m_category_QComboBox->currentText().trimmed();

            if (m_old_transaction_object == nullptr) {
                m_parent_bill_object->createTransaction(date, amount, category);
            } else {
                m_old_transaction_object->setAmount(amount);
                m_old_transaction_object->setDate(date);
                m_old_transaction_object->setCategory(category);
            }

            done(QDialog::Accepted);
        }

    private:
        Ui::TransactionEditorDialog m_ui;

        Persistance::BillObject *m_parent_bill_object;
        Persistance::TransactionObject *m_old_transaction_object;
    };
}
