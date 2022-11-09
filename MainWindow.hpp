#pragma once

#include <QMainWindow>
#include <QTimerEvent>
#include <QMessageBox>

#include "Models/DatabaseModel.hpp"
#include "Models/PocketMoneyProxyModel.hpp"
#include "Widgets/BillListWidget.hpp"
#include "Persistance/S3.hpp"

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

            m_database_model = new Models::DatabaseModel(this);
            m_ui.m_bills_BillListWidget->setModel(m_database_model);

            // Try to load database from disk.
            {
                auto database_opt = Persistance::load();

                if (database_opt.has_value()) {
                    m_database_model->deserialize(database_opt.value());
                } else {
                    QMessageBox message_box;
                    message_box.setText("No existing database found, loading empty database.");
                    message_box.setStandardButtons(QMessageBox::StandardButton::Ok);
                    message_box.exec();
                }
            }

            {
                m_ui.m_pocketMoney_QTableView->setModel(m_database_model->pocketMoneyModel());
            }
        }

        virtual void closeEvent(QCloseEvent *event) override {
            QMessageBox message_box;
            message_box.setText("The database may have been modified.");
            message_box.setInformativeText("Do you want to save changes?");
            message_box.setStandardButtons(QMessageBox::StandardButton::Save | QMessageBox::StandardButton::Discard | QMessageBox::StandardButton::Cancel);
            message_box.setDefaultButton(QMessageBox::StandardButton::Cancel);
            int retval = message_box.exec();

            if (retval == QMessageBox::StandardButton::Cancel) {
                event->ignore();
            } else if (retval == QMessageBox::StandardButton::Discard) {
                event->accept();
            } else if (retval == QMessageBox::StandardButton::Save) {
                auto database = m_database_model->serialize();
                Persistance::save(database);

                event->accept();
            } else {
                Q_UNREACHABLE();
            }
        }

    private:
        Models::DatabaseModel *m_database_model;

        Ui::MainWindow m_ui;
    };
}
