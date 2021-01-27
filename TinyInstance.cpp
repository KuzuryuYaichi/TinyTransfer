#include "TinyInstance.h"
#include "dllexport.h"
#include "dllImport.h"
#include "Struct_Data.h"
#include "SerialPort/win_qextserialport.h"

TinyInstance::TinyInstance(QObject *parent): QObject(parent)
{
    tinyConfig = new TinyConfig("config.ini");

    nbSocket = new M_UdpSocket(6543, QHostAddress(tinyConfig->Get_NB_RemoteIP()), tinyConfig->Get_NB_RemotePort(), this);
    fftSocket = new M_UdpSocket(8765, QHostAddress(tinyConfig->Get_FFT_RemoteIP()), tinyConfig->Get_FFT_RemotePort(), this);
    orderSocket = new M_UdpSocket(tinyConfig->Get_Order_LocalPort(), this);

    dataThreadNB = new DataThreadNB(this);
    dataThreadNB->SetUdpSocket(nbSocket);
    dataThreadNB->start();

    dataThreadWB = new DataThreadWB(this);
    dataThreadWB->SetUdpSocket(wbSocket);
    dataThreadWB->start();

    dataThreadFFT = new DataThreadFFT(this);
    dataThreadFFT->SetUdpSocket(fftSocket);
    dataThreadFFT->start();

    orderThreadRecv = new OrderSerialThread(this);
    orderThreadRecv->SetUdpSocket(orderSocket);
    orderThreadRecv->start();

    InitThread();
}

void TinyInstance::InitThread()
{
    RegisterCallBackWBNB(DataWBNB, NULL);
    RegisterCallBackFFT(DataFFT, NULL);
    OpenDevice();
}
