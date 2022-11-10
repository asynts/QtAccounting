#include "Models/TransactionModel.hpp"
#include "Models/DatabaseModel.hpp"

namespace Accounting::Models
{
    void TransactionModel::deleteMyself() {
        m_database_model->transactionListModel()->deleteTransaction(this);
    }
}
