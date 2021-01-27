#include "TinyConfig.h"

QString TinyConfig::Get_NB_RemoteIP()
{
	return GetConfigData(NB_Area, "RemoteIP").toString();
}

QString TinyConfig::Get_WB_RemoteIP()
{
	return GetConfigData(WB_Area, "RemoteIP").toString();
}

QString TinyConfig::Get_FFT_RemoteIP()
{
	return GetConfigData(FFT_Area, "RemoteIP").toString();
}

int TinyConfig::Get_NB_RemotePort()
{
	return GetConfigData(NB_Area, "RemotePort").toInt();
}

int TinyConfig::Get_WB_RemotePort()
{
	return GetConfigData(WB_Area, "RemotePort").toInt();
}

int TinyConfig::Get_FFT_RemotePort()
{
	return GetConfigData(FFT_Area, "RemotePort").toInt();
}

int TinyConfig::Get_Order_LocalPort()
{
	return GetConfigData(Order_Area, "LocalPort").toInt();
}

void TinyConfig::Set_NB(QString& RemoteIP, int& RemotePort)
{
	SetConfigData(NB_Area, "RemoteIP", RemoteIP);
	SetConfigData(NB_Area, "RemotePort", RemotePort);
}
void TinyConfig::Get_NB(QString& RemoteIP, int& RemotePort)
{
	RemoteIP = GetConfigData(NB_Area, "RemoteIP").toString();
	RemotePort = GetConfigData(NB_Area, "RemotePort").toInt();
}
void TinyConfig::Set_WB(QString& RemoteIP, int& RemotePort)
{
	SetConfigData(WB_Area, "RemoteIP", RemoteIP);
	SetConfigData(WB_Area, "RemotePort", RemotePort);
}
void TinyConfig::Get_WB(QString& RemoteIP, int& RemotePort)
{
	RemoteIP = GetConfigData(WB_Area, "RemoteIP").toString();
	RemotePort = GetConfigData(WB_Area, "RemotePort").toInt();
}
void TinyConfig::Set_FFT(QString& RemoteIP, int& RemotePort)
{
	SetConfigData(FFT_Area, "RemoteIP", RemoteIP);
	SetConfigData(FFT_Area, "RemotePort", RemotePort);
}
void TinyConfig::Get_FFT(QString& RemoteIP, int& RemotePort)
{
	RemoteIP = GetConfigData(FFT_Area, "RemoteIP").toString();
	RemotePort = GetConfigData(FFT_Area, "RemotePort").toInt();
}
void TinyConfig::Set_Order(QString& RemoteIP, int& RemotePort)
{
	SetConfigData(Order_Area, "RemoteIP", RemoteIP);
	SetConfigData(Order_Area, "RemotePort", RemotePort);
}
void TinyConfig::Get_Order(QString& RemoteIP, int& RemotePort)
{
	RemoteIP = GetConfigData(Order_Area, "RemoteIP").toString();
	RemotePort = GetConfigData(Order_Area, "RemotePort").toInt();
}

TinyConfig::TinyConfig(QString fileName)
{
	QString configFileName = QCoreApplication::applicationDirPath() + "/" + fileName;

	//根据Config.ini路径new QSetting对象
	m_psetting = new QSettings(configFileName, QSettings::IniFormat);

	QFileInfo fileInfo(configFileName);
	if (!fileInfo.exists())
	{
		SetConfigData(NB_Area, "RemoteIP", DEFAULT_REMOTE_NB_IP);
		SetConfigData(NB_Area, "RemotePort", DEFAULT_REMOTE_NB_PORT);
		SetConfigData(WB_Area, "RemoteIP", DEFAULT_REMOTE_WB_IP);
		SetConfigData(WB_Area, "RemotePort", DEFAULT_REMOTE_WB_PORT);
		SetConfigData(FFT_Area, "RemoteIP", DEFAULT_REMOTE_FFT_IP);
		SetConfigData(FFT_Area, "RemotePort", DEFAULT_REMOTE_FFT_PORT);
		SetConfigData(Order_Area, "LocalPort", DEFAULT_LOCAL_ORDER_PORT);
	}

	//读取Config.ini的数据
	QString RemoteIP_NB = GetConfigData(NB_Area, "RemoteIP").toString();
	int RemotePort_NB = GetConfigData(NB_Area, "RemotePort").toInt();

	//根据读取的数据判断配置项有效性，无效则设置为默认值
	if (isIpAddr(RemoteIP_NB) && RemotePort_NB > 0 && RemotePort_NB < 65536) {
		this->RemoteIP_NB = RemoteIP_NB;
		this->RemotePort_NB = RemotePort_NB;
	}

	//读取Config.ini的数据
	QString RemoteIP_WB = GetConfigData(WB_Area, "RemoteIP").toString();
	int RemotePort_WB = GetConfigData(WB_Area, "RemotePort").toInt();

	//根据读取的数据判断配置项有效性，无效则设置为默认值
	if (isIpAddr(RemoteIP_WB) && RemotePort_WB > 0 && RemotePort_WB < 65536) {
		this->RemoteIP_NB = RemoteIP_WB;
		this->RemotePort_NB = RemotePort_WB;
	}

	//读取Config.ini的数据
	QString RemoteIP_FFT = GetConfigData(NB_Area, "RemoteIP").toString();
	int RemotePort_FFT = GetConfigData(NB_Area, "RemotePort").toInt();

	//根据读取的数据判断配置项有效性，无效则设置为默认值
	if (isIpAddr(RemoteIP_FFT) && RemotePort_FFT > 0 && RemotePort_FFT < 65536) {
		this->RemoteIP_NB = RemoteIP_FFT;
		this->RemotePort_NB = RemotePort_FFT;
	}
}

// 写配置文件接口
void TinyConfig::SetConfigData(QString qstrnodename, QString qstrkeyname, QVariant qvarvalue)
{
	if (m_psetting) {
		m_psetting->setValue(QString("/%1/%2").arg(qstrnodename).arg(qstrkeyname), qvarvalue);
	}
}

// 读配置文件接口
QVariant TinyConfig::GetConfigData(QString qstrnodename, QString qstrkeyname)
{
	QVariant qvar = -1;
	if (m_psetting) {
		qvar = m_psetting->value(QString("/%1/%2").arg(qstrnodename).arg(qstrkeyname));
	}
	return qvar;
}

// 验证IP合法性
bool TinyConfig::isIpAddr(const QString& ip)
{
	QRegExp rx2("((2[0-4]\\d|25[0-5]|[01]?\\d\\d?)\\.){3}(2[0-4]\\d|25[0-5]|[01]?\\d\\d?)");
	if (rx2.exactMatch(ip))
	{
		return true;
	}
	return false;
}
