#pragma once

#include <QMainWindow>
#include <QTimerEvent>

#include "Persistance/Database.hpp"
#include "Widgets/StagedBillWidget.hpp"

namespace Accounting
{
    class MainWindow : public QMainWindow
    {
        Q_OBJECT

    public:
        MainWindow(QWidget *parent = nullptr)
            : QMainWindow(parent)
        {
            m_database = new Persistance::Database(this);

            auto staged_bill_widget = new Widgets::StagedBillWidget(*m_database, this);
            setCentralWidget(staged_bill_widget);
        }

    private:
        Persistance::Database *m_database;
    };
}
