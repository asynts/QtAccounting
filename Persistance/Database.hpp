#pragma once

#include <QObject>
#include <QString>
#include <QDateTime>
#include <QMap>

namespace Accounting::Persistance
{
    class Database;

    struct TransactionData {
        QString m_id;
        QDateTime m_timestamp_created;
        qreal m_amount;
        QString m_category;
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
            return m_versions.constLast().m_id;
        }

        QDateTime timestamp_created() const {
            return m_versions.constLast().m_timestamp_created;
        }

        qreal amount() const {
            return m_versions.constLast().m_amount;
        }

        QString category() const {
            return m_versions.constLast().m_category;
        }

    public slots:
        void update(Accounting::Persistance::TransactionData data)
        {
            m_versions.append(std::move(data));
            emit onChanged();
        }

    signals:
        void onChanged();

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
            return m_versions.constLast().m_id;
        }

        QDateTime timestamp_created() const {
            return m_versions.constLast().m_timestamp_created;
        }

        QList<TransactionObject*> transactions() const;

    public slots:
        void update(Accounting::Persistance::BillData data)
        {
            m_versions.append(std::move(data));
            emit onChanged();
        }

    signals:
        // This could be done way more efficiently if we communicate which entries have changed.
        // That seems to be what the whole model/view thing is about, but I can't get it to work properly.
        void onChanged();

    private:
        Database& m_database;

        // The last element is the most recent version.
        QList<BillData> m_versions;
    };

    class Database final : public QObject{
        Q_OBJECT

    public:
        Database(QObject *parent = nullptr)
            : QObject(parent)
        {
        }

        TransactionObject& create_transaction(TransactionData&& data) {
            auto transaction = new TransactionObject(*this, std::move(data), this);
            m_transactions.insert(transaction->id(), transaction);
            return *transaction;
        }

        BillObject& create_bill(BillData&& data) {
            auto bill = new BillObject(*this, std::move(data), this);
            m_bills.insert(bill->id(), bill);
            return *bill;
        }

        QMap<QString, TransactionObject*> m_transactions;
        QMap<QString, BillObject*> m_bills;
    };

    inline QList<TransactionObject*> BillObject::transactions() const {
        // FIXME: This is essentially a join and could be done more efficiently.
        QList<TransactionObject*> transactions;
        for (auto& transaction_id : m_versions.constLast().m_transaction_ids) {
            transactions.append(m_database.m_transactions[transaction_id]);
        }

        return transactions;
    }
}

Q_DECLARE_METATYPE(Accounting::Persistance::TransactionData);
Q_DECLARE_METATYPE(Accounting::Persistance::BillData);
