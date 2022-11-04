#pragma once

#include <QAbstractItemModel>

#include "Models/TransactionModel.hpp"

namespace Accounting::Models
{
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
