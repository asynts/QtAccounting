#pragma once

#include <QAbstractItemModel>

#include "Models/BillModel.hpp"
#include "Persistance/Database.hpp"

namespace Accounting::Models
{
    class DatabaseModel final : public QAbstractItemModel {
    public:
        enum Columns {
            ColumnDate,
            ColumnTotal,
            ColumnStatus,
            ColumnId,

            COLUMN_COUNT
        };

    private:
        Q_OBJECT

    public:
        explicit DatabaseModel(QObject *parent = nullptr)
            : QAbstractItemModel(parent) { }

        BillModel* createBill() {
            auto *bill_model = new BillModel(new_id(), QDate::currentDate(), BillModel::Status::Staged, QDateTime::currentMSecsSinceEpoch(), this);

            int row = m_bills.size();

            beginInsertRows(QModelIndex(), row, row);
            m_bills.append(bill_model);
            endInsertRows();

            connect(bill_model, &BillModel::signalChanged,
                    this, [=, this] {
                        emit dataChanged(index(row, 0), index(row, columnCount() - 1));
                    });

            return bill_model;
        }

        Persistance::Database serialize() const {
            QList<Persistance::Bill> serialized_bills;
            for (auto *bill_model : m_bills) {
                serialized_bills.append(bill_model->serialize());
            }

            return Persistance::Database{
                .m_bills = serialized_bills,
                .m_next_id = m_next_id,
            };
        }

        void deserialize(const Persistance::Database& value) {
            m_next_id = value.m_next_id;

            beginResetModel();

            for (auto *bill_model : m_bills) {
                bill_model->deleteLater();
            }
            m_bills.clear();

            for (auto& bill : value.m_bills) {
                auto *bill_model = new BillModel(this);
                bill_model->deserialize(bill);
                m_bills.append(bill_model);
            }

            endResetModel();
        }

        QString new_id() {
            return hash_and_stringify_id(m_next_id++);
        }

        virtual QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override {
            if (row < 0 || row >= rowCount()) {
                return QModelIndex();
            }

            return createIndex(row, column, reinterpret_cast<void*>(m_bills[row]));
        }

        virtual Qt::ItemFlags flags(const QModelIndex& index) const override {
            if (!checkIndex(index)) {
                return Qt::ItemFlag::NoItemFlags;
            }

            return Qt::ItemFlag::ItemIsSelectable
                 | Qt::ItemFlag::ItemIsEditable
                 | Qt::ItemFlag::ItemIsEnabled;
        }

        virtual QModelIndex parent(const QModelIndex& index) const override {
            return QModelIndex();
        }

        virtual int rowCount(const QModelIndex& parent = QModelIndex()) const override {
            return m_bills.size();
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

            if (section == Columns::ColumnId) {
                return "Id";
            } else if (section == Columns::ColumnStatus) {
                return "Status";
            } else if (section == Columns::ColumnDate) {
                return "Date";
            } else if (section == Columns::ColumnTotal) {
                return "Total";
            } else {
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

            auto *bill = m_bills[index.row()];

            if (index.column() == Columns::ColumnId) {
                return bill->id();
            } else if (index.column() == Columns::ColumnStatus) {
                return QVariant::fromValue(bill->status());
            } else if (index.column() == Columns::ColumnDate) {
                return bill->date().toString("yyyy-MM-dd");
            } else if (index.column() == Columns::ColumnTotal) {
                return QString::number(bill->totalAmount(), 'f', 2);
            }

            return QVariant();
        }

    private:
        QList<BillModel*> m_bills;
        quint64 m_next_id = 1;
    };
}
