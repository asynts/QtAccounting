#pragma once

#include <QMainWindow>
#include <QTimerEvent>
#include <QMessageBox>

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

            connect(m_ui.m_buttons_QDialogButtonBox, &QDialogButtonBox::rejected,
                    this, &MainWindow::slotDiscard);

            connect(m_ui.m_buttons_QDialogButtonBox, &QDialogButtonBox::accepted,
                    this, &MainWindow::slotCommit);

            // FIXME: Call 'slotDiscard' if close button of window is pressed.
        }

    private slots:
        void slotDiscard() {
            QMessageBox message_box;
            message_box.setText("The database has been modified.");
            message_box.setInformativeText("Do you want to save changes?");
            message_box.setStandardButtons(QMessageBox::StandardButton::Save | QMessageBox::StandardButton::Discard | QMessageBox::StandardButton::Cancel);
            message_box.setDefaultButton(QMessageBox::StandardButton::Cancel);
            int retval = message_box.exec();

            if (retval == QMessageBox::StandardButton::Cancel) {
                // Do nothing.
            } else if (retval == QMessageBox::StandardButton::Discard) {
                close();
            } else if (retval == QMessageBox::StandardButton::Save) {
                slotCommit();
            } else {
                Q_UNREACHABLE();
            }
        }

        void slotCommit() {
            auto database = m_database_model->serialize();
            Persistance::save_to_disk(database);

            close();
        }

    private:
        Models::DatabaseModel *m_database_model;

        Ui::MainWindow m_ui;
    };
}
