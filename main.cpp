#include "MainWindow.hpp"
#include "Types.hpp"
#include "TransactionListWidget.hpp"
#include "TransactionListModel.hpp"

#include <QApplication>
#include <QBuffer>
#include <QVBoxLayout>

int main(int argc, char *argv[])
{
    QApplication application(argc, argv);

    qRegisterMetaType<Accounting::Id>();
    qRegisterMetaType<Accounting::Transaction>();

    MainWindow main_window;

    auto model = new Accounting::TransactionListModel;
    model->setParent(&main_window);

    model->append(Accounting::Transaction{
         .m_id = Accounting::Id::create_random(),
         .m_previous_version_id = std::nullopt,
         .m_timestamp = QDateTime::currentDateTime(),
         .m_category = "Groceries",
         .m_description = std::nullopt,
         .m_date = QDate(2022, 10, 24),
         .m_amount = 42.00,
     });

    auto transaction_list_widget = new Accounting::TransactionListWidget{ *model };
    transaction_list_widget->setParent(&main_window);

    main_window.setCentralWidget(transaction_list_widget);
    main_window.show();

    return application.exec();
}
