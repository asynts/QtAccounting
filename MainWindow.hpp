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

            auto& transaction_object = m_database->create_transaction(Persistance::TransactionData{
                                                                   .m_id = "transaction#1",
                                                                   .m_timestamp_created = QDateTime::currentDateTimeUtc(),
                                                                   .m_amount = 42.00,
                                                               });

            QList<QString> transaction_ids;
            transaction_ids.append(transaction_object.id());

            auto& bill_object = m_database->create_bill(Persistance::BillData{
                                                            .m_id = "bill#1",
                                                            .m_timestamp_created = QDateTime::currentDateTimeUtc(),
                                                            .m_transaction_ids = transaction_ids,
                                                        });

            auto bill_widget = new Widgets::BillWidget(bill_object, this);
            setCentralWidget(bill_widget);

            m_transaction_object = &transaction_object;
            startTimer(1000);
        }

        void timerEvent(QTimerEvent *event)
        {
            m_transaction_object->update(Persistance::TransactionData{
                                             .m_id = "transaction#1",
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
