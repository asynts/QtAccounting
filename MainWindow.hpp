#pragma once

#include <QMainWindow>
#include <QTimerEvent>

#include "Persistance/Database.hpp"
#include "Widgets/TransactionWidget.hpp"

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

            auto& transaction_object = m_database->create_transaction(Persistance::TransactionData{
                                                                   .m_id = "#1",
                                                                   .m_timestamp_created = QDateTime::currentDateTimeUtc(),
                                                                   .m_amount = 42.00,
                                                               });

            auto transaction_widget = new Widgets::TransactionWidget(transaction_object, this);
            setCentralWidget(transaction_widget);

            m_transaction_object = &transaction_object;
            startTimer(1000);
        }

        void timerEvent(QTimerEvent *event)
        {
            m_transaction_object->update(Persistance::TransactionData{
                                             .m_id = "#1",
                                             .m_timestamp_created = QDateTime::currentDateTimeUtc(),
                                             .m_amount = 13.00,
                                         });

            killTimer(event->timerId());
        }

    private:
        Persistance::TransactionObject *m_transaction_object;

        Persistance::Database *m_database;
    };
}
