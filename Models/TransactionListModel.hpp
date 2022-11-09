#pragma once

#include <QAbstractItemModel>

#include "Entities/TransactionEntity.hpp"
#include "Util.hpp"

namespace Accounting::Models
{
    class DatabaseModel;

    class TransactionListModel final : public QAbstractItemModel {
    public:
        enum Columns {
            ColumnDate,
            ColumnAmount,
            ColumnCategory,
            ColumnStatus,
            ColumnId,

            COLUMN_COUNT
        };
        Q_ENUM(Columns);

    private:
        Q_OBJECT

    public:
        explicit TransactionListModel(DatabaseModel *parent_database_model);

        void appendTransaction(Entities::TransactionEntity *transaction) {
            int index = m_owned_transaction_models.size();

            beginInsertRows(QModelIndex(), index, index);
            m_owned_transaction_models.append(transaction);
            endInsertRows();
        }

        virtual QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override {
            if (row < 0 || row >= rowCount()) {
                return QModelIndex();
            }

            return createIndex(row, column, reinterpret_cast<void*>(m_owned_transaction_models[row]));
        }

        virtual QModelIndex parent(const QModelIndex& index) const override {
            return QModelIndex();
        }

        virtual int rowCount(const QModelIndex& parent = QModelIndex()) const override {
            return m_owned_transaction_models.size();
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

            return enum_type_to_string<Columns>(section);
        }

        virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override {
            if (index.row() < 0 || index.row() > rowCount()) {
                return QVariant();
            }

            if (role != Qt::DisplayRole) {
                return QVariant();
            }

            auto *transaction_model = m_owned_transaction_models[index.row()];

            if (index.column() == Columns::ColumnDate) {
                return transaction_model->date().toString("yyyy-MM-dd");
            } else if (index.column() == Columns::ColumnAmount) {
                return QString::number(transaction_model->amount(), 'f', 2);
            } else if (index.column() == Columns::ColumnCategory) {
                return transaction_model->category();
            } else if (index.column() == Columns::ColumnId) {
                return transaction_model->id();
            } else if (index.column() == Columns::ColumnStatus) {
                return enum_type_to_string(transaction_model->status());
            }

            return QVariant();
        }

    private:
        QList<Entities::TransactionEntity*> m_owned_transaction_models;

        DatabaseModel *m_parent_database_model;
    };
}
