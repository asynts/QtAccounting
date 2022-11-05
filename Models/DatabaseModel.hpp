#pragma once

#include <QAbstractItemModel>

#include "Models/BillModel.hpp"

namespace Accounting::Models
{
    // FIXME: Rename to 'BillListModel'.
    class DatabaseModel final : public QAbstractItemModel {
        Q_OBJECT

    public:
        explicit DatabaseModel(QObject *parent = nullptr)
            : QAbstractItemModel(parent) { }

        BillModel* createBill() {
            auto *bill_model = new BillModel(generate_id(), QDate::currentDate(), BillModel::Status::Staged, this);

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
            // Id
            // Status
            // Date
            return 3;
        }

        virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override {
            if (!checkIndex(index)) {
                return QVariant();
            }

            if (role != Qt::DisplayRole) {
                return QVariant();
            }

            auto *bill = m_bills[index.row()];

            if (index.column() == 0) {
                return bill->id();
            }

            if (index.column() == 1) {
                return QVariant::fromValue(bill->status());
            }

            if (index.column() == 2) {
                return bill->date().toString("yyyy-MM-dd");
            }

            return QVariant();
        }

    private:
        QList<BillModel*> m_bills;
    };
}
