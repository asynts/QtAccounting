#pragma once

#include <QString>
#include <QDateTime>
#include <QObject>
#include <QObjectBindableProperty>
#include <QMetaEnum>

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

        Q_PROPERTY(QString id READ id BINDABLE bindableId NOTIFY signalChanged);
        Q_PROPERTY(QDate date READ date WRITE setDate BINDABLE bindableDate NOTIFY signalChanged);
        Q_PROPERTY(qreal amount READ amount WRITE setAmount BINDABLE bindableAmount NOTIFY signalChanged);
        Q_PROPERTY(QString category READ category WRITE setCategory BINDABLE bindableCategory NOTIFY signalChanged);

    public:
        // The parent 'BillObject' will maintain a list of transactions.
        TransactionObject(InternalMarker, BillObject *parent);

        QString id() const { return m_id.value(); }
        QBindable<QString> bindableId() { return QBindable<QString>(&m_id); }

        QDate date() const { return m_date.value(); }
        void setDate(QDate value) { m_date = value; }
        QBindable<QDate> bindableDate() { return QBindable<QDate>(&m_date); }

        qreal amount() const { return m_amount.value(); }
        void setAmount(qreal value) { m_amount = value; }
        QBindable<qreal> bindableAmount() { return QBindable<qreal>(&m_amount); }

        QString category() const { return m_category.value(); }
        void setCategory(QString value) { m_category = value; }
        QBindable<QString> bindableCategory() { return QBindable<QString>(&m_category); }

    signals:
        void signalChanged();

    private:
        Q_OBJECT_BINDABLE_PROPERTY(TransactionObject, QString, m_id, &TransactionObject::signalChanged);
        Q_OBJECT_BINDABLE_PROPERTY(TransactionObject, QDate, m_date, &TransactionObject::signalChanged);
        Q_OBJECT_BINDABLE_PROPERTY(TransactionObject, qreal, m_amount, &TransactionObject::signalChanged);
        Q_OBJECT_BINDABLE_PROPERTY(TransactionObject, QString, m_category, &TransactionObject::signalChanged);
    };

    class BillObject final : public QObject {
        Q_OBJECT

        Q_PROPERTY(QString id READ id BINDABLE bindableId NOTIFY signalChanged);
        Q_PROPERTY(Status status READ status WRITE setStatus BINDABLE bindableStatus NOTIFY signalChanged);
        Q_PROPERTY(QList<TransactionObject*> transactions READ transactions BINDABLE bindableTransactions NOTIFY signalChanged);

    public:
        enum class Status {
            Staged,
            PendingPayment,
            ConfirmedPaid,
        };
        Q_ENUM(Status);

        static Status status_from_string(QString key) {
            bool ok;
            auto value = QMetaEnum::fromType<Status>().keyToValue(key.toUtf8(), &ok);
            Q_ASSERT(ok);

            return static_cast<Status>(value);
        }
        static QString status_to_string(Status value) {
            return QMetaEnum::fromType<Status>().valueToKey(static_cast<int>(value));
        }

        // The parent 'DatabaseObject' will maintain a list of bills.
        BillObject(InternalMarker, DatabaseObject *parent);

        QString id() const { return m_id.value(); }
        QBindable<QString> bindableId() { return QBindable<QString>(&m_id); }

        Status status() const { return m_status.value(); }
        void setStatus(Status value) { m_status = value; }
        QBindable<Status> bindableStatus() { return QBindable<Status>(&m_status); }

        QList<TransactionObject*> transactions() const { return m_transactions; }
        QBindable<QList<TransactionObject*>> bindableTransactions() { return QBindable<QList<TransactionObject*>>(&m_transactions); }

        TransactionObject* createTransaction(QDate date, qreal amount, QString category) {
            auto *transaction_object = new TransactionObject(InternalMarker{}, this);
            transaction_object->setDate(date);
            transaction_object->setAmount(amount);
            transaction_object->setCategory(category);

            QList<TransactionObject*> new_transactions = m_transactions;
            new_transactions.append(transaction_object);
            m_transactions = new_transactions;

            return transaction_object;
        }

    signals:
        void signalChanged();

    private:
        Q_OBJECT_BINDABLE_PROPERTY(BillObject, QString, m_id, &BillObject::signalChanged);
        Q_OBJECT_BINDABLE_PROPERTY(BillObject, Status, m_status, &BillObject::signalChanged);
        Q_OBJECT_BINDABLE_PROPERTY(BillObject, QList<TransactionObject*>, m_transactions, &BillObject::signalChanged);
    };

    // FIXME: The database should maintain a log of what changed.
    class DatabaseObject final : public QObject {
        Q_OBJECT

        Q_PROPERTY(QList<BillObject*> bills READ bills BINDABLE bindableBills NOTIFY signalChanged);

    public:
        DatabaseObject(QObject *parent = nullptr)
            : QObject(parent) { }

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
        void signalChanged();

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
