#pragma once

#include <QMainWindow>
#include <QTimerEvent>
#include <QMessageBox>

#include "Models/DatabaseModel.hpp"
#include "Widgets/BillListWidget.hpp"
#include "Persistance/S3.hpp"

#include "ui_MainWindow.h"

// #include "Migrations/0001_2022-11-06_add-timestamps.hpp"
// #include "Migrations/0002_2022-11-06_transaction-status.hpp"

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

            // Migrations::From1To2::load_migrate_save();
            // Migrations::From2To3::load_migrate_save();

            m_database_model = new Models::DatabaseModel(this);
            m_ui.m_bills_BillListWidget->setModel(m_database_model);

            // Try to load database from disk.
            {
                auto database_opt = Persistance::load_from_disk();

                if (database_opt.has_value()) {
                    m_database_model->deserialize(database_opt.value());
                } else {
                    QMessageBox message_box;
                    message_box.setText("No existing database found, loading empty database.");
                    message_box.setStandardButtons(QMessageBox::StandardButton::Ok);
                    message_box.exec();
                }
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

                // Save locally.
                auto localPath = Persistance::save_to_disk(database);

                // Upload file to S3.
                QString remotePath = "/Database";
                remotePath.append(QFileInfo(localPath).fileName());
                Persistance::upload_file(localPath, remotePath);

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
