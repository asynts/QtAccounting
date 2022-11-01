#pragma once

#include <QMainWindow>
#include <QTimerEvent>

#include "Persistance/Database.hpp"
#include "Widgets/BillListWidget.hpp"
#include "ui_MainWindow.h"

namespace Accounting
{
    class MainWindow final : public QMainWindow
    {
        Q_OBJECT

    public:
        MainWindow(QWidget *parent = nullptr)
            : QMainWindow(parent)
        {
            m_ui.setupUi(this);

            auto *database = new Persistance::Database(this);

            auto bill_list_widget = new Widgets::BillListWidget(*database, this);
            setCentralWidget(bill_list_widget);
        }

    private:
        Ui::MainWindow m_ui;
    };
}
