#pragma once

#include <QAbstractItemModel>

#include "Models/TransactionListModel.hpp"
#include "Models/PocketMoneyProxyModel.hpp"
#include "Persistance/Database.hpp"

namespace Accounting::Models
{
    class DatabaseModel final : public QObject {
        Q_OBJECT

    public:
        explicit DatabaseModel(QObject *parent = nullptr)
            : QObject(parent)
        {
            m_transactionList_model = new TransactionListModel(this);
            m_pocketMoney_model = new PocketMoneyProxyModel(this);
        }

        Persistance::Database serialize() const {
            // FIXME
            Q_UNREACHABLE();
        }

        void deserialize(const Persistance::Database& value) {
            // FIXME
            Q_UNREACHABLE();
        }

        QString consume_next_id() {
            return hash_and_stringify_id(m_next_id++);
        }

        TransactionListModel* transactionListModel() {
            return m_transactionList_model;
        }

        PocketMoneyProxyModel* pocketMoneyModel() {
            return m_pocketMoney_model;
        }

    private:
        // FIXME: m_billList_model

        TransactionListModel *m_transactionList_model;
        PocketMoneyProxyModel *m_pocketMoney_model;

        quint64 m_next_id = 1;
    };
}
