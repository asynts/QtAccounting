#pragma once

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>

#include "Persistance/TransactionObject.hpp"

namespace Accounting::Widgets
{
    class BillWidget final : public QWidget {
        Q_OBJECT

    public:
        BillWidget(Persistance::BillObject& bill_object, QWidget *parent = nullptr)
            : QWidget(parent)
            , m_bill_object(bill_object)
        {
            connect(&bill_object, &Persistance::BillObject::onChanged,
                    this, &BillWidget::onBillObjectChanged);

            auto layout = new QVBoxLayout();

            m_label_widget = new QLabel("", this);;
            layout->addWidget(m_label_widget);

            onBillObjectChanged();

            setLayout(layout);
        }

    public slots:
        void onBillObjectChanged() {
            m_label_widget->setText(m_bill_object.get().m_id.m_id.m_value);
        }

    private:
        QLabel *m_label_widget;

        Persistance::BillObject& m_bill_object;
    };
}
