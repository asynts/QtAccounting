#pragma once

#include <QSortFilterProxyModel>

#include "Entities/TransactionEntity.hpp"

namespace Accounting::Models
{
    class DatabaseModel;

    class PocketMoneyProxyModel final : public QSortFilterProxyModel {
        Q_OBJECT

    public:
        explicit PocketMoneyProxyModel(DatabaseModel *parent_database_model);

        virtual bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const override {
            auto sourceIndex = sourceModel()->index(sourceRow, 0, sourceParent);
            auto *transaction_model = reinterpret_cast<Entities::TransactionEntity*>(sourceIndex.internalPointer());

            return transaction_model->isPocketMoney();
        }

    private:
        using QSortFilterProxyModel::setSourceModel;
    };
}
