#pragma once

#include <QWidget>
#include <QLayout>
#include <QLabel>
#include <QPushButton>

#include "Persistance/Database.hpp"
#include "Widgets/BillEditorDialog.hpp"
#include "ui_BillListWidget.h"

namespace Accounting::Widgets
{
    class BillListWidget final : public QWidget {
        Q_OBJECT

    public:
        BillListWidget(Persistance::DatabaseObject *database_object, QWidget *parent = nullptr)
            : QWidget(parent)
            , m_database_object(database_object)
        {
            m_ui.setupUi(this);

            connect(m_database_object, &Persistance::DatabaseObject::signalChanged,
                    this, &BillListWidget::slotUpdate);

            connect(m_ui.m_new_QPushButton, &QPushButton::clicked,
                    this, &BillListWidget::slotNewBill);

            slotUpdate();
        }

    private slots:
        void slotUpdate() {
            auto *container_widget = generateContainerWidget();
            layout()->replaceWidget(m_ui.m_container_QWidget, container_widget);

            m_ui.m_container_QWidget->deleteLater();
            m_ui.m_container_QWidget = container_widget;
        }

        void slotNewBill() {
            m_database_object->createBill();
        }

    private:
        QWidget* generateContainerWidget() {
            auto container_widget = new QWidget(this);

            auto container_layout = new QVBoxLayout;
            container_layout->setAlignment(Qt::AlignTop);
            container_widget->setLayout(container_layout);

            for (auto *bill_object : m_database_object->bills()) {
                auto *bill_widget = new QWidget(container_widget);
                container_layout->addWidget(bill_widget);

                auto *bill_layout = new QHBoxLayout;
                bill_widget->setLayout(bill_layout);

                auto label_text = QString("%1 (%2)").arg(bill_object->id(), Persistance::BillObject::status_to_string(bill_object->status()));
                bill_layout->addWidget(new QLabel(label_text, container_widget));

                auto *edit_button = new QPushButton("Edit", container_widget);
                bill_layout->addWidget(edit_button);

                BillEditorDialog *dialog = nullptr;
                connect(edit_button, &QPushButton::clicked,
                        this, [=]() mutable {
                            if (dialog == nullptr) {
                                dialog = new BillEditorDialog(bill_object, container_widget);
                            }

                            dialog->show();
                            dialog->raise();
                            dialog->activateWindow();
                        });
            }

            return container_widget;
        }

        Ui::BillListWidget m_ui;

        Persistance::DatabaseObject *m_database_object;
    };
}
