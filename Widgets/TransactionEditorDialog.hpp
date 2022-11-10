#pragma once

#include <fmt/format.h>

#include <QDialog>
#include <QPushButton>
#include <QMessageBox>

#include "Models/TransactionModel.hpp"
#include "Models/BillProxyModel.hpp"

#include "ui_TransactionEditorDialog.h"

namespace Accounting::Widgets
{
    class TransactionEditorDialog final : public QDialog {
        Q_OBJECT

    public:
        explicit TransactionEditorDialog(Models::BillProxyModel *parent_bill_model, Models::TransactionModel *transaction_model, QWidget *parent = nullptr)
            : QDialog(parent)
            , m_old_transaction_model(transaction_model)
            , m_parent_bill_model(parent_bill_model)
        {
            m_ui.setupUi(this);

            if (transaction_model == nullptr) {
                setWindowTitle("New Transaction");
            } else {
                setWindowTitle(QString("Edit Transaction '%1'").arg(transaction_model->id()));
            }

            {
                fill_QComboBox_with_enum<Models::TransactionModel::Status>(m_ui.m_status_QComboBox);
                if (transaction_model == nullptr) {
                    m_ui.m_status_QComboBox->setCurrentIndex(0);
                } else {
                    m_ui.m_status_QComboBox->setCurrentIndex(static_cast<int>(transaction_model->status()));
                }
            }

            {
                m_ui.m_category_QComboBox->addItem("Groceries");
                m_ui.m_category_QComboBox->addItem("Transport");
                m_ui.m_category_QComboBox->addItem("Pocket Money");

                if (transaction_model == nullptr) {
                    m_ui.m_category_QComboBox->setCurrentIndex(0);
                } else {
                    m_ui.m_category_QComboBox->setCurrentText(transaction_model->category());
                }

                connect(m_ui.m_category_QComboBox, &QComboBox::currentTextChanged,
                        this, &TransactionEditorDialog::slotValidate);
            }

            {
                auto *validator = new QDoubleValidator(this);
                m_ui.m_amount_QLineEdit->setValidator(validator);

                if (transaction_model == nullptr) {
                    m_ui.m_amount_QLineEdit->setText("0.00");
                } else {
                    m_ui.m_amount_QLineEdit->setText(QString::number(transaction_model->amount(), 'f', 2));
                }

                connect(m_ui.m_amount_QLineEdit, &QLineEdit::textChanged,
                        this, &TransactionEditorDialog::slotValidate);
            }

            {
                if (transaction_model == nullptr) {
                    m_ui.m_date_QDateEdit->setDate(QDate::currentDate());
                } else {
                    m_ui.m_date_QDateEdit->setDate(transaction_model->date());
                }

                connect(m_ui.m_date_QDateEdit, &QDateEdit::dateChanged,
                        this, &TransactionEditorDialog::slotValidate);
            }

            {
                m_ui.m_delete_QPushButton->setEnabled(transaction_model != nullptr);

                connect(m_ui.m_delete_QPushButton, &QPushButton::clicked,
                        this, &TransactionEditorDialog::slotDelete);
            }

            {
                m_ui.m_pocketMoney_QComboBox->addItem("No");
                m_ui.m_pocketMoney_QComboBox->addItem("Yes");

                if (transaction_model != nullptr && transaction_model->isPocketMoney()) {
                    m_ui.m_pocketMoney_QComboBox->setCurrentIndex(1);
                } else {
                    m_ui.m_pocketMoney_QComboBox->setCurrentIndex(0);
                }
            }

            slotValidate();
        }

    private slots:
        void slotDelete() {
            Q_ASSERT(m_old_transaction_model != nullptr);

            int reply = QMessageBox::question(
                        this,
                        "Delete?",
                        QString::fromStdString(fmt::format("Are you sure that you want to delete transaction '{}'?", m_old_transaction_model->id().toStdString())),
                        QMessageBox::StandardButton::Yes | QMessageBox::StandardButton::No);

            if (reply == QMessageBox::StandardButton::Yes) {
                m_old_transaction_model->deleteMyself();
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

            if (m_ui.m_pocketMoney_QComboBox->currentIndex() == 1 && m_ui.m_category_QComboBox->currentText() != "Pocket Money") {
                is_valid = false;
            }

            m_ui.m_buttons_QDialogButtonBox->button(QDialogButtonBox::Ok)->setEnabled(is_valid);

            return is_valid;
        }

        virtual void accept() override {
            auto amount = m_ui.m_amount_QLineEdit->text().toDouble();
            auto date = m_ui.m_date_QDateEdit->date();
            auto category = m_ui.m_category_QComboBox->currentText().trimmed();
            auto status = static_cast<Models::TransactionModel::Status>(m_ui.m_status_QComboBox->currentIndex());
            auto is_pocket_money = m_ui.m_pocketMoney_QComboBox->currentIndex() == 1;

            if (m_old_transaction_model == nullptr) {
                m_parent_bill_model->createTransaction(date, amount, category, status, is_pocket_money);
            } else {
                m_old_transaction_model->setAmount(amount);
                m_old_transaction_model->setDate(date);
                m_old_transaction_model->setCategory(category);
                m_old_transaction_model->setStatus(status);
            }

            done(QDialog::Accepted);
        }

    private:
        Ui::TransactionEditorDialog m_ui;

        Models::BillProxyModel *m_parent_bill_model;
        Models::TransactionModel *m_old_transaction_model;
    };
}
