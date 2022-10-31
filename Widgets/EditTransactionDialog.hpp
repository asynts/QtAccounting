#pragma once

#include <QDialog>
#include <QVBoxLayout>
#include <QLabel>

#include "Persistance/Database.hpp"
#include "ui_EditTransactionDialog.h"

namespace Accounting::Widgets
{
    class EditTransactionDialog final : public QDialog {
        Q_OBJECT

    public:
        EditTransactionDialog(Persistance::TransactionObject& transaction_object, QWidget *parent = nullptr)
            : QDialog(parent)
            , m_transaction_object(transaction_object)
        {
            setModal(false);

            Ui::EditTransactionDialog ui;
            ui.setupUi(this);

            m_category_widget = ui.m_category_LineEdit;
            m_date_widget = ui.m_date_DateEdit;
            m_amount_widget = ui.m_amount_LineEdit;
            m_type_widget = ui.m_type_ComboBox;

            // FIXME: Setup bindings for interpreted values.
            // FIXME: Disable 'ok' button if invalid (binding).
        }

    private slots:
        virtual void accept() override {
            // FIXME: This has already been validated, simply update the transaction object.
        }

    private:
        Persistance::TransactionObject& m_transaction_object;

        QLineEdit *m_category_widget;
        QDateEdit *m_date_widget;
        QLineEdit *m_amount_widget;
        QComboBox *m_type_widget;
    };
}
