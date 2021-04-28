#include "M_UdpSocket.h"
#include "ThreadSafeQueue.h"
#include "Struct_Order.h"

class QHostAddress;

threadsafe_queue<std::shared_ptr<struct Struct_Orders>> tsqueueSerialOrder;

M_UdpSocket::M_UdpSocket(QObject* parent): QUdpSocket(parent)
{

}

M_UdpSocket::M_UdpSocket(int localPort, QObject* parent) : QUdpSocket(parent)
{
    connect(this, SIGNAL(readyRead()), this, SLOT(udpDataReceived()));
    bool result = bind(localPort);
    if (!result)
    {
        return;
    }
}

M_UdpSocket::M_UdpSocket(int localPort, QHostAddress&& remoteAddress, int remotePort, QObject* parent) : QUdpSocket(parent)
{
    connect(this, SIGNAL(readyRead()), this, SLOT(udpDataReceived()));
    this->localPort = localPort;
    this->remoteAddr = remoteAddress;
    this->remotePort = remotePort;
    bool result = bind(localPort);
    if (!result)
    {
        return;
    }
}

M_UdpSocket::M_UdpSocket(QHostAddress&& localAddr, int localPort, QHostAddress&& remoteAddress, int remotePort, QObject* parent): QUdpSocket(parent)
{
    connect(this, SIGNAL(readyRead()), this, SLOT(udpDataReceived()));
    this->localAddr = localAddr;
    this->localPort = localPort;
    this->remoteAddr = remoteAddress;
    this->remotePort = remotePort;
    bool result = bind(localAddr, localPort);
    if (!result)
    {
        return;
    }
}

void M_UdpSocket::udpDataReceived()
{
    QHostAddress remoteAddress;
    quint16 remotePort;
    int rcvDataCnt = 0;

    while (hasPendingDatagrams())
    {
        auto datagram = std::make_shared<QByteArray>();
        datagram->resize(pendingDatagramSize());
        readDatagram(datagram->data(), datagram->size(), &remoteAddress, &remotePort);
        rcvDataCnt += datagram->size();

        const char* data = datagram->data();
        if (data[0] == 0x44 && data[1] == 0x44 && data[2] == 0x44 && data[3] == 0x44)
        {
            auto ptr = std::make_shared<struct Struct_Orders>(datagram->data(), remoteAddress, remotePort);
            static int orderCnt = 1;
            qDebug("[%d] Received Instruction: 0x%x ", orderCnt++, ptr->instruction_Net);
            tsqueueSerialOrder.push(ptr);
        }
    }
}

void M_UdpSocket::udpDataSend(const char* data, int len)
{
    int total = len;
    int ret;
    while (total > 0)
    {
        ret = writeDatagram(data, len, remoteAddr, remotePort);
        if (ret > 0)
            total -= ret;
        //else if (ret < 0)
        //{
        //    qDebug() << "MySocket Error:" << error();
        //    break;
        //}
    }
}

void M_UdpSocket::udpDataSend(const char* data, int len, QHostAddress& remoteAddr, int remotePort)
{
    int total = len;
    int ret;
    while (total > 0)
    {
        ret = writeDatagram(data, len, remoteAddr, remotePort);
        if (ret > 0)
            total -= ret;
    }
}
