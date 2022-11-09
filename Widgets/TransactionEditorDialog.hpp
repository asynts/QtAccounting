#pragma once

#include <fmt/format.h>

#include <QDialog>
#include <QPushButton>
#include <QMessageBox>

#include "Entities/BillEntity.hpp"
#include "Entities/TransactionEntity.hpp"

#include "ui_TransactionEditorDialog.h"

namespace Accounting::Widgets
{
    class TransactionEditorDialog final : public QDialog {
        Q_OBJECT

    public:
        explicit TransactionEditorDialog(Entities::BillEntity *parent_bill, Entities::TransactionEntity *transaction, QWidget *parent = nullptr)
            : QDialog(parent)
            , m_old_transaction(transaction)
            , m_parent_bill(parent_bill)
        {
            m_ui.setupUi(this);

            if (transaction == nullptr) {
                setWindowTitle("New Transaction");
            } else {
                setWindowTitle(QString("Edit Transaction '%1'").arg(transaction->id()));
            }

            {
                fill_QComboBox_with_enum<Entities::TransactionEntity::Status>(m_ui.m_status_QComboBox);
                if (transaction == nullptr) {
                    m_ui.m_status_QComboBox->setCurrentIndex(0);
                } else {
                    m_ui.m_status_QComboBox->setCurrentIndex(static_cast<int>(transaction->status()));
                }
            }

            {
                m_ui.m_category_QComboBox->addItem("Groceries");
                m_ui.m_category_QComboBox->addItem("Transport");

                if (transaction == nullptr) {
                    m_ui.m_category_QComboBox->setCurrentIndex(0);
                } else {
                    m_ui.m_category_QComboBox->setCurrentText(transaction->category());
                }

                connect(m_ui.m_category_QComboBox, &QComboBox::currentTextChanged,
                        this, &TransactionEditorDialog::slotValidate);
            }

            {
                auto *validator = new QDoubleValidator(this);
                m_ui.m_amount_QLineEdit->setValidator(validator);

                if (transaction == nullptr) {
                    m_ui.m_amount_QLineEdit->setText("0.00");
                } else {
                    m_ui.m_amount_QLineEdit->setText(QString::number(transaction->amount(), 'f', 2));
                }

                connect(m_ui.m_amount_QLineEdit, &QLineEdit::textChanged,
                        this, &TransactionEditorDialog::slotValidate);
            }

            {
                if (transaction == nullptr) {
                    m_ui.m_date_QDateEdit->setDate(QDate::currentDate());
                } else {
                    m_ui.m_date_QDateEdit->setDate(transaction->date());
                }

                connect(m_ui.m_date_QDateEdit, &QDateEdit::dateChanged,
                        this, &TransactionEditorDialog::slotValidate);
            }

            {
                m_ui.m_delete_QPushButton->setEnabled(transaction != nullptr);

                connect(m_ui.m_delete_QPushButton, &QPushButton::clicked,
                        this, &TransactionEditorDialog::slotDelete);
            }

            slotValidate();
        }

    private slots:
        void slotDelete() {
            Q_ASSERT(m_old_transaction != nullptr);

            int reply = QMessageBox::question(
                        this,
                        "Delete?",
                        QString::fromStdString(fmt::format("Are you sure that you want to delete transaction '{}'?", m_old_transaction->id().toStdString())),
                        QMessageBox::StandardButton::Yes | QMessageBox::StandardButton::No);

            if (reply == QMessageBox::StandardButton::Yes) {
                // FIXME: m_parent_bill->deleteTransaction(m_old_transaction);
                done(QDialog::DialogCode::Accepted);
            }
        }

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
            auto amount = m_ui.m_amount_QLineEdit->text().toDouble();
            auto date = m_ui.m_date_QDateEdit->date();
            auto category = m_ui.m_category_QComboBox->currentText().trimmed();
            auto status = static_cast<Entities::TransactionEntity::Status>(m_ui.m_status_QComboBox->currentIndex());

            if (m_old_transaction == nullptr) {
                // FIXME: m_parent_bill->createTransaction(date, amount, category, status);
            } else {
                m_old_transaction->setAmount(amount);
                m_old_transaction->setDate(date);
                m_old_transaction->setCategory(category);
                m_old_transaction->setStatus(status);
            }

            done(QDialog::Accepted);
        }

    private:
        Ui::TransactionEditorDialog m_ui;

        Entities::BillEntity *m_parent_bill;
        Entities::TransactionEntity *m_old_transaction;
    };
}
