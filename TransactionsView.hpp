#pragma once

#include <QWidget>

#include "Types.hpp"

class TransactionsView : public QWidget
{
    Q_OBJECT

public:
    TransactionsView(QList<Accounting::Transaction> transactions, QWidget *parent = nullptr);
};
