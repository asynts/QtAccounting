#pragma once

#include <QMainWindow>
#include <QTimerEvent>
#include <QMessageBox>
#include <QCloseEvent>

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
        virtual void closeEvent(QCloseEvent *event) override {
            // If there is no database loaded, we don't need to try to save it.
            if (!m_database_loaded) {
                event->accept();
                return;
            }

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
                if (save_database_to_disk()) {
                    event->accept();
                } else {
                    event->ignore();
                }
            } else {
                Q_UNREACHABLE();
            }
        }

        virtual bool event(QEvent *event) override
        {
            if (event->type() == DatabaseLoadedEvent::Type) {
                eventDatabaseLoaded(static_cast<DatabaseLoadedEvent*>(event)->m_database);
                return true;
            } else {
                return QMainWindow::event(event);
            }
        }

        void eventDatabaseLoaded(const Persistance::Database& database)
        {
            m_database_model->deserialize(database);
            m_ui.m_pocketMoney_QTableView->setModel(m_database_model->pocketMoneyModel());

            m_ui.m_pocketMoney_QTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::ResizeToContents);
            m_ui.m_pocketMoney_QTableView->horizontalHeader()->setSectionResizeMode(
                        Models::TransactionListModel::Columns::ColumnCategory,
                        QHeaderView::ResizeMode::Stretch);

            m_database_loaded = true;
        }

        // This will download the database and provide a dialog to inform the user.
        // If the operation failed and the user decided to cancel it, returns 'std::nullopt'.
        static std::optional<MainWindow*> try_create_main_window()
        {
            MainWindow *main_window = new MainWindow;

            // This will create a modal dialog which can run without QApplication.
            if (!main_window->load_database_from_disk())
            {
                return std::nullopt;
            }

            return main_window;
        }

    private:
        explicit MainWindow(QWidget *parent = nullptr)
            : QMainWindow(parent)
        {
            m_ui.setupUi(this);

            m_database_model = new Models::DatabaseModel(this);
            m_ui.m_bills_BillListWidget->setModel(m_database_model);

            // The 'try_create_main_window' function should have put an 'DatabaseLoadedEvent' event into the queue.
            // It will be processed as soon as the event loop is started.
            m_database_loaded = false;
        }

        bool save_database_to_disk()
        {
            auto database = m_database_model->serialize();

            Widgets::FutureProgressDialog dialog{
                "Saving database to disk",
                [this, database] {
                    return std::async(std::launch::async, [this, database] {
                        if (Persistance::save_async(database).get()) {
                            return ResultEnum::Success;
                        } else {
                            return ResultEnum::Failure;
                        }
                    });
                }
            };

            return dialog.exec() == QDialog::DialogCode::Accepted;
        }

        bool load_database_from_disk()
        {
            Widgets::FutureProgressDialog dialog{
                "Loading database from disk",
                [this] {
                    return std::async(std::launch::async, [this] {
                        auto database_opt = Persistance::load_async().get();

                        if (database_opt.has_value()) {
                            // We must not update the UI from a worker thread.
                            // Emit an event for the main thread.
                            QCoreApplication::postEvent(this, new DatabaseLoadedEvent{ std::move(database_opt).value() });
                            return ResultEnum::Success;
                        } else {
                            return ResultEnum::Failure;
                        }
                    });
                }
            };

            return dialog.exec() == QDialog::DialogCode::Accepted;
        }

        Models::DatabaseModel *m_database_model;
        bool m_database_loaded;

        Ui::MainWindow m_ui;
    };
}
