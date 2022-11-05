#pragma once

#include <QMainWindow>
#include <QTimerEvent>

#include "Models/DatabaseModel.hpp"
#include "Widgets/BillListWidget.hpp"

#include "ui_MainWindow.h"

namespace Accounting
{
    class MainWindow final : public QMainWindow
    {
        Q_OBJECT

    public:
        explicit MainWindow(QWidget *parent = nullptr)
            : QMainWindow(parent)
        {
            m_ui.setupUi(this);

            auto *database_model = new Models::DatabaseModel(this);

            auto bill_list_widget = new Widgets::BillListWidget(database_model, this);
            setCentralWidget(bill_list_widget);
        }

    private:
        Ui::MainWindow m_ui;
    };
}
