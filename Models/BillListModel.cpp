#include "Models/BillListModel.hpp"
#include "Models/DatabaseModel.hpp"

namespace Accounting::Models {
    BillListModel::BillListModel(DatabaseModel *parent_database_model)
        : QAbstractItemModel(parent_database_model)
        , m_parent_database_model(parent_database_model) { }

    BillProxyModel* BillListModel::createBill() {
        auto bill = new BillProxyModel(m_parent_database_model, this);
        bill->setId(m_parent_database_model->consume_next_id());
        bill->setStatus(BillProxyModel::Status::Staged);
        bill->setDate(QDate::currentDate());

        appendBill(bill);

        return bill;
    }
}
