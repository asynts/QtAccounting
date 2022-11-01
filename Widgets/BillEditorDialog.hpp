#pragma once

#include <QWidget>
#include <QLayout>
#include <QLabel>
#include <QPushButton>
#include <QDialog>

#include "Persistance/Database.hpp"

namespace Accounting::Widgets
{
    class BillEditorDialog final : public QDialog {
        Q_OBJECT

    public:
        BillEditorDialog(Persistance::BillObject& bill_object, QWidget *parent = nullptr)
            : QDialog(parent)
            , m_bill_object(bill_object)
        {
            setModal(false);
            setWindowTitle(QString("Editing Bill '%1'").arg(bill_object.id()));

            setLayout(new QVBoxLayout);

            m_container_widget = generateContainerWidget();
            layout()->addWidget(m_container_widget);

            auto *new_button = new QPushButton("New Transaction", this);
            layout()->addWidget(new_button);

            connect(new_button, &QPushButton::clicked,
                    this, &BillEditorDialog::slotNewTransaction);

            connect(&bill_object, &Persistance::BillObject::onChanged,
                    this, &BillEditorDialog::slotUpdate);
        }

    private slots:
        void slotUpdate() {
            auto *container_widget = generateContainerWidget();
            layout()->replaceWidget(m_container_widget, container_widget);

            delete m_container_widget;
            m_container_widget = container_widget;
        }

        void slotNewTransaction() {
            // FIXME: TransactionEditorDialog.
        }

    private:
        QWidget* generateContainerWidget() {
            auto container_widget = new QWidget(this);

            auto container_layout = new QVBoxLayout;
            container_widget->setLayout(container_layout);

            for (auto *transaction_object : m_bill_object.transactions()) {
                auto *transaction_widget = new QWidget(container_widget);
                container_layout->addWidget(transaction_widget);

                auto *transaction_layout = new QHBoxLayout;
                transaction_widget->setLayout(transaction_layout);

                transaction_layout->addWidget(new QLabel(QString("transaction: %1").arg(transaction_object->id()), container_widget));

                auto *edit_button = new QPushButton("Edit", container_widget);
                transaction_layout->addWidget(edit_button);

                connect(edit_button, &QPushButton::clicked,
                        this, [=] {
                            // FIXME: Open dialog to edit this transaction.
                        });
            }

            return container_widget;
        }

        QWidget *m_container_widget = nullptr;

        Persistance::BillObject& m_bill_object;
    };
}
