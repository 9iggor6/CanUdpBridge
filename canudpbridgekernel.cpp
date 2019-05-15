#include "canudpbridgekernel.h"
#include "QDebug"

CanUdpBridgeKernel::CanUdpBridgeKernel(QString caniface,
                                       QHostAddress address,
                                       quint16 udpportRx,
                                       quint16 udpportTx):m_caniface(caniface),
    m_address(address),m_udpPortRx(udpportRx),m_udpPortTx(udpportTx)
{
 qDebug()<<"canUdpBridgeKernel construktor: "<<caniface<<"udpPort: "<<udpportRx;

}

CanUdpBridgeKernel::~CanUdpBridgeKernel()
{
    qDebug()<<"canUdpBridgeKernel destruktor";

    delete canDevice;
    delete udpSocket;
}

void CanUdpBridgeKernel::startBridge()
{
    emit error(m_caniface+" -> udp rx:"+QString::number(m_udpPortRx)+" tx:"+QString::number(m_udpPortTx)+ " started working");

    QString errorString;
    canDevice = QCanBus::instance()->createDevice(QStringLiteral("socketcan"),
                                                  m_caniface,
                                                  &errorString);

    if(!canDevice){
        emit error(errorString);
        emit finished();
    } else {
       canDevice->connectDevice();
    }
    udpSocket = new QUdpSocket(this);
    udpSocket->bind(m_udpPortRx, QAbstractSocket::DefaultForPlatform);

    connect(udpSocket, &QUdpSocket::readyRead,
            this, &CanUdpBridgeKernel::receivedUdpData);
    connect(canDevice,&QCanBusDevice::framesReceived,
            this,&CanUdpBridgeKernel::receivedCanData);
}

void CanUdpBridgeKernel::stopBridge(QString iface)
{
    if(iface == m_caniface){
        qDebug()<<"StopBridge";
        emit error(m_caniface+" -> udp"+m_address.toString()+"rx:"+QString::number(m_udpPortRx)+" tx:"+QString::number(m_udpPortTx)+ " stopped working");
        emit finished();
    } else if(iface == "all") {
        qDebug()<<"StopBridge";
        emit error(m_caniface+" -> udp"+m_address.toString()+"rx:"+QString::number(m_udpPortRx)+" tx:"+QString::number(m_udpPortTx)+ " stopped working");
        emit finished();
    }
}

void CanUdpBridgeKernel::receivedCanData()
{
    /* format incoming data to UDP     0      1 2 3    4 5 6 ...   */
    /*                               |S(E)|   | ID |  | Data....|  */
    /* S - standart message */
    /* E - extended message */
   QCanBusFrame frame;
   frame = canDevice->readFrame();
   qDebug()<<"received can frame"<<QString::number(frame.frameId(),16)<<" "<<frame.payload().toHex(' ');
   QByteArray payload;
   payload.clear();
   payload.begin();
   if(frame.hasExtendedFrameFormat()){
       payload.push_back('E');
       payload.push_back(frame.frameId()&0xFF0000);
   } else {
       payload.push_back('S');
       payload.push_back('\0');
   }
   payload.push_back((frame.frameId()>>8)&0xFF);
   payload.push_back(frame.frameId()&0xFF);
   payload.push_back(frame.payload());
   udpSocket->writeDatagram(payload,
                            frame.payload().length(),
                            m_address,
                            m_udpPortTx);

   qDebug()<<"message for tx:"<<payload.toHex(' ');
}

void CanUdpBridgeKernel::receivedUdpData()
{

    /* format incoming data to UDP     0      1 2 3    4 5 6 ...   */
    /*                               |S(E)|   | ID |  | Data....|  */
    /* S - standart message */
    /* E - extended message */
    QNetworkDatagram datagram = udpSocket->receiveDatagram();

    qDebug()<<"Udp receive message: "<<datagram.data();

    quint32 frameId = 0;
    QCanBusFrame frame = QCanBusFrame(frameId, datagram.data().data()+4);
    if(datagram.data().at(0) == 'E'){
        frame.setExtendedFrameFormat(true);
    } else{
        frame.setExtendedFrameFormat(false);
    }
    frameId = (datagram.data().at(1)<<16) | (datagram.data().at(2)<<8) | (datagram.data().at(3));
    frame.setFrameId(frameId);

    qDebug("incoming message:  %c Id: %X data: %s",datagram.data().at(0),frameId,frame.payload().data());
    canDevice->writeFrame(frame);

    qDebug()<<"received Udp Data: "<<datagram.data();
}
