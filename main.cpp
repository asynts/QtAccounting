#include "MainWindow.hpp"
#include "Types.hpp"

#include <QApplication>

int main(int argc, char *argv[])
{
    qDebug() << "result: " << Accounting::Id::create_random();
    qDebug() << "result: " << Accounting::Id::create_random();
    qDebug() << "result: " << Accounting::Id::create_random();
    qDebug() << "result: " << Accounting::Id::create_random();

    QApplication application(argc, argv);

    qRegisterMetaType<Accounting::Id>();

    MainWindow main_window;
    main_window.show();

    return application.exec();
}
