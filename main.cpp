#include <QApplication>
#include <QBuffer>
#include <QVBoxLayout>
#include <QSettings>

#include "MainWindow.hpp"

int main(int argc, char *argv[])
{
    QCoreApplication::setOrganizationName("Asynts");
    QCoreApplication::setOrganizationDomain("asynts.com");
    QCoreApplication::setApplicationName("Accounting");

    QApplication application(argc, argv);

    Accounting::MainWindow main_window;
    main_window.show();

    return application.exec();
}
