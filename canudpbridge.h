#ifndef CANUDPBRIDGE_H
#define CANUDPBRIDGE_H

#include <QMainWindow>
#include "canudpbridgekernel.h"
#include <QCanBus>
#include <QThread>

namespace Ui {
class canUdpBridge;
}

class CanUdpBridge : public QMainWindow
{
    Q_OBJECT

public:
    explicit CanUdpBridge(QWidget *parent = nullptr);
    ~CanUdpBridge();
signals:
    void stopBridge(QString iface);

private slots:
    void errorString(QString err);
    void on_btnConnect_clicked();

    void on_btnDisconnect_clicked();

    void on_btnDisconnectAll_clicked();

private:
    Ui::canUdpBridge *ui;
    QList<QCanBusDeviceInfo> m_interfaces;
    QThread *threadForBridge;
    CanUdpBridgeKernel *bridge;
};

#endif // CANUDPBRIDGE_H
