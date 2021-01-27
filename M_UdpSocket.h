#ifndef M_UDPSOCKET
#define M_UPDSOCKET

#include <QtNetwork>

class M_UdpSocket: QUdpSocket
{
	Q_OBJECT
private:
	QHostAddress localAddr;
	int localPort;
	QHostAddress remoteAddr;
	int remotePort;

public:
	M_UdpSocket(QObject* parent = nullptr);
	M_UdpSocket(int localPort, QObject* parent = nullptr);
	M_UdpSocket(int localPort, QHostAddress&& remoteAddress, int remotePort, QObject* parent = nullptr);
	M_UdpSocket(QHostAddress&& localAddr, int localPort, QHostAddress&& remoteAddress, int remotePort, QObject* parent = nullptr);
	void udpDataSend(const char* data, int len);
	void udpDataSend(const char* data, int len, QHostAddress& remoteAddr, int remotePort);

private Q_SLOTS:
	void udpDataReceived();
};

#endif
