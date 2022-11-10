#pragma once

#include <QSortFilterProxyModel>

#include "Models/TransactionModel.hpp"

namespace Accounting::Models
{
    class DatabaseModel;

    class PocketMoneyProxyModel final : public QSortFilterProxyModel {
        Q_OBJECT

    public:
        explicit PocketMoneyProxyModel(DatabaseModel *parent_database_model);

        virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override {
            // The default is to share the headers of the source model.
            // We want the row numbers to start at 1.
            if (orientation == Qt::Orientation::Vertical && role == Qt::DisplayRole) {
                return QString::number(section + 1);
            }

            return QSortFilterProxyModel::headerData(section, orientation, role);
        }

        virtual bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const override {
            auto sourceIndex = sourceModel()->index(sourceRow, 0, sourceParent);
            auto *transaction_model = reinterpret_cast<TransactionModel*>(sourceIndex.internalPointer());

            return transaction_model->isPocketMoney();
        }

    private:
        using QSortFilterProxyModel::setSourceModel;
    };
}
