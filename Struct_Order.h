#ifndef _STRUCT_ORDER_
#define _STRUCT_ORDER_

#include <QHostAddress>
#include <memory>

extern struct NB_Params wb_params;

//单板机与V7间指令协议
//通过115200Hz
//每条下发指令固定长度7Byte，每条反馈指令长度固定5Byte。
enum SERIAL_ORDER_TYPE
{
	FPGA_TEMPRATURE = 0,//1.FPGA温度自检
	RF_DESC_CONTROL,//2.射频衰减控制
	RF_WORK_MODE,//3.射频工作模式控制
	RF__SET,//4.射频是否馈电天线设置
	RF_STATUS,//5.射频状态查询
	RF_AGC_MGC_SWITCH,//6.射频MGC/AGC状态切换
	RF_AGC_PARAMS,//7.射频AGC参数控制
	DIGITAL_AGC_MGC_SWITCH,//8.数字MGC/AGC状态切换
	DIGITAL_AGC_PARAMS,//9.数字AGC参数控制
	DIGITAL_MGC_PARAMS,//10.数字MGC参数控制
	RF_GAIN_MODE,//11.射频增益模式查询
	DIGITAL_GAIN_MODE,//12.数字增益模式查询
	NB_PARAMS,//13.窄带参数配置
	FFT_NUMS,//14.FFT点数选择配置
	DIGITAL_24dB_GAIN,//15.固定数字增益24dcBm开启与关闭
	GJ_VERSION,//16.	固件版本号查询
	CLOCK_STATUS,//17.	系统时钟状态查询
	PCIE_STATUS,//18.	PCIE连接状态查询
	CW_VALUE,//19.	CW拍频值下发
	FFT_WINDOW,//20.	FFT运算窗类型下发
	NB_RECV_MODE,//21.	窄带接收模式下发
	NB_AGC_SWITCH,//22.	窄带AGC模式控制
	SMOOTH_TIMES,//23.	平滑次数控制
	ISRECORD, //24. 落盘开关
	DIGITAL_NB_MGC_GAIN
	//26.	上位机与单板机之间的指令（不需要透传给V7），待用户给出协议
};

struct NB_Params
{
	int freq = 5801000;
	int band = 50000;
	int mode = 0; //0 IQ 1 AM 2 ISB 3 CW 4 USB 5 LSB
	bool isMgcAgc = 0; //0 MGC 1 AGC
	int mgcVal = 0;
	int agcVal = 0;

	unsigned int CaclSample()
	{
		unsigned int sample = 6;
		switch (band)
		{
		case 3200:
		case 6400:
			sample = 9600;
			break;
		case 10000:
		case 15000:
			sample = 19200;
			break;
		case 20000:
		case 25000:
			sample = 38400;
			break;
		case 50000:
			sample = 76800;
			break;
		}
		return sample;
	}

	unsigned int CaclFreq()
	{
		return CaclFreq(freq);
	}

	unsigned int CaclFreq(int freq)
	{
		this->freq = freq;
		double d_freq = freq;
		d_freq *= pow(2, 28) / 1000000 / 96;
		return (unsigned int)d_freq;
	}

	char findModeType()
	{
		return findModeType(mode);
	}

	char findModeType(int mode)
	{
		this->mode = mode;
		char type = 6;
		switch (mode)
		{
		case 0:
			type = 0;
			break;
		case 1:
			type = 1;
			break;
		case 2:
		case 4:
		case 5:
			type = 2;
			break;
		case 3:
			type = 3;
			break;
		}
		return type;
	}

	char findBandType()
	{
		return findBandType(band);
	}

	char findBandType(int band)
	{
		this->band = band;
		char type = 6;
		switch (band)
		{
		case 3200:
			type = 0;
			break;
		case 6400:
			type = 1;
			break;
		case 10000:
			type = 2;
			break;
		case 15000:
			type = 3;
			break;
		case 20000:
			type = 4;
			break;
		case 25000:
			type = 5;
			break;
		case 50000:
			type = 6;
			break;
		}
		return type;
	}
};

//指令回传数据解析和封包
struct S_Control_Instruction
{
	static const int CONTROL_HEAD = 0x44444444;
	static const int CONTROL_TAIL = 0x55555555;

	unsigned int  iPackHead;
	unsigned int  iDeviceType;
	unsigned int  iChannelNo;
	unsigned int  iInstruction = 0;
	unsigned int  iControlContext;
	unsigned char cReserved[8] = { 0 };
	unsigned int  iPackEnd;

	S_Control_Instruction(int instruction_Net, int iChannelNo)
	{
		iPackHead = CONTROL_HEAD;
		iDeviceType = 1;
		iPackEnd = CONTROL_TAIL;
		this->iChannelNo = iChannelNo;
		iInstruction = 0x1000FFFF;
		iControlContext = 1;
		*(int*)cReserved = instruction_Net;
	}

	void CopyRet(const char* order)
	{
		memcpy(cReserved + 4, order + 1, 4);
	}
};

enum NET_CONTROL_ORDER
{
	NET_CMD_CALLBACK = 0x1000FFFF,//指令回复
	NET_SELF_CHECK = 0x10000002,//FPGA_TEMPRATURE RF_AGC_MGC_SWITCH RF_DESC_CONTROL RF_STATUS PCIE_STATUS CLOCK_STATUS
	NET_FREQ = 0x10000101,//NB_PARAMS
	NET_BANDWIDTH = 0x10000102,//NB_PARAMS
	NET_RF_GAIN = 0x10000103,//RF_AGC_MGC_SWITCH RF_DESC_CONTROL
	NET_RF_GAIN_MODE = 0x10000104,
	NET_RBW = 0x10000202,//FFT_NUMS
	NET_RECV_MODE = 0x10000301,//NB_RECV_MODE
	NET_DEMOMODEM = 0x10000302,//NB_PARAMS
	NET_CW_BFO = 0x10000303,//CW_VALUE
	NET_DIGITAL_24dB_GAIN = 0x10000ccc,//24dB固定数字增益
	NET_RF_STATUS = 0x10000eee,//RF_STATUS
	NET_GAIN_MODE = 0x10000fff, //RF_GAIN_MODE
	NET_SMOOTH_TIMES = 0x10000ddd,
	NET_ISRECORD = 0x10000106
};

extern struct NB_Params nb_params[64];
//指令接收数据解析和封包
struct Struct_Order
{
	char order[7] = {0};

	Struct_Order(int instruction_Net, int instruction_Serial, int channel, const char* params)
	{
		order[0] = instruction_Serial;
		switch (instruction_Net)
		{
		case NET_SELF_CHECK:
			break;
		case NET_FREQ:
		{
			if (channel >= 0 && channel < 64)
			{
				order[1] = channel;//通道号
				int freq = nb_params[channel].CaclFreq(*(int*)params);
				order[2] = (nb_params[channel].findModeType() << 4) & 0xF0 | (freq >> 24 & 0xF);//前4bit是解调类型
				order[3] = (freq >> 16) & 0xFF;//28bit频点
				order[4] = (freq >> 8) & 0xFF;
				order[5] = freq & 0xFF;
				order[6] = nb_params[channel].findBandType();//带宽
			}
			break;
		}
		case NET_BANDWIDTH:
		{
			if (channel >= 0 && channel < 64)
			{
				order[1] = channel;//通道号
				int bandwidth = *(int*)params;
				int freq = nb_params[channel].CaclFreq();
				order[2] = (nb_params[channel].findModeType() << 4) & 0xF0 | (freq >> 24 & 0xF);//前4bit是解调类型
				order[3] = (freq >> 16) & 0xFF;//28bit频点
				order[4] = (freq >> 8) & 0xFF;
				order[5] = freq & 0xFF;
				order[6] = nb_params[channel].findBandType(bandwidth);//带宽
			}
			break;
		}
		case NET_DEMOMODEM:
		{
			if (channel >= 0 && channel < 64)
			{
				order[1] = channel;//通道号
				int mode = *(int*)params;
				int freq = nb_params[channel].CaclFreq();
				order[2] = (nb_params[channel].findModeType(mode) << 4) & 0xF0 | (freq >> 24) & 0xF;//前4bit是解调类型
				order[3] = (freq >> 16) & 0xFF;//28bit频点
				order[4] = (freq >> 8) & 0xFF;
				order[5] = freq & 0xFF;
				order[6] = nb_params[channel].findBandType();//带宽
			}
			break;
		}
		case NET_CW_BFO:
		{
			if (channel >= 0 && channel < 64)
			{
				order[1] = channel;
				double CW_Value_tmp = *(unsigned int*)params;
				quint32 CW_Value = CW_Value_tmp * pow(2, 25) / 12000000;
				order[2] = (CW_Value >> 24) & 0xFF;
				order[3] = (CW_Value >> 16) & 0xFF;
				order[4] = (CW_Value >> 8) & 0xFF;
				order[5] = (CW_Value) & 0xFF;
			}
			break;
		}
		case NET_RF_GAIN:
		{
			if (instruction_Serial == RF_AGC_MGC_SWITCH)
			{
				order[1] = *(int*)params;
			}
			else if (instruction_Serial == RF_DESC_CONTROL)
			{
				int val = *(int*)(params + 4);
				if(val > 31)
					order[1] = 0;
				else if (val >= 0)
					order[1] = 31 - val;
			}
			else if (instruction_Serial == DIGITAL_MGC_PARAMS)
			{
				int val = *(int*)(params + 4);
				if (val > 31)
					order[1] = val - 31;
				else if (val >= 0)
					order[1] = 0;
			}
			else if (instruction_Serial == DIGITAL_NB_MGC_GAIN)
			{
				int val = *(int*)(params + 4);
				order[1] = channel;
				order[2] = val;
			}
			break;
		}
		case NET_RBW:
		{
			order[1] = *(int*)params;
			break;
		}
		case NET_RECV_MODE:
		{
			break;
		}
		case NET_DIGITAL_24dB_GAIN:
		{
			order[1] = *(int*)params;
			break;
		}
		case NET_RF_STATUS:
		{
			break;
		}
		case NET_GAIN_MODE:
		{
			break;
		}
		case NET_SMOOTH_TIMES:
		{
			order[1] = *(int*)params;
			break;
		}
		case NET_ISRECORD:
		{
			order[1] = *(int*)params;
			break;
		}
		}
	}
};

struct Struct_Orders
{
	struct Struct_Order** p = nullptr;
	int pos = 0, instruction_Net = 0, channel = 0;
	char* params = nullptr;
	QHostAddress remoteAddr;
	int remotePort;

	void push(int instruction_Serial)
	{
		p[pos++] = new Struct_Order(instruction_Net, instruction_Serial, channel, params);
	}

	Struct_Orders(char* data)
	{
		channel = *(int*)(data + 8);
		qDebug() << " ChannelNo: " << channel;
		instruction_Net = *(int*)(data + 12);
		params = data + 16;
		switch (instruction_Net)
		{
		case NET_SELF_CHECK:
		{
			p = new Struct_Order * [8];
			push(FPGA_TEMPRATURE);
			push(RF_AGC_MGC_SWITCH);
			push(RF_STATUS);
			push(RF_DESC_CONTROL);
			push(RF_STATUS);
			push(RF_DESC_CONTROL);
			push(PCIE_STATUS);
			push(CLOCK_STATUS);
			break;
		}
		case NET_FREQ:
		{
			p = new Struct_Order * [1];
			push(NB_PARAMS);
			break;
		}
		case NET_BANDWIDTH:
		{
			p = new Struct_Order * [1];
			push(NB_PARAMS);
			break;
		}
		case NET_RF_GAIN:
		{
			if (channel >= 0 && channel < 64) //当channel在0到63之间时控制窄带
			{
				nb_params[channel].isMgcAgc = *(int*)params;
				if (nb_params[channel].isMgcAgc == 0) //窄带MGC
				{
					p = new Struct_Order * [2];
					push(DIGITAL_NB_MGC_GAIN);
				}
				else //窄带AGC
				{

				}
			}
			else if (channel == 64) //否则channel为64时控制宽带
			{
				wb_params.isMgcAgc = *(int*)params;
				if (wb_params.isMgcAgc == 0) //宽带MGC
				{
					p = new Struct_Order * [3];
					push(RF_AGC_MGC_SWITCH);
					push(RF_DESC_CONTROL);
					push(DIGITAL_MGC_PARAMS);
				}
				else //宽带AGC
				{
					p = new Struct_Order * [1];
					push(RF_AGC_MGC_SWITCH);
				}
			}
			break;
		}
		case NET_RF_GAIN_MODE:
		{
			break;
		}
		case NET_RBW:
		{
			p = new Struct_Order * [1];
			push(FFT_NUMS);
			break;
		}
		case NET_RECV_MODE:
		{
			p = new Struct_Order * [1];
			push(NB_RECV_MODE);
			break;
		}
		case NET_DEMOMODEM:
		{
			p = new Struct_Order * [1];
			push(NB_PARAMS);
			break;
		}
		case NET_CW_BFO:
		{
			p = new Struct_Order * [1];
			push(CW_VALUE);
			break;
		}
		case NET_DIGITAL_24dB_GAIN:
		{
			p = new Struct_Order * [1];
			push(DIGITAL_24dB_GAIN);
			break;
		}
		case NET_RF_STATUS:
		{
			p = new Struct_Order * [1];
			push(RF_STATUS);
			break;
		}
		case NET_GAIN_MODE:
		{
			p = new Struct_Order * [1];
			push(RF_GAIN_MODE);
			break;
		}
		case NET_SMOOTH_TIMES:
		{
			p = new Struct_Order * [1];
			push(SMOOTH_TIMES);
			break;
		}
		case NET_ISRECORD:
		{
			p = new Struct_Order * [1];
			push(ISRECORD);
			break;
		}
		}
	}

	Struct_Orders(char* data, QHostAddress& remoteAddr, int remotePort): Struct_Orders(data)
	{
		this->remoteAddr = remoteAddr;
		this->remotePort = remotePort;
	}

	~Struct_Orders()
	{
		for (int i = 0; i < pos; ++i)
		{
			if (p[i] != nullptr)
				delete p[i];
		}
		delete[] p;
	}
};

#endif
