#pragma once

#include <QWidget>

#include "Persistance/Database.hpp"
#include "Widgets/BillWidget.hpp"
#include "Widgets/EditTransactionDialog.hpp"

namespace Accounting::Widgets
{
    class StagedBillWidget final : public QWidget {
        Q_OBJECT

    public:
        StagedBillWidget(Persistance::Database& database, QWidget *parent = nullptr)
            : QWidget(parent)
            , m_database(database)
        {
            auto layout = new QVBoxLayout;
            setLayout(layout);

            m_bill_widget = new BillWidget(this);
            m_bill_widget->setBillObject(*database.m_staged_bill);
            layout->addWidget(m_bill_widget);

            m_new_button = new QPushButton("New Transaction", this);

            connect(m_new_button, &QPushButton::clicked,
                    this, &StagedBillWidget::onNew);
        }

    public slots:
        void onNew() {
            if (m_edit_dialog == nullptr) {
                m_edit_dialog = new EditTransactionDialog(Persistance::TransactionData::new_default(), this);

                connect(m_edit_dialog, &EditTransactionDialog::onComplete,
                        this, &StagedBillWidget::onEditComplete);
            }

            m_edit_dialog->show();
            m_edit_dialog->raise();
            m_edit_dialog->activateWindow();

            // FIXME: Open a variation of 'EditTransactionDialog'.
        }

    private slots:
        void onEditComplete(Persistance::TransactionData new_transaction_data) {
            auto& transaction = m_database.create_transaction(std::move(new_transaction_data));
            m_database.stage_transaction(transaction);
        }

    private:
        EditTransactionDialog *m_edit_dialog = nullptr;

        BillWidget *m_bill_widget;
        QPushButton *m_new_button;

        Persistance::Database& m_database;
    };
}
