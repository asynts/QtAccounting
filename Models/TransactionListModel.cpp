#include "Models/TransactionListModel.hpp"
#include "Models/DatabaseModel.hpp"

namespace Accounting::Models
{
    TransactionListModel::TransactionListModel(DatabaseModel *parent_database_model)
        : QAbstractItemModel(parent_database_model)
        , m_parent_database_model(parent_database_model) { }
}
