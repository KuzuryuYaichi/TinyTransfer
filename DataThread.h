#ifndef _DATA_THREAD_H
#define _DATA_THREAD_H

#include <QThread>
#include "TinySerialPort.h"
#include "Struct_NetData.h"
#include "SerialPort/win_qextserialport.h"

class M_UdpSocket;

class DataThread : public QThread
{
    Q_OBJECT
public:
    explicit DataThread(QObject* parent = nullptr);
    void SetUdpSocket(M_UdpSocket* udpSocket);
    M_UdpSocket* udpSocket = nullptr;
};

class DataThreadNB : public DataThread
{
    Q_OBJECT
public:
    explicit DataThreadNB(QObject* parent = nullptr);

    void MakeProtocol(int type, std::shared_ptr<struct Struct_NB>& ptr);
    void MakeProtocol(int type, struct Struct_NB* ptr);

    NB_DDC_DATA Data[64];
    int NB_Pos[64] = { 0 };

protected:
    void run();
};

class DataThreadWB : public DataThread
{
    Q_OBJECT
public:
    explicit DataThreadWB(QObject* parent = nullptr);

    void MakeProtocol(int type, struct Struct_WB* ptr);

    WB_DDC_DATA Data[60];
    WB_DDC_Record Record[60];
    int WB_Pos[60] = { 0 };

protected:
    void run();
};

class DataThreadFFT : public DataThread
{
    Q_OBJECT
public:
    explicit DataThreadFFT(QObject* parent = nullptr);
    void MakeProtocol(int type, std::shared_ptr<struct Struct_FFT>& ptr);
    void MakeProtocol(int type, struct Struct_FFT* ptr);

protected:
    void run();
};

class OrderSerialThread : public DataThread
{
    Q_OBJECT
public:
    explicit OrderSerialThread(QObject* parent = nullptr);
    TinySerialPort* tinySerialPort = nullptr;
    Win_QextSerialPort* winQextSerialPort = nullptr;

protected:
    void run();

private:
    QByteArray SerialWriteRead(const char* data, qint64 len);
    void SerialOrderDeal(std::shared_ptr<struct Struct_Orders>);
};

enum Protocol_Type
{
    PROT_DDC = 0,
    PROT_FFT
};

#endif // _DATA_THREAD_H