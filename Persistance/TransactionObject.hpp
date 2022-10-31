#pragma once

#include <QObject>

#include "Persistance/Database.hpp"

namespace Accounting::Persistance
{
    // FIXME: Find a better name for this file.

    class NewDatabase;

    template<typename T>
    class TrackedObject final : public QObject {
        Q_OBJECT

    public:
        TrackedObject(const T& value, QObject *parent = nullptr)
            : QObject(parent)
            , m_value(&value)
        {
        }

        const T& get() const {
            Q_ASSERT(m_value != nullptr);
            return *m_value;
        }

    public slots:
        void notifyNewVersion(const T& value)
        {
            m_value = &value;
            emit onChanged();
        }

    signals:
        void onChanged();

    private:
        const T *m_value;
    };

    class NewDatabase : public QObject {
        Q_OBJECT

    public:
        NewDatabase(QObject *parent = nullptr)
            : QObject(parent)
        {
        }

        void insert_transaction(const Transaction& transaction) {
            _insert<Transaction>(transaction, m_transactions, m_transaction_trackers);
        }

        void insert_bill(const Bill& bill) {
            _insert<Bill>(bill, m_bills, m_bill_trackers);
        }

        auto get_transaction(PersistantId id) const {
            return _get<Transaction>(id, m_transaction_trackers);
        }

        auto get_bill(PersistantId id) const {
            return _get<Bill>(id, m_bill_trackers);
        }

    private:
        template<typename T>
        void _insert(const T& value, QMap<QualifiedId, T>& values, QMap<PersistantId, TrackedObject<T>*>& trackers) {
            auto inserted_value = values.insert(value.m_id, value);

            auto tracker = trackers.find(value.m_id.m_id);
            if (tracker == trackers.end()) {
                // Create new tracker object.
                trackers.insert(
                            inserted_value.value().m_id.m_id,
                            new TrackedObject<T>(inserted_value.value(), this));
            } else {
                // Notify existing tracker object.
                tracker.value()->notifyNewVersion(inserted_value.value());
            }
        }

        template<typename T>
        const TrackedObject<T>* _get(PersistantId id, const QMap<PersistantId, TrackedObject<T>*>& trackers) const {
            auto iterator = trackers.find(id);
            if (iterator == trackers.end()) {
                return nullptr;
            } else {
                return iterator.value();
            }
        }

        // This is what we serialize to disk.
        QMap<QualifiedId, Transaction> m_transactions;
        QMap<QualifiedId, Bill> m_bills;

        // This keeps track of the most recent version.
        QMap<PersistantId, TrackedObject<Transaction>*> m_transaction_trackers;
        QMap<PersistantId, TrackedObject<Bill>*> m_bill_trackers;
    };
}
