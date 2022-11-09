#pragma once

#include <QAbstractItemModel>

#include "Models/BillProxyModel.hpp"
#include "Util.hpp"

namespace Accounting::Models
{
    class DatabaseModel;

    class BillListModel final : public QAbstractItemModel {
    public:
        enum Columns {
            ColumnDate,
            ColumnTotal,
            ColumnStatus,
            ColumnId,

            COLUMN_COUNT
        };
        Q_ENUM(Columns);

    private:
        Q_OBJECT

    public:
        explicit BillListModel(DatabaseModel *parent_database_model);

        void appendBill(BillProxyModel *bill) {
            bill->setParent(this);

            int index = m_owned_bills.size();

            beginInsertRows(QModelIndex(), index, index);
            m_owned_bills.append(bill);
            endInsertRows();
        }

        void deleteAll() {
            beginResetModel();
            m_owned_bills.clear();
            endResetModel();
        }

        const QList<BillProxyModel*> bills() const {
            return m_owned_bills;
        }

        BillProxyModel* createBill();

        virtual QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override {
            if (row < 0 || row >= rowCount()) {
                return QModelIndex();
            }

            return createIndex(row, column, reinterpret_cast<void*>(m_owned_bills[row]));
        }

        virtual QModelIndex parent(const QModelIndex& index) const override {
            return QModelIndex();
        }

        virtual int rowCount(const QModelIndex& parent = QModelIndex()) const override {
            return m_owned_bills.size();
        }

        virtual int columnCount(const QModelIndex& parent = QModelIndex()) const override {
            return Columns::COLUMN_COUNT;
        }

        virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override {
            if (orientation == Qt::Orientation::Vertical) {
                return QAbstractItemModel::headerData(section, orientation, role);
            }

            if (role != Qt::DisplayRole) {
                return QVariant();
            }

            switch (section) {
            case ColumnDate:
                return "Date";
            case ColumnTotal:
                return "Total";
            case ColumnStatus:
                return "Status";
            case ColumnId:
                return "Id";
            default:
                Q_UNREACHABLE();
            }
        }

        virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override {
            if (!checkIndex(index)) {
                return QVariant();
            }

            if (role != Qt::DisplayRole) {
                return QVariant();
            }

            auto *bill_entity = m_owned_bills[index.row()];

            if (index.column() == Columns::ColumnId) {
                return bill_entity->id();
            } else if (index.column() == Columns::ColumnStatus) {
                return QVariant::fromValue(bill_entity->status());
            } else if (index.column() == Columns::ColumnDate) {
                return bill_entity->date().toString("yyyy-MM-dd");
            } else if (index.column() == Columns::ColumnTotal) {
                return QString::number(bill_entity->totalAmount(), 'f', 2);
            }

            return QVariant();
        }

    private:
        QList<BillProxyModel*> m_owned_bills;

        DatabaseModel *m_parent_database_model;
    };
}
