#pragma once

#include <QMainWindow>
#include <QTimerEvent>

#include "Persistance/Database.hpp"
#include "Widgets/StagedBillWidget.hpp"
#include "Widgets/BillListWidget.hpp"

namespace Accounting
{
    class MainWindow : public QMainWindow
    {
        Q_OBJECT

    public:
        MainWindow(QWidget *parent = nullptr)
            : QMainWindow(parent)
        {
            auto *database = new Persistance::Database(this);

            auto bill_list_widget = new Widgets::BillListWidget(*database, this);
            setCentralWidget(bill_list_widget);
        }
    };
}
