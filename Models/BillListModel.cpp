#include "Models/BillListModel.hpp"
#include "Models/DatabaseModel.hpp"

namespace Accounting::Models {
    BillListModel::BillListModel(DatabaseModel *parent_database_model)
        : QAbstractItemModel(parent_database_model)
        , m_parent_database_model(parent_database_model) { }
}
