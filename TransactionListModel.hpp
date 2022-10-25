#pragma once

#include <QAbstractListModel>
#include <QList>

#include "Types.hpp"

namespace Accounting
{
    class TransactionListModel final : public QAbstractListModel
    {
    public:
        TransactionListModel() = default;

        int rowCount(const QModelIndex& parent = QModelIndex{}) const override {
            return this->m_transactions.size();
        }

        QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override {
            return QVariant::fromValue(this->m_transactions[index.row()]);
        }

        bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override {
            m_transactions[index.row()] = value.value<Transaction>();
            return true;
        }

    public slots:
        void append(Accounting::Transaction transaction) {
            int new_row_index = this->m_transactions.size();

            this->beginInsertRows(QModelIndex{}, new_row_index, new_row_index + 1);
            this->m_transactions.append(transaction);
            this->endInsertRows();
        }

    private:
        QList<Transaction> m_transactions;
    };
}
