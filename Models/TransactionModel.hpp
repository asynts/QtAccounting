#pragma once

#include <QObject>
#include <QBindable>
#include <QDate>

namespace Accounting::Models
{
    class TransactionModel final : public QObject {
        Q_OBJECT

        Q_PROPERTY(QDate date READ date WRITE setDate BINDABLE bindableDate NOTIFY signalChanged);
        Q_PROPERTY(qreal amount READ amount WRITE setAmount BINDABLE bindableAmount NOTIFY signalChanged);
        Q_PROPERTY(QString category READ category WRITE setCategory BINDABLE bindableCategory NOTIFY signalChanged);

    public:
        explicit TransactionModel(QDate date, qreal amount, QString category, QObject *parent = nullptr)
            : QObject(parent)
            , m_date(date)
            , m_amount(amount)
            , m_category(category) { }

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
        Q_OBJECT_BINDABLE_PROPERTY(TransactionModel, QDate, m_date, &TransactionModel::signalChanged);
        Q_OBJECT_BINDABLE_PROPERTY(TransactionModel, qreal, m_amount, &TransactionModel::signalChanged);
        Q_OBJECT_BINDABLE_PROPERTY(TransactionModel, QString, m_category, &TransactionModel::signalChanged);
    };
}
