#pragma once

#include <QWidget>
#include <QLayout>
#include <QLabel>
#include <QPushButton>

#include "Models/DatabaseModel.hpp"
/*
#include "Widgets/BillEditorDialog.hpp"
*/

#include "ui_BillListWidget.h"

namespace Accounting::Widgets
{
    class BillListWidget final : public QWidget {
        Q_OBJECT

    public:
        explicit BillListWidget(Models::DatabaseModel *database_model, QWidget *parent = nullptr)
            : QWidget(parent)
            , m_database_model(database_model)
        {
            m_ui.setupUi(this);

            m_ui.m_bills_QTableView->setModel(database_model);

            connect(m_ui.m_new_QPushButton, &QPushButton::clicked,
                    this, &BillListWidget::slotNewBill);
        }

    private slots:
        void slotNewBill() {
            m_database_model->createBill();
        }

    private:
        Ui::BillListWidget m_ui;

        Models::DatabaseModel *m_database_model;
    };
}
