#include "canudpbridge.h"
#include "ui_canudpbridge.h"
#include "QDebug"
#include "QCanBus"
#include "QUdpSocket"

CanUdpBridge::CanUdpBridge(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::canUdpBridge)
{
    ui->setupUi(this);

    ui->cmbIfaces->clear();
    m_interfaces = QCanBus::instance()->availableDevices(QStringLiteral("socketcan"));
    for (const QCanBusDeviceInfo &info : qAsConst(m_interfaces)){
        ui->cmbIfaces->addItem(info.name());
    }

}

CanUdpBridge::~CanUdpBridge()
{
    delete ui;
}

void CanUdpBridge::errorString(QString err)
{
    ui->console->appendPlainText(err);
}


void CanUdpBridge::on_btnConnect_clicked()
{

    threadForBridge = new QThread(this);
    bridge = new CanUdpBridgeKernel(ui->cmbIfaces->currentText(),
                                    QHostAddress(ui->lineEditAddress->text()),
                                    ui->lineEditUdpPortRx->text().toUInt(nullptr,10),
                                    ui->lineEditUdpPortTx->text().toUInt(nullptr,10));
    bridge->moveToThread(threadForBridge);
    connect(bridge, &CanUdpBridgeKernel::error,
            this, &CanUdpBridge::errorString);

    connect(threadForBridge, &QThread::started,
            bridge, &CanUdpBridgeKernel::startBridge);

    connect(this, &CanUdpBridge::stopBridge,
            bridge, &CanUdpBridgeKernel::stopBridge);

    connect(bridge, &CanUdpBridgeKernel::finished,
            threadForBridge, &QThread::quit);


    connect(bridge, &CanUdpBridgeKernel::finished,
            bridge, &CanUdpBridgeKernel::deleteLater);

    connect(threadForBridge, &QThread::finished,
            threadForBridge, &QThread::deleteLater);

    threadForBridge->start();
}

void CanUdpBridge::on_btnDisconnect_clicked()
{
    emit stopBridge(ui->cmbIfaces->currentText());
}

void CanUdpBridge::on_btnDisconnectAll_clicked()
{
    emit stopBridge("all");
}
