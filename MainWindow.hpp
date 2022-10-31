#pragma once

#include <QMainWindow>
#include <QTimerEvent>

#include "Persistance/Database.hpp"
#include "Widgets/BillWidget.hpp"

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

            m_database->create_transaction(Persistance::TransactionData{
                                               .m_id = "transaction#1",
                                               .m_timestamp_created = QDateTime::currentDateTimeUtc(),
                                               .m_amount = 42.00,
                                           });

            QList<QString> transaction_ids;
            transaction_ids.append("transaction#1");

            auto& bill_object = m_database->create_bill(Persistance::BillData{
                                                            .m_id = "bill#1",
                                                            .m_timestamp_created = QDateTime::currentDateTimeUtc(),
                                                            .m_transaction_ids = transaction_ids,
                                                        });

            auto bill_widget = new Widgets::BillWidget(bill_object, this);
            setCentralWidget(bill_widget);

            m_bill_object = &bill_object;
            startTimer(1000);
        }

        void timerEvent(QTimerEvent *event)
        {
            m_database->create_transaction(Persistance::TransactionData{
                                               .m_id = "transaction#2",
                                               .m_timestamp_created = QDateTime::currentDateTimeUtc(),
                                               .m_amount = -14.23,
                                           });

            QList<QString> transaction_ids;
            transaction_ids.append("transaction#1");
            transaction_ids.append("transaction#2");

            m_bill_object->update(Persistance::BillData{
                                      .m_id = "bill#1",
                                      .m_timestamp_created = QDateTime::currentDateTimeUtc(),
                                      .m_transaction_ids = transaction_ids,
                                  });

            killTimer(event->timerId());
        }

    private:
        Persistance::BillObject *m_bill_object;

        Persistance::Database *m_database;
    };
}
