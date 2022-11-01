#pragma once

#include <QDialog>

#include "Persistance/Database.hpp"

namespace Accounting::Widgets
{
    class TransactionEditorDialog final : public QDialog {
        Q_OBJECT

    public:
        TransactionEditorDialog(Persistance::TransactionObject *transaction_object, QWidget *parent = nullptr)
            : QDialog(parent)
        {
            if (transaction_object == nullptr) {
                setWindowTitle("New Transaction");
            } else {
                setWindowTitle(QString("Edit Transaction '%1'").arg(transaction_object->id()));
            }
        }

    signals:
        void signalComplete(Persistance::TransactionData);
    };
}
