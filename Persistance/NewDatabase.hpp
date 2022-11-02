#pragma once

#include <QString>
#include <QDateTime>
#include <QObject>
#include <QObjectBindableProperty>

#include "Util.hpp"

namespace Accounting::Persistance
{
    // Used to indicate that this function should be used with care.
    // There are non-obvious constraints that need to be fulfilled.
    struct InternalMarker { };

    class DatabaseObject;
    class BillObject;
    class TransactionObject;

    class TransactionObject final : public QObject {
        Q_OBJECT

        Q_PROPERTY(QString id READ id BINDABLE bindableId);
        Q_PROPERTY(QDate date READ date WRITE setDate BINDABLE bindableDate);
        Q_PROPERTY(qreal amount READ amount WRITE setAmount BINDABLE bindableAmount);

    public:
        // The parent 'BillObject' will maintain a list of transactions.
        TransactionObject(InternalMarker, BillObject *parent);

        QString id() const { return m_id; }
        QBindable<QString> bindableId() { return QBindable<QString>(&m_id); }

        QDate date() const { return m_date; }
        void setDate(QDate value) { m_date = value; }
        QBindable<QDate> bindableDate() { return QBindable<QDate>(&m_date); }

        qreal amount() const { return m_amount; }
        void setAmount(qreal value) { m_amount = value; }
        QBindable<qreal> bindableAmount() { return QBindable<qreal>(&m_amount); }

    signals:
        void signalChanged(Accounting::Persistance::TransactionObject *transaction_object);

    private:
        Q_OBJECT_BINDABLE_PROPERTY(TransactionObject, QString, m_id, &TransactionObject::signalChanged);
        Q_OBJECT_BINDABLE_PROPERTY(TransactionObject, QDate, m_date, &TransactionObject::signalChanged);
        Q_OBJECT_BINDABLE_PROPERTY(TransactionObject, qreal, m_amount, &TransactionObject::signalChanged);
    };

    class BillObject final : public QObject {
        Q_OBJECT

        Q_PROPERTY(QString id READ id BINDABLE bindableId);
        Q_PROPERTY(QString transactions READ transactions BINDABLE bindableTransactions);

    public:
        enum class Status {
            Staged,
            PendingPayment,
            ConfirmedPaid,
        };
        Q_ENUM(Status);

        // The parent 'DatabaseObject' will maintain a list of bills.
        BillObject(InternalMarker, DatabaseObject *parent);

        QString id() const { return m_id; }
        QBindable<QString> bindableId() { return QBindable<QString>(&m_id); }

        QList<TransactionObject*> transactions() const { return m_transactions; }
        QBindable<QList<TransactionObject*>> bindableTransactions() { return QBindable<QList<TransactionObject*>>(&m_transactions); }

        TransactionObject* createTransaction(QDate date, qreal amount) {
            auto *transaction_object = new TransactionObject(InternalMarker{}, this);
            transaction_object->setDate(date);
            transaction_object->setAmount(amount);

            QList<TransactionObject*> new_transactions = m_transactions;
            new_transactions.append(transaction_object);
            m_transactions = new_transactions;

            return transaction_object;
        }

    signals:
        void signalChanged(Accounting::Persistance::BillObject *bill_object);

    private:
        Q_OBJECT_BINDABLE_PROPERTY(BillObject, QString, m_id, &BillObject::signalChanged);
        Q_OBJECT_BINDABLE_PROPERTY(BillObject, QList<TransactionObject*>, m_transactions, &BillObject::signalChanged);
    };

    class DatabaseObject final : public QObject {
        Q_OBJECT

        Q_PROPERTY(QString bills READ bills BINDABLE bindableBills);

    public:
        QList<BillObject*> bills() const { return m_bills; }
        QBindable<QList<BillObject*>> bindableBills() { return QBindable<QList<BillObject*>>(&m_bills); }

        BillObject* createBill() {
            auto *bill_object = new BillObject(InternalMarker{}, this);

            QList<BillObject*> bills = m_bills;
            bills.append(bill_object);
            m_bills = bills;

            return bill_object;
        }

    signals:
        void signalChanged(Accounting::Persistance::DatabaseObject *database_object);

    private:
        Q_OBJECT_BINDABLE_PROPERTY(DatabaseObject, QList<BillObject*>, m_bills, &DatabaseObject::signalChanged);
    };

    inline TransactionObject::TransactionObject(InternalMarker, BillObject *parent)
        : QObject(parent)
    {
        m_id = generate_id();
    }

    inline BillObject::BillObject(InternalMarker, DatabaseObject *parent)
        : QObject(parent)
    {
        m_id = generate_id();
    }
}
