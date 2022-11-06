#pragma once

#include <QObject>
#include <QBindable>
#include <QDate>

#include "Persistance.hpp"

namespace Accounting::Models
{
    class TransactionModel final : public QObject {
        Q_OBJECT

        Q_PROPERTY(QString id READ id BINDABLE bindableId NOTIFY signalChanged);
        Q_PROPERTY(QDate date READ date WRITE setDate BINDABLE bindableDate NOTIFY signalChanged);
        Q_PROPERTY(qreal amount READ amount WRITE setAmount BINDABLE bindableAmount NOTIFY signalChanged);
        Q_PROPERTY(QString category READ category WRITE setCategory BINDABLE bindableCategory NOTIFY signalChanged);

    public:
        explicit TransactionModel(QObject *parent = nullptr)
            : QObject(parent) { }

        explicit TransactionModel(QString id, QDate date, qreal amount, QString category, QObject *parent = nullptr)
            : QObject(parent)
            , m_id(id)
            , m_date(date)
            , m_amount(amount)
            , m_category(category) { }

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

        Persistance::Transaction serialize() const {
            return Persistance::Transaction{
                .m_id = id(),
                .m_date = date(),
                .m_amount = amount(),
                .m_category = category(),
            };
        }

        void deserialize(const Persistance::Transaction& value) {
            m_id = value.m_id;
            m_date = value.m_date;
            m_amount = value.m_amount;
            m_category = value.m_category;
        }

    signals:
        void signalChanged();

    private:
        Q_OBJECT_BINDABLE_PROPERTY(TransactionModel, QString, m_id, &TransactionModel::signalChanged);
        Q_OBJECT_BINDABLE_PROPERTY(TransactionModel, QDate, m_date, &TransactionModel::signalChanged);
        Q_OBJECT_BINDABLE_PROPERTY(TransactionModel, qreal, m_amount, &TransactionModel::signalChanged);
        Q_OBJECT_BINDABLE_PROPERTY(TransactionModel, QString, m_category, &TransactionModel::signalChanged);
    };
}
