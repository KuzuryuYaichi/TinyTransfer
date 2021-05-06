#ifndef _TINY_CONFIG_H
#define _TINY_CONFIG_H

#include <QSettings>
#include <QFileInfo>
#include <QCoreApplication>

class TinyConfig
{
public:
	TinyConfig(QString);
	void Set_NB(QString&, int&);
	void Get_NB(QString&, int&);
	void Set_WB(QString&, int&);
	void Get_WB(QString&, int&);
	void Set_FFT(QString&, int&);
	void Get_FFT(QString&, int&);
	void Set_Order(QString&, int&);
	void Get_Order(QString&, int&);

	QString Get_NB_RemoteIP();
	QString Get_WB_RemoteIP();
	QString Get_FFT_RemoteIP();
	int Get_NB_RemotePort();
	int Get_WB_RemotePort();
	int Get_FFT_RemotePort();
	int Get_Order_LocalPort();

private:
	const QString DEFAULT_REMOTE_NB_IP = "192.168.1.11", DEFAULT_REMOTE_WB_IP = "192.168.1.11", DEFAULT_REMOTE_FFT_IP = "192.168.1.11";
	const int DEFAULT_REMOTE_NB_PORT = 6543, DEFAULT_REMOTE_WB_PORT = 7654, DEFAULT_REMOTE_FFT_PORT = 8765, DEFAULT_LOCAL_ORDER_PORT = 9876;
	const QString NB_Area = "NarrowBand", WB_Area = "WideBand", FFT_Area = "FFT", Order_Area = "Order";

	QSettings* m_psetting = nullptr;
	QString RemoteIP_NB, RemoteIP_WB, RemoteIP_FFT, RemoteIP_Order;
	int RemotePort_NB, RemotePort_WB, RemotePort_FFT, RemotePort_Order;

	void SetConfigData(QString, QString, QVariant);
	QVariant GetConfigData(QString, QString);
	bool isIpAddr(const QString&);
};

#endif
