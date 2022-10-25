#include <QVBoxLayout>
#include <QLabel>

#include "TransactionsView.hpp"

TransactionsView::TransactionsView(Accounting::TransactionListModel &model, QWidget *parent)
    : QWidget(parent)
{
    auto layout = new QVBoxLayout();

    /*
    for (auto& transaction : transactions) {
        // Convert
        QString label_value;
        QDebug{ &label_value } << transaction;

        auto transaction_widget = new QLabel(label_value);
        transaction_widget->setParent(this);
        layout->addWidget(transaction_widget);
    }
    */

    this->setLayout(layout);
}
