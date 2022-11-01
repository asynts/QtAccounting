#pragma once

#include <QObject>
#include <QString>
#include <QDateTime>
#include <QMap>

#include "Util.hpp"

namespace Accounting::Persistance
{
    class Database;

    struct TransactionData {
        QString m_id;
        QDateTime m_timestamp_created;
        QDate m_date;
        qreal m_amount;
        QString m_category;

        static TransactionData new_default() {
            return TransactionData{
                .m_id = generate_id(),
                .m_timestamp_created = QDateTime::currentDateTimeUtc(),
                .m_date = QDate::currentDate(),
                .m_amount = 0.00,
                .m_category = "",
            };
        }
    };

    class TransactionObject final : public QObject {
        Q_OBJECT

    public:
        TransactionObject(Database& database, TransactionData&& data, QObject *parent = nullptr)
            : QObject(parent)
            , m_database(database)
        {
            m_versions.append(std::move(data));
        }

        QString id() const {
            return data().m_id;
        }

        QDate date() const {
            return data().m_date;
        }

        QDateTime timestamp_created() const {
            return data().m_timestamp_created;
        }

        qreal amount() const {
            return data().m_amount;
        }

        QString category() const {
            return data().m_category;
        }

        TransactionData data() const {
            return m_versions.constLast();
        }

    public slots:
        void slotUpdate(Accounting::Persistance::TransactionData data)
        {
            m_versions.append(std::move(data));
            emit signalChanged();
        }

    signals:
        void signalChanged();

    private:
        Database& m_database;

        // The last entry is the most recent one.
        QList<TransactionData> m_versions;
    };

    struct BillData {
        QString m_id;
        QDateTime m_timestamp_created;
        QList<QString> m_transaction_ids;
    };

    class BillObject final : public QObject {
        Q_OBJECT

    public:
        BillObject(Database& database, BillData&& bill_data, QObject *parent = nullptr)
            : QObject(parent)
            , m_database(database)
        {
            m_versions.append(std::move(bill_data));
        }

        QString id() const {
            return data().m_id;
        }

        QDateTime timestamp_created() const {
            return data().m_timestamp_created;
        }

        QList<TransactionObject*> transactions() const;

        BillData data() const {
            return m_versions.constLast();
        }

        void create_transaction(TransactionData&& data);

    public slots:
        void slotUpdate(Accounting::Persistance::BillData data)
        {
            m_versions.append(std::move(data));
            emit signalChanged();
        }

    signals:
        // This could be done way more efficiently if we communicate which entries have changed.
        // That seems to be what the whole model/view thing is about, but I can't get it to work properly.
        void signalChanged();

    private:
        Database& m_database;

        // The last element is the most recent version.
        QList<BillData> m_versions;
    };

    class Database final : public QObject {
        Q_OBJECT

    public:
        Database(QObject *parent = nullptr)
            : QObject(parent)
        {
            m_staged_bill = &internal_create_bill(BillData{
                                             .m_id = generate_id(),
                                             .m_timestamp_created = QDateTime::currentDateTimeUtc(),
                                             .m_transaction_ids = {},
                                         });
        }

        // Should not be used directly.
        // Instead use 'BillObject::create_transaction'.
        TransactionObject& internal_create_transaction(TransactionData&& data) {
            auto transaction = new TransactionObject(*this, std::move(data), this);
            m_transactions.insert(transaction->id(), transaction);
            return *transaction;
        }

        // Should not be used directly.
        BillObject& internal_create_bill(BillData&& data) {
            // FIXME: This should only be possible by replacing the staged bill.

            auto bill = new BillObject(*this, std::move(data), this);
            m_bills.insert(bill->id(), bill);

            emit signalBillAdded();

            return *bill;
        }

    signals:
        void signalBillAdded();

    public:
        QMap<QString, TransactionObject*> m_transactions;
        QMap<QString, BillObject*> m_bills;

        // Not null.
        BillObject *m_staged_bill = nullptr;
    };

    inline QList<TransactionObject*> BillObject::transactions() const {
        // FIXME: This is essentially a join and could be done more efficiently.
        QList<TransactionObject*> transactions;
        for (auto& transaction_id : data().m_transaction_ids) {
            transactions.append(m_database.m_transactions[transaction_id]);
        }

        return transactions;
    }

    inline void BillObject::create_transaction(TransactionData&& transaction_data) {
        auto& transaction_object = m_database.internal_create_transaction(std::move(transaction_data));

        auto bill_data = data();
        bill_data.m_transaction_ids.append(transaction_object.id());

        slotUpdate(bill_data);
    }
}

Q_DECLARE_METATYPE(Accounting::Persistance::TransactionData);
Q_DECLARE_METATYPE(Accounting::Persistance::BillData);
