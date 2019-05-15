#include "canudpbridge.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    CanUdpBridge w;
    w.show();

    return a.exec();
}
