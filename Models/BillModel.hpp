#pragma once

#include <QAbstractItemModel>
#include <QDate>
#include <QBindable>

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

    class BillModel final : public QAbstractItemModel {
        Q_OBJECT

    public:
        explicit BillModel(QObject *parent = nullptr)
            : QAbstractItemModel(parent) { }

        virtual QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override {
            if (row < 0 || row >= rowCount()) {
                return QModelIndex();
            }

            return createIndex(row, column, reinterpret_cast<void*>(m_transactions[row]));
        }

        virtual QModelIndex parent(const QModelIndex& index) const override {
            return QModelIndex();
        }

        virtual int rowCount(const QModelIndex& parent = QModelIndex()) const override {
            return m_transactions.size();
        }

        virtual int columnCount(const QModelIndex& parent = QModelIndex()) const override {
            // Date
            // Expense
            // Amount
            // Category
            return 4;
        }

        virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override {
            if (index.row() < 0 || index.row() > rowCount()) {
                return QVariant();
            }

            if (role != Qt::DisplayRole) {
                return QVariant();
            }

            auto *transaction = m_transactions[index.row()];

            if (index.column() == 0) {
                return transaction->date();
            }

            if (index.column() == 1) {
                if (transaction->amount() <= 0.00) {
                    return "EXPENSE";
                } else {
                    return "INCOME";
                }
            }

            if (index.column() == 2) {
                return std::abs(transaction->amount());
            }

            if (index.column() == 3) {
                return transaction->category();
            }

            return QVariant();
        }

    public slots:
        void createTransaction(QDate date, qreal amount, QString category) {
            auto *transaction_object = new TransactionModel(date, amount, category, this);

            int row = m_transactions.size();
            m_transactions.append(transaction_object);

            connect(transaction_object, &TransactionModel::signalChanged,
                    this, [=, this]() {
                        emit dataChanged(index(row, 0), index(row, columnCount() - 1));
                    });
        }

    private:
        QList<TransactionModel*> m_transactions;
    };
}
