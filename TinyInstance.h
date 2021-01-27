#ifndef _TINY_INSTANCE_H_
#define _TINY_INSTANCE_H_

#include "DataThread.h"
#include "M_UdpSocket.h"
#include "TinySerialPort.h"
#include "TinyConfig.h"

class TinyInstance: public QObject
{
    Q_OBJECT

public:
    TinyInstance(QObject *parent = nullptr);
    
private:
    TinyConfig* tinyConfig;
    M_UdpSocket* nbSocket;
    M_UdpSocket* fftSocket;
    M_UdpSocket* wbSocket;
    M_UdpSocket* orderSocket;
    TinySerialPort* serialPort;
    DataThreadNB* dataThreadNB;
    DataThreadWB* dataThreadWB;
    DataThreadFFT* dataThreadFFT;
    OrderSerialThread* orderThreadRecv;

    void InitThread();
};

#endif
