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

    enum class BillStatus {
        // I am working on this bill.
        Staged,

        // The bill has been send out.
        PendingPayment,

        // The bill has been paid.
        ConfirmedPaid,
    };

    inline BillStatus bill_status_from_string(QString string) {
        if (string == "Staged") {
            return BillStatus::Staged;
        } else if (string == "PendingPayment") {
            return BillStatus::PendingPayment;
        } else if (string == "ConfirmedPaid") {
            return BillStatus::ConfirmedPaid;
        } else {
            Q_UNREACHABLE();
        }
    }

    struct BillData {
        QString m_id;
        QDateTime m_timestamp_created;
        QList<QString> m_transaction_ids;
        BillStatus m_status;

        static BillData new_default() {
            return BillData{
                .m_id = generate_id(),
                .m_timestamp_created = QDateTime::currentDateTimeUtc(),
                .m_transaction_ids = {},
                .m_status = BillStatus::Staged,
            };
        }
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

        BillStatus status() const {
            return data().m_status;
        }

        QString status_string() const {
            switch (status()) {
            case BillStatus::Staged:
                return "Staged";
            case BillStatus::PendingPayment:
                return "PendingPayment";
            case BillStatus::ConfirmedPaid:
                return "ConfirmedPaid";
            default:
                Q_UNREACHABLE();
            }
        }

        BillData data() const {
            return m_versions.constLast();
        }

        void create_transaction(TransactionData&& data);

    public slots:
        void slotUpdate(Accounting::Persistance::BillData data);

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
            create_staged_bill();
        }

        // Should not be used directly.
        // Instead use 'BillObject::create_transaction'.
        TransactionObject& internal_create_transaction(TransactionData&& data) {
            auto transaction = new TransactionObject(*this, std::move(data), this);
            m_transactions.insert(transaction->id(), transaction);
            return *transaction;
        }

        BillObject& create_staged_bill() {
            auto bill = new BillObject(*this, BillData::new_default(), this);
            m_bills.insert(bill->id(), bill);

            emit signalBillChanged(*bill);

            return *bill;
        }

    signals:
        void signalBillChanged(Accounting::Persistance::BillObject& bill_object);

    public:
        QMap<QString, TransactionObject*> m_transactions;
        QMap<QString, BillObject*> m_bills;
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

    inline void BillObject::slotUpdate(Accounting::Persistance::BillData data)
    {
        m_versions.append(std::move(data));
        emit signalChanged();

        // FIXME: This causes a SIGSEGV:
        // emit m_database.signalBillChanged(*this);
    }
}

Q_DECLARE_METATYPE(Accounting::Persistance::TransactionData);
Q_DECLARE_METATYPE(Accounting::Persistance::BillData);
