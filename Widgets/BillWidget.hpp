#pragma once

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>

#include "Persistance/Database.hpp"
#include "Widgets/TransactionWidget.hpp"

namespace Accounting::Widgets
{
    class BillWidget final : public QWidget {
        Q_OBJECT

    public:
        BillWidget(Persistance::BillObject& bill_object, QWidget *parent = nullptr)
            : QWidget(parent)
            , m_bill_object(bill_object)
        {
            setLayout(new QVBoxLayout());

            update();

            connect(&bill_object, &Persistance::BillObject::onChanged,
                    this, &BillWidget::update);
        }

    public slots:
        void update() {
            // Delete old widgets.
            for (auto *widget : m_widgets) {
                layout()->removeWidget(widget);
                delete widget;
            }
            m_widgets.clear();

            // Create new widgets.
            for (auto *transaction_object : m_bill_object.transactions()) {
                auto widget = new TransactionWidget(*transaction_object, this);
                m_widgets.append(widget);
            }
        }

    private:
        QList<QWidget*> m_widgets;

        Persistance::BillObject& m_bill_object;
    };
}
