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
	//serialPort->setPortName(m_serialPortName[0]);//���ô������� �������������Ѿ��ɹ���ȡ���� ����ʹ�õ�һ��

	//if (!serialPort->open(QIODevice::ReadWrite))//��ReadWrite ��ģʽ���Դ򿪴���
	//	return;

	//serialPort->setBaudRate(QSerialPort::Baud115200);	//���ò����ʺͶ�д����
	//serialPort->setDataBits(QSerialPort::Data8);		//����λΪ8λ
	//serialPort->setFlowControl(QSerialPort::NoFlowControl);//��������
	//serialPort->setParity(QSerialPort::NoParity);		//��У��λ
	//serialPort->setStopBits(QSerialPort::OneStop);	//һλֹͣλ
}

void MySerialPort::CloseSerialPort()
{
	//if (serialPort->isOpen())//��������Ѿ����� �ȸ����ر���
	//{
	//	serialPort->clear();
	//	serialPort->close();
	//}
}
