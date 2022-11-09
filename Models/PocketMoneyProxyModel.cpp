#include "Models/PocketMoneyProxyModel.hpp"
#include "Models/DatabaseModel.hpp"

namespace Accounting::Models
{
    PocketMoneyProxyModel::PocketMoneyProxyModel(DatabaseModel *parent_database_model)
        : QSortFilterProxyModel(parent_database_model)
    {
        setSourceModel(parent_database_model->transactionListModel());
    }
}
