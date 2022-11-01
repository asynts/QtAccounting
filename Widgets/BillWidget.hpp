#pragma once

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QSplitter>

#include "Persistance/Database.hpp"
#include "Widgets/TransactionWidget.hpp"

namespace Accounting::Widgets
{
    class BillWidget final : public QWidget {
        Q_OBJECT

    public:
        BillWidget(QWidget *parent = nullptr)
            : QWidget(parent)
        {
            setLayout(new QVBoxLayout());

            update();
        }

        void setBillWidget(Persistance::BillObject& new_bill_object) {
            if (m_bill_object != nullptr) {
                disconnect(m_bill_object, &Persistance::BillObject::onChanged,
                           this, &BillWidget::update);
            }
            m_bill_object = &new_bill_object;

            connect(m_bill_object, &Persistance::BillObject::onChanged,
                    this, &BillWidget::update);
        }

    public slots:
        void update() {
            if (m_bill_object == nullptr) {
                return;
            }

            // Delete old widgets.
            for (auto *widget : m_widgets) {
                layout()->removeWidget(widget);
                delete widget;
            }
            m_widgets.clear();

            // Create new widgets.
            for (auto *transaction_object : m_bill_object->transactions()) {
                auto widget = new TransactionWidget(*transaction_object, this);
                layout()->addWidget(widget);

                m_widgets.append(widget);
            }
        }

    private:
        QList<QWidget*> m_widgets;

        Persistance::BillObject *m_bill_object = nullptr;
    };
}
