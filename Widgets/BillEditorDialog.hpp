#pragma once

#include <QWidget>
#include <QLayout>
#include <QLabel>
#include <QPushButton>
#include <QDialog>

#include "Persistance/Database.hpp"
#include "Widgets/TransactionEditorDialog.hpp"

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
            setWindowTitle(QString("Edit Bill '%1'").arg(bill_object.id()));

            setLayout(new QVBoxLayout);

            // FIXME: Use '.ui' file.

            {
                m_container_widget = generateContainerWidget();
                layout()->addWidget(m_container_widget);
            }

            {
                auto *new_button = new QPushButton("New Transaction", this);
                layout()->addWidget(new_button);

                connect(new_button, &QPushButton::clicked,
                        this, &BillEditorDialog::slotNewTransaction);
            }

            {
                auto *dialog_buttons = new QDialogButtonBox(QDialogButtonBox::Ok, this);
                layout()->addWidget(dialog_buttons);

                connect(dialog_buttons, &QDialogButtonBox::accepted,
                        this, &BillEditorDialog::accept);
            }

            connect(&bill_object, &Persistance::BillObject::signalChanged,
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
            if (m_new_transaction_dialog == nullptr) {
                m_new_transaction_dialog = new TransactionEditorDialog(nullptr, this);

                connect(m_new_transaction_dialog, &TransactionEditorDialog::signalComplete,
                        this, [=, this](Persistance::TransactionData transaction_data) {
                            m_bill_object.create_transaction(std::move(transaction_data));
                        });
            }

            m_new_transaction_dialog->show();
            m_new_transaction_dialog->raise();
            m_new_transaction_dialog->activateWindow();
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

                TransactionEditorDialog *dialog = nullptr;
                connect(edit_button, &QPushButton::clicked,
                        this, [=]() mutable {
                            if (dialog == nullptr) {
                                dialog = new TransactionEditorDialog(transaction_object, container_widget);

                                connect(dialog, &TransactionEditorDialog::signalComplete,
                                        container_widget, [=](Persistance::TransactionData transaction_data) {
                                            transaction_object->slotUpdate(transaction_data);
                                        });
                            }

                            dialog->show();
                            dialog->raise();
                            dialog->activateWindow();
                        });
            }

            return container_widget;
        }

        TransactionEditorDialog *m_new_transaction_dialog = nullptr;
        QWidget *m_container_widget = nullptr;

        Persistance::BillObject& m_bill_object;
    };
}
