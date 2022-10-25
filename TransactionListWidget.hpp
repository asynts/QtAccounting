#pragma once

#include <QWidget>
#include <QListView>
#include <QBoxLayout>
#include <QLabel>

#include "TransactionListModel.hpp"

namespace Accounting
{
    class TransactionListWidget : public QWidget
    {
        Q_OBJECT

    public:
        TransactionListWidget(TransactionListModel &model, QWidget *parent = nullptr)
            : QWidget(parent)
            , m_model(model)
        {
            auto layout = new QVBoxLayout;

            // FIXME: How does this thing know, what it should display?
            auto list_view_widget = new QListView;
            list_view_widget->setParent(this);
            list_view_widget->setModel(&model);
            layout->addWidget(list_view_widget);

            this->setLayout(layout);
        }

    private:
        TransactionListModel& m_model;
    };
}
