#pragma once

#include <QWidget>

#include "TransactionListModel.hpp"

class TransactionsView : public QWidget
{
    Q_OBJECT

public:
    TransactionsView(Accounting::TransactionListModel &model, QWidget *parent = nullptr);
};
