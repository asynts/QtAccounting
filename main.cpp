#include <QApplication>
#include <QBuffer>
#include <QVBoxLayout>

#include "MainWindow.hpp"

int main(int argc, char *argv[])
{
    QApplication application(argc, argv);

    Accounting::MainWindow main_window;
    main_window.show();

    return application.exec();
}
