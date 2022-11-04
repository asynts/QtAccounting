#pragma once

#include <QWidget>
#include <QLayout>
#include <QLabel>
#include <QPushButton>
#include <QDialog>

#include "Models/BillModel.hpp"
/*
#include "Widgets/TransactionEditorDialog.hpp"
*/
#include "ui_BillEditorDialog.h"

namespace Accounting::Widgets
{
    class BillEditorDialog final : public QDialog {
        Q_OBJECT

    public:
        explicit BillEditorDialog(Models::BillModel *bill_model, QWidget *parent = nullptr)
            : QDialog(parent)
            , m_bill_model(bill_model)
        {
            m_ui.setupUi(this);

            /*
            setWindowTitle(QString("Edit Bill '%1'").arg(bill_model->id()));
            */

            /*
            fill_QComboBox_with_enum<Persistance::BillObject::Status>(m_ui.m_status_QComboBox);
            */

            m_ui.m_transactions_QTableView->setModel(bill_model);

            connect(m_ui.m_status_QComboBox, &QComboBox::currentIndexChanged,
                    this, &BillEditorDialog::slotStatusChanged);

            connect(m_ui.m_new_QPushButton, &QPushButton::clicked,
                    this, &BillEditorDialog::slotNewTransaction);
        }

    private slots:
        void slotStatusChanged() {
            /*
            m_bill_model->setStatus(static_cast<Persistance::BillObject::Status>(m_ui.m_status_QComboBox->currentIndex()));
            */
        }

        void slotNewTransaction()
        {
            /*
            TransactionEditorDialog dialog{ m_bill_object, nullptr };
            dialog.exec();
            */
        }

    private:
        Ui::BillEditorDialog m_ui;

        Models::BillModel*const m_bill_model;
    };
}
