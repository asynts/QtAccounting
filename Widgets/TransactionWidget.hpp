#pragma once

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>

#include "Persistance/Database_2.hpp"

namespace Accounting::Widgets
{
    class TransactionWidget final : public QWidget {
        Q_OBJECT

    public:
        TransactionWidget(Persistance::TransactionObject& transaction_object, QWidget *parent = nullptr)
            : QWidget(parent)
            , m_transaction_object(transaction_object)
        {
            connect(&transaction_object, &Persistance::TransactionObject::onChanged,
                    this, &TransactionWidget::onTransactionObjectChanged);

            auto layout = new QVBoxLayout();

            m_label_widget = new QLabel("", this);;
            layout->addWidget(m_label_widget);

            onTransactionObjectChanged();

            setLayout(layout);
        }

    public slots:
        void onTransactionObjectChanged() {
            m_label_widget->setText(m_transaction_object.get().m_id.m_id.m_value);
        }

    private:
        QLabel *m_label_widget;

        Persistance::TransactionObject& m_transaction_object;
    };
}
