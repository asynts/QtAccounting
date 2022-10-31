#pragma once

#include <QWidget>
#include <QLabel>
#include <QHBoxLayout>

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

            m_label_widget = new QLabel("", this);;
            layout->addWidget(m_label_widget);

            update();

            setLayout(layout);

            connect(&transaction_object, &Persistance::TransactionObject::onChanged,
                    this, &TransactionWidget::update);
        }

    public slots:
        void update() {
            // FIXME: Somehow, this text is being truncated.
            m_label_widget->setText(QString("%1 (%2)").arg(m_transaction_object.id(), QString::number(m_transaction_object.amount())));
            qDebug() << m_label_widget->text();
        }

    private:
        QLabel *m_label_widget;

        Persistance::TransactionObject& m_transaction_object;
    };
}
