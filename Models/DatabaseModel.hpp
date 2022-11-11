#pragma once

#include <QAbstractItemModel>

#include "Models/TransactionListModel.hpp"
#include "Models/PocketMoneyProxyModel.hpp"
#include "Models/BillListModel.hpp"
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
            m_billList_model = new BillListModel(this);
        }

        Persistance::Database serialize() const {
            QList<Persistance::Transaction> transactions;
            for (auto *transaction_model : m_transactionList_model->transactions()) {
                transactions.append(transaction_model->serialize());
            }

            QList<Persistance::Bill> bills;
            for (auto *bill_model : m_billList_model->bill_models()) {
                bills.append(bill_model->serialize());
            }

            return Persistance::Database{
                .m_bills = bills,
                .m_next_id = m_next_id,
                .m_transactions = transactions,
            };
        }

        void deserialize(const Persistance::Database& value) {
            m_billList_model->deleteAll();
            m_transactionList_model->deleteAll();

            for (auto& transaction : value.m_transactions) {
                auto *transaction_model = new TransactionModel(this);
                transaction_model->deserialize(transaction);
                m_transactionList_model->appendTransaction(transaction_model);
            }

            for (auto& bill : value.m_bills) {
                auto *bill_model = new BillProxyModel(this);
                bill_model->deserialize(bill);
                m_billList_model->appendBill(bill_model);
            }

            m_next_id = value.m_next_id;
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

        BillListModel* billListModel() {
            return m_billList_model;
        }

    private:
        TransactionListModel *m_transactionList_model;
        BillListModel *m_billList_model;
        PocketMoneyProxyModel *m_pocketMoney_model;

        quint32 m_next_id = 1;
    };
}
