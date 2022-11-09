#include "Models/BillProxyModel.hpp"
#include "Models/DatabaseModel.hpp"

namespace Accounting::Models
{
    BillProxyModel::BillProxyModel(DatabaseModel *database_model, QObject *parent)
        : QSortFilterProxyModel(parent)
        , m_creation_timestamp(QDateTime::currentMSecsSinceEpoch())
        , m_database_model(database_model)
    {
        setSourceModel(database_model->transactionListModel());
    }

    Entities::TransactionEntity* BillProxyModel::createTransaction(
            QDate date,
            qreal amount,
            QString category,
            Entities::TransactionEntity::Status status,
            bool is_pocket_money)
    {
        auto *transaction = new Entities::TransactionEntity(this);
        transaction->setId(m_database_model->consume_next_id());
        transaction->setParentBillId(id());
        transaction->setDate(date);
        transaction->setAmount(amount);
        transaction->setCategory(category);
        transaction->setStatus(status);
        transaction->setIsPocketMoney(is_pocket_money);

        m_database_model->transactionListModel()->appendTransaction(transaction);

        return transaction;
    }
}
