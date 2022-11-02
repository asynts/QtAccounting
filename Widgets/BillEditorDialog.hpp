#pragma once

#include <QWidget>
#include <QLayout>
#include <QLabel>
#include <QPushButton>
#include <QDialog>

#include "Persistance/Database.hpp"
#include "Widgets/TransactionEditorDialog.hpp"
#include "ui_BillEditorDialog.h"

namespace Accounting::Widgets
{
    class BillEditorDialog final : public QDialog {
        Q_OBJECT

    public:
        BillEditorDialog(Persistance::BillObject *bill_object, QWidget *parent = nullptr)
            : QDialog(parent)
            , m_bill_object(bill_object)
        {
            m_ui.setupUi(this);

            setWindowTitle(QString("Edit Bill '%1'").arg(bill_object->id()));

            fill_QComboBox_with_enum<Persistance::BillObject::Status>(m_ui.m_status_QComboBox);

            connect(m_ui.m_status_QComboBox, &QComboBox::currentIndexChanged,
                    this, &BillEditorDialog::slotStatusChanged);

            connect(m_ui.m_new_QPushButton, &QPushButton::clicked,
                    this, &BillEditorDialog::slotNewTransaction);

            connect(bill_object, &Persistance::BillObject::signalChanged,
                    this, &BillEditorDialog::slotUpdate);

            slotUpdate();
        }

    private slots:
        void slotUpdate() {
            // This is not an infinite loop, since we only emit 'signalStatusChanged' if the index changes.
            m_ui.m_status_QComboBox->setCurrentIndex(static_cast<int>(m_bill_object->status()));

            auto *container_widget = generateContainerWidget();
            layout()->replaceWidget(m_ui.m_container_QWidget, container_widget);

            m_ui.m_container_QWidget->deleteLater();
            m_ui.m_container_QWidget = container_widget;
        }

        void slotStatusChanged() {
            m_bill_object->setStatus(static_cast<Persistance::BillObject::Status>(m_ui.m_status_QComboBox->currentIndex()));
        }

        void slotNewTransaction() {
            if (m_new_transaction_dialog == nullptr) {
                m_new_transaction_dialog = new TransactionEditorDialog(m_bill_object, nullptr, this);
            }

            m_new_transaction_dialog->show();
            m_new_transaction_dialog->raise();
            m_new_transaction_dialog->activateWindow();
        }

    private:
        QWidget* generateContainerWidget() {
            auto container_widget = new QWidget(this);

            auto container_layout = new QVBoxLayout;
            container_layout->setAlignment(Qt::AlignTop);
            container_widget->setLayout(container_layout);

            for (auto *transaction_object : m_bill_object->transactions()) {
                // FIXME: Move this into 'TransactionWidget'
                //        That is necessary because otherwise the values won't be updated.

                auto *transaction_widget = new QWidget(container_widget);
                container_layout->addWidget(transaction_widget);

                auto *transaction_layout = new QHBoxLayout;
                transaction_widget->setLayout(transaction_layout);

                transaction_layout->addWidget(new QLabel(QString("transaction: %1").arg(transaction_object->id()), container_widget));

                auto *edit_button = new QPushButton("Edit", container_widget);
                transaction_layout->addWidget(edit_button);

                TransactionEditorDialog *dialog = nullptr;
                connect(edit_button, &QPushButton::clicked,
                        this, [=, bill_object = m_bill_object]() mutable {
                            if (dialog == nullptr) {
                                dialog = new TransactionEditorDialog(bill_object, transaction_object, container_widget);
                            }

                            dialog->show();
                            dialog->raise();
                            dialog->activateWindow();
                        });
            }

            return container_widget;
        }

        Ui::BillEditorDialog m_ui;

        TransactionEditorDialog *m_new_transaction_dialog = nullptr;

        Persistance::BillObject *const m_bill_object;
    };
}
