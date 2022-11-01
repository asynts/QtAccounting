#pragma once

#include <QWidget>
#include <QLayout>
#include <QLabel>
#include <QPushButton>

#include "Persistance/Database.hpp"
#include "Widgets/BillEditorDialog.hpp"

namespace Accounting::Widgets
{
    class BillListWidget final : public QWidget {
        Q_OBJECT

    public:
        BillListWidget(Persistance::Database& database, QWidget *parent = nullptr)
            : QWidget(parent)
            , m_database(database)
        {
            setLayout(new QVBoxLayout);

            m_container_widget = generateContainerWidget();
            layout()->addWidget(m_container_widget);

            connect(&m_database, &Persistance::Database::signalBillAdded,
                    this, &BillListWidget::slotUpdate);
        }

    private slots:
        void slotUpdate() {
            auto *container_widget = generateContainerWidget();
            layout()->replaceWidget(m_container_widget, container_widget);

            delete m_container_widget;
            m_container_widget = container_widget;
        }

    private:
        QWidget* generateContainerWidget() {
            auto container_widget = new QWidget(this);

            auto container_layout = new QVBoxLayout;
            container_layout->setAlignment(Qt::AlignTop);
            container_widget->setLayout(container_layout);

            for (auto *bill_object : std::as_const(m_database.m_bills)) {
                auto *bill_widget = new QWidget(container_widget);
                container_layout->addWidget(bill_widget);

                auto *bill_layout = new QHBoxLayout;
                bill_widget->setLayout(bill_layout);

                bill_layout->addWidget(new QLabel(QString("%1 (%2)").arg(bill_object->id(), bill_object->status_string()), container_widget));

                auto *edit_button = new QPushButton("Edit", container_widget);
                bill_layout->addWidget(edit_button);

                BillEditorDialog *dialog = nullptr;
                connect(edit_button, &QPushButton::clicked,
                        this, [=]() mutable {
                            if (dialog == nullptr) {
                                dialog = new BillEditorDialog(*bill_object, container_widget);

                                // FIXME: Connect 'signalComplete'.
                            }

                            dialog->show();
                            dialog->raise();
                            dialog->activateWindow();
                        });
            }

            return container_widget;
        }

        QWidget *m_container_widget = nullptr;

        Persistance::Database& m_database;
    };
}
