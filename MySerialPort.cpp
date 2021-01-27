#include "MySerialPort.h"
#include "ThreadSafeQueue.h"

MySerialPort::MySerialPort()
{
	//serialPort = new QSerialPort();
}

MySerialPort::~MySerialPort()
{
	//if (serialPort != nullptr && serialPort->isOpen())
	//{
	//	serialPort->clear();
	//	serialPort->close();
	//	delete serialPort;
	//}
}

void MySerialPort::OpenSerialPort()
{
	//QStringList m_serialPortName;

	//for(auto& info : QSerialPortInfo::availablePorts())
	//{
	//	m_serialPortName << info.portName();
	//}
	//serialPort->setPortName(m_serialPortName[0]);//设置串口名字 假设我们上面已经成功获取到了 并且使用第一个

	//if (!serialPort->open(QIODevice::ReadWrite))//用ReadWrite 的模式尝试打开串口
	//	return;

	//serialPort->setBaudRate(QSerialPort::Baud115200);	//设置波特率和读写方向
	//serialPort->setDataBits(QSerialPort::Data8);		//数据位为8位
	//serialPort->setFlowControl(QSerialPort::NoFlowControl);//无流控制
	//serialPort->setParity(QSerialPort::NoParity);		//无校验位
	//serialPort->setStopBits(QSerialPort::OneStop);	//一位停止位
}

void MySerialPort::CloseSerialPort()
{
	//if (serialPort->isOpen())//如果串口已经打开了 先给他关闭了
	//{
	//	serialPort->clear();
	//	serialPort->close();
	//}
}
