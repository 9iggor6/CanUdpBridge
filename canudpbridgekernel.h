#ifndef CANUDPBRIDGEKERNEL_H
#define CANUDPBRIDGEKERNEL_H

#include <QObject>
#include "QCanBus"
#include "QUdpSocket"
#include <QNetworkDatagram>

class CanUdpBridgeKernel : public QObject
{
    Q_OBJECT
public:
    explicit CanUdpBridgeKernel(QString caniface, QHostAddress address,quint16 udpportRx, quint16 udppotrTx);
    ~CanUdpBridgeKernel();


signals:
   void finished();
   void error(QString err);
public slots:
    void startBridge();
    void stopBridge(QString iface);

private:
    QHostAddress m_address;
    QUdpSocket *udpSocket;
    QCanBus *canBus;
    QCanBusDevice *canDevice;
    QString m_caniface;
    quint16 m_udpPortRx, m_udpPortTx;


private slots:
    void receivedCanData();
    void receivedUdpData();
};

#endif // CANUDPBRIDGEKERNEL_H
