#include "Models/AbstractTransactionListModel.hpp"
#include "Models/DatabaseModel.hpp"

namespace Accounting::Models
{
    AbstractTransactionListModel::AbstractTransactionListModel(DatabaseModel *parent_database_model)
        : QAbstractItemModel(parent_database_model)
        , m_parent_database_model(parent_database_model) { }
}
