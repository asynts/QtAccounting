#include "MainWindow.hpp"
#include "Types.hpp"
#include "TransactionsView.hpp"

#include <QApplication>
#include <QBuffer>
#include <QVBoxLayout>

int main(int argc, char *argv[])
{
    // Create a list of dummy transactions to display.
    QList<Accounting::Transaction> transactions;
    transactions.append(Accounting::Transaction{
        .m_id = Accounting::Id::create_random(),
        .m_previous_version_id = std::nullopt,
        .m_timestamp = QDateTime::currentDateTime(),
        .m_category = "Groceries",
        .m_description = std::nullopt,
        .m_date = QDate(2022, 10, 24),
        .m_amount = 42.00,
    });

    QApplication application(argc, argv);

    qRegisterMetaType<Accounting::Id>();
    qRegisterMetaType<Accounting::Transaction>();

    MainWindow main_window;

    auto transactions_view = new TransactionsView{ transactions };
    transactions_view->setParent(&main_window);

    main_window.setCentralWidget(transactions_view);
    main_window.show();

    return application.exec();
}
