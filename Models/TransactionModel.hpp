#pragma once

#include <QObject>
#include <QBindable>

#include "Persistance/Database.hpp"
#include "Util.hpp"

namespace Accounting::Models
{
    class TransactionModel final : public QObject {
    public:
        enum class Status {
            Normal,
            AlreadyPaid,
        };
        Q_ENUM(Status);

    private:
        Q_OBJECT

        Q_PROPERTY(QString id READ id WRITE setId BINDABLE bindableId NOTIFY signalChanged);
        Q_PROPERTY(QString parentBillId READ parentBillId WRITE setParentBillId BINDABLE bindableParentBillId NOTIFY signalChanged);
        Q_PROPERTY(QDate date READ date WRITE setDate BINDABLE bindableDate NOTIFY signalChanged);
        Q_PROPERTY(qreal amount READ amount WRITE setAmount BINDABLE bindableAmount NOTIFY signalChanged);
        Q_PROPERTY(QString category READ category WRITE setCategory BINDABLE bindableCategory NOTIFY signalChanged);
        Q_PROPERTY(Status status READ status WRITE setStatus BINDABLE bindableStatus NOTIFY signalChanged);
        Q_PROPERTY(bool isPocketMoney READ isPocketMoney WRITE setIsPocketMoney BINDABLE bindableIsPocketMoney NOTIFY signalChanged);
        Q_PROPERTY(qint64 creationTimestamp READ creationTimestamp WRITE setCreationTimestamp BINDABLE bindableCreationTimestamp NOTIFY signalChanged);

    public:
        explicit TransactionModel(QObject *parent = nullptr)
            : QObject(parent)
            , m_creation_timestamp(QDateTime::currentMSecsSinceEpoch())
        {
            qDebug() << "[TransactionModel::TransactionModel] this:" << this;
        }

        QString id() const { return m_id.value(); }
        void setId(QString value) { m_id =  value; }
        QBindable<QString> bindableId() { return QBindable<QString>(&m_id); }

        QString parentBillId() const { return m_parent_bill_id.value(); }
        void setParentBillId(QString value) { m_parent_bill_id =  value; }
        QBindable<QString> bindableParentBillId() { return QBindable<QString>(&m_parent_bill_id); }

        QDate date() const { return m_date.value(); }
        void setDate(QDate value) { m_date = value; }
        QBindable<QDate> bindableDate() { return QBindable<QDate>(&m_date); }

        qreal amount() const { return m_amount.value(); }
        void setAmount(qreal value) { m_amount = value; }
        QBindable<qreal> bindableAmount() { return QBindable<qreal>(&m_amount); }

        QString category() const { return m_category.value(); }
        void setCategory(QString value) { m_category = value; }
        QBindable<QString> bindableCategory() { return QBindable<QString>(&m_category); }

        Status status() const { return m_status.value(); }
        void setStatus(Status value) { m_status = value; }
        QBindable<Status> bindableStatus() { return QBindable<Status>(&m_status); }

        bool isPocketMoney() const { return m_is_pocket_money.value(); }
        void setIsPocketMoney(bool value) { m_is_pocket_money = value; }
        QBindable<bool> bindableIsPocketMoney() { return QBindable<bool>(&m_is_pocket_money); }

        qint64 creationTimestamp() const { return m_is_pocket_money.value(); }
        void setCreationTimestamp(qint64 value) { m_is_pocket_money = value; }
        QBindable<qint64> bindableCreationTimestamp() { return QBindable<qint64>(&m_is_pocket_money); }

        Persistance::Transaction serialize() const {
            return Persistance::Transaction{
                .m_id = id(),
                .m_date = date(),
                .m_amount = amount(),
                .m_category = category(),
                .m_creation_timestamp = m_creation_timestamp,
                .m_status = enum_type_to_string(status()),
                .m_is_pocket_money = m_is_pocket_money,
                .m_parent_bill_id = m_parent_bill_id,
            };
        }

        void deserialize(const Persistance::Transaction& value) {
            m_id = value.m_id;
            m_date = value.m_date;
            m_amount = value.m_amount;
            m_category = value.m_category;
            m_creation_timestamp = value.m_creation_timestamp;
            m_status = enum_type_from_string<Status>(value.m_status);
            m_is_pocket_money = value.m_is_pocket_money;
            m_parent_bill_id = value.m_parent_bill_id;
        }

    signals:
        void signalChanged();

    private:
        Q_OBJECT_BINDABLE_PROPERTY(TransactionModel, QString, m_id, &TransactionModel::signalChanged);
        Q_OBJECT_BINDABLE_PROPERTY(TransactionModel, QString, m_parent_bill_id, &TransactionModel::signalChanged);
        Q_OBJECT_BINDABLE_PROPERTY(TransactionModel, QDate, m_date, &TransactionModel::signalChanged);
        Q_OBJECT_BINDABLE_PROPERTY(TransactionModel, qreal, m_amount, &TransactionModel::signalChanged);
        Q_OBJECT_BINDABLE_PROPERTY(TransactionModel, QString, m_category, &TransactionModel::signalChanged);
        Q_OBJECT_BINDABLE_PROPERTY(TransactionModel, Status, m_status, &TransactionModel::signalChanged);
        Q_OBJECT_BINDABLE_PROPERTY(TransactionModel, bool, m_is_pocket_money, &TransactionModel::signalChanged);
        Q_OBJECT_BINDABLE_PROPERTY(TransactionModel, qint64, m_creation_timestamp, &TransactionModel::signalChanged);
    };
}
