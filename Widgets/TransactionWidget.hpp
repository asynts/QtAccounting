#pragma once

#include <QWidget>
#include <QLabel>
#include <QHBoxLayout>
#include <QPushButton>

#include "Persistance/Database.hpp"

namespace Accounting::Widgets
{
    class TransactionWidget final : public QWidget {
        Q_OBJECT

    public:
        TransactionWidget(Persistance::TransactionObject& transaction_object, QWidget *parent = nullptr)
            : QWidget(parent)
            , m_transaction_object(transaction_object)
        {
            auto layout = new QHBoxLayout();

            m_expense_widget = new QLabel("", this);
            layout->addWidget(m_expense_widget);
            m_amount_widget = new QLabel("", this);
            layout->addWidget(m_amount_widget);
            m_category_widget = new QLabel("", this);
            layout->addWidget(m_category_widget);
            m_id_widget = new QLabel("", this);
            layout->addWidget(m_id_widget);
            m_edit_widget = new QPushButton("Edit", this);
            layout->addWidget(m_edit_widget);

            update();

            setLayout(layout);

            connect(&transaction_object, &Persistance::TransactionObject::onChanged,
                    this, &TransactionWidget::update);
        }

    public slots:
        void update() {
            if (m_transaction_object.amount() < 0) {
                m_expense_widget->setText("EXPENSE");
            } else {
                m_expense_widget->setText("INCOME");
            }

            m_amount_widget->setText(QString::number(std::abs(m_transaction_object.amount()), 'f', 2));

            m_id_widget->setText(m_transaction_object.id());

            m_category_widget->setText(m_transaction_object.category());
        }

    private:
        QLabel *m_expense_widget;
        QLabel *m_amount_widget;
        QLabel *m_category_widget;
        QLabel *m_id_widget;
        QPushButton *m_edit_widget;

        Persistance::TransactionObject& m_transaction_object;
    };
}
