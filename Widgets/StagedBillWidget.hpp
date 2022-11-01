#pragma once

#include <QWidget>

#include "Persistance/Database.hpp"
#include "ui_StagedBillWidget.h"

namespace Accounting::Widgets
{
    class StagedBillWidget final : public QWidget {
        Q_OBJECT

    public:
        StagedBillWidget(Persistance::Database& database, QWidget *parent = nullptr)
            : QWidget(parent)
            , m_database(database)
        {
            Ui::StagedBillWidget ui;
            ui.setupUi(this);

            m_staged_bill = ui.m_staged_BillWidget;
            m_staged_bill->setBillWidget(*database.m_staged_bill);

            m_add_button = ui.m_add_PushButton;

            // FIXME: Connect add button.
        }

    public slots:
        void add() {
            // FIXME: Open a variation of 'EditTransactionDialog'.
        }

    private:
        BillWidget *m_staged_bill;
        QPushButton *m_add_button;

        Persistance::Database& m_database;
    };
}
