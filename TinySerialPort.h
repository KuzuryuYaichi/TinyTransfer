#ifndef _TINY_SERIAL_PORT_
#define _TINY_SERIAL_PORT_

#include <QtSerialPort>

class TinySerialPort: public QSerialPort
{
public:
	explicit TinySerialPort(const QString& name, QObject* parent = nullptr);
};

#endif
