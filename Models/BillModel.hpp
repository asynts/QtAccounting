#pragma once

#include <QAbstractListModel>

#include "Persistance/Database.hpp"

namespace Accounting::Models
{
    class TransactionItem {
    public:
        Persistance::Database *m_database;
        Persistance::PersistantId m_persistant_id;

        TransactionItem()
            : m_database(nullptr)
            , m_persistant_id()
        {
        }

        TransactionItem(Persistance::Database& database, Persistance::PersistantId persistant_id)
            : m_database(&database)
            , m_persistant_id(persistant_id)
        {
        }

        const Persistance::Transaction& transaction() const {
            Q_ASSERT(m_database != nullptr);

            if (m_cached_transaction == nullptr) {
                auto qualified_id = m_database->m_version_lookup[m_persistant_id];
                m_cached_transaction = &m_database->m_transactions[qualified_id];
            }

            return *m_cached_transaction;
        }

        void invalidate() const {
            m_cached_transaction = nullptr;
        }

    private:
        mutable const Persistance::Transaction *m_cached_transaction;
    };

    class BillItem {
    public:
        Persistance::Database *m_database;
        Persistance::PersistantId m_persistant_id;

        BillItem()
            : m_database(nullptr)
            , m_persistant_id()
        {
        }

        BillItem(Persistance::Database& database, Persistance::PersistantId persistant_id)
            : m_database(&database)
            , m_persistant_id(persistant_id)
        {
        }

        const Persistance::Bill& bill() const {
            Q_ASSERT(m_database != nullptr);

            if (m_cached_bill == nullptr) {
                auto qualified_id = m_database->m_version_lookup[m_persistant_id];
                m_cached_bill = &m_database->m_bills[qualified_id];
            }

            return *m_cached_bill;
        }

        const QList<TransactionItem> transactions() const {
            Q_ASSERT(m_database != nullptr);

            if (!m_cached_transaction_items.has_value()) {
                m_cached_transaction_items = QList<TransactionItem>{};
                for (auto persistant_transaction_id : bill().m_transaction_ids) {
                    m_cached_transaction_items.value().append(TransactionItem{
                                                                  *m_database,
                                                                  persistant_transaction_id,
                                                              });
                }
            }

            return m_cached_transaction_items.value();
        }

        void invalidate() const {
            m_cached_bill = nullptr;
            m_cached_transaction_items->clear();
        }

    private:
        mutable const Persistance::Bill *m_cached_bill;
        mutable std::optional<QList<TransactionItem>> m_cached_transaction_items;
    };

    class BillModel final : public QAbstractListModel {
        Q_OBJECT

    public:
        BillModel(std::optional<BillItem> bill = std::nullopt)
            : m_bill(bill)
        {
        }

        int rowCount(const QModelIndex& parent = QModelIndex{}) const override {
            if (!m_bill.has_value()) {
                return 0;
            }

            return m_bill.value().bill().m_transaction_ids.size();
        }

        QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override {
            if (!index.isValid()) {
                return QVariant();
            }

            if (index.row() >= rowCount()) {
                return QVariant();
            }

            if (role == Qt::DisplayRole) {
                return QVariant::fromValue(m_bill.value().transactions()[index.row()]);
            } else {
                return QVariant();
            }
        }

        bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override {
            Q_UNREACHABLE();
        }

        QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override {
            Q_UNREACHABLE();
        }

    public slots:
        void billChanged(Persistance::QualifiedId new_qualified_bill_id, const Persistance::Bill& bill) {
            if (!m_bill.has_value()) {
                return;
            }

            if (new_qualified_bill_id.m_id != m_bill.value().m_persistant_id) {
                // That bill doesn't affect us.
                return;
            }

            beginResetModel();

            // FIXME: Avoid additional lookup using parameters.
            m_bill->invalidate();

            endResetModel();
        }

        void transactionChanged(Persistance::QualifiedId qualified_bill_id, Persistance::QualifiedId new_qualified_transaction_id, const Persistance::Transaction& transaction) {
            if (!m_bill.has_value()) {
                return;
            }

            if (m_bill.value().m_persistant_id != qualified_bill_id.m_id) {
                // This transaction does not belong to this bill.
                return;
            }

            int row = 0;
            for (auto& transaction_item : m_bill.value().transactions()) {
                if (transaction_item.m_persistant_id == new_qualified_transaction_id.m_id) {
                    transaction_item.invalidate();
                    emit dataChanged(createIndex(row, 0), createIndex(row, 0));
                    return;
                }
                ++row;
            }

            Q_UNREACHABLE();
        }

    private:
        std::optional<BillItem> m_bill;
    };
}

Q_DECLARE_METATYPE(Accounting::Models::TransactionItem);
Q_DECLARE_METATYPE(Accounting::Models::BillItem);
