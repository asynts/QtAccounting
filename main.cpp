#include "MainWindow.hpp"
#include "Types.hpp"

#include <QApplication>
#include <QBuffer>

int main(int argc, char *argv[])
{
    // Create a transaction in memory.
    Accounting::Transaction transaction_1{
        .m_id = Accounting::Id::create_random(),
        .m_previous_version_id = std::nullopt,
        .m_timestamp = QDateTime::currentDateTime(),
        .m_category = "Groceries",
        .m_description = std::nullopt,
        .m_date = QDate(2022, 10, 24),
        .m_amount = 42.00,
    };

    qDebug() << transaction_1;

    QByteArray buffer;
    QDataStream stream{ &buffer, QIODevice::ReadWrite };

    // Serialize it into the buffer.
    stream << transaction_1;

    Accounting::Transaction transaction_2;

    // Deserialize it from the buffer.
    stream.device()->reset();
    stream >> transaction_2;

    qDebug() << transaction_2;

    QApplication application(argc, argv);

    qRegisterMetaType<Accounting::Id>();
    qRegisterMetaType<Accounting::Transaction>();

    MainWindow main_window;
    main_window.show();

    return application.exec();
}
