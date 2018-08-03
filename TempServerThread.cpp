
#include "TempServerThread.h"
#include "log4z.h"
#include "CrcUtil.h"

#define RECV_TIMEOUT 10000
#define RECV_DATA_BUF	128

TempServerThread::TempServerThread(GGRobotFPtr robot)
	:_robot(robot)
{
	m_syscfg = _robot->getSysCfg();
	_terminated = true;
	_waittimeout = 2000;	//ms
	_localserverport = 2404;
	//LOGD("INIT");
}

TempServerThread::~TempServerThread()
{

}

void TempServerThread::Start()
{
	//LOGD("START");
	if (!DoInit())
	{
		LOGI("初始化失败");
	}
	this->start();
}

void TempServerThread::terminate()
{
	//IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_monitor);
	if (!_terminated)
	{
		_terminated = true;
		//_monitor.notify();
	}
}

bool TempServerThread::DoInit()
{
	//创建用于监听的套接字,即服务端的套接字
	m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	SOCKADDR_IN addrSrv;
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(_localserverport); //1024以上的端口号
									/**
									* INADDR_ANY就是指定地址为0.0.0.0的地址，这个地址事实上表示不确定地址，或“所有地址”、“任意地址”。 一般来说，在各个系统中均定义成为0值。
									*/
	addrSrv.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	int retVal = ::bind(m_socket, (LPSOCKADDR)&addrSrv, sizeof(SOCKADDR_IN));
	if (retVal == SOCKET_ERROR) 
	{
		LOGFMTE("连接失败:%d\n", WSAGetLastError());
		return false;
	}

	if (listen(m_socket, 10) == SOCKET_ERROR)
	{
		LOGFMTE("监听失败:%d", WSAGetLastError());
		return false;
	}

	return true;

}

void TempServerThread::DoRun()
{
	////业务处理
	SOCKADDR_IN addrClient;
	int len = sizeof(SOCKADDR);
	printf("监听开始\n");

	while (!_terminated)
	{
		//等待客户请求到来
		SOCKET sConn = accept(m_socket, (SOCKADDR *)&addrClient, &len);
		if (sConn == SOCKET_ERROR)
		{
			LOGFMTE("等待请求失败:%d", WSAGetLastError());
			break;
		}

		LOGFMTT("客户端的IP是:[%s]\n", inet_ntoa((addrClient.sin_addr)));
		responseClient(sConn);
		Sleep(10);

	}

	closesocket(m_socket);
	m_socket = 0;
}

void TempServerThread::run()
{
	//IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_monitor);
	_terminated = false;

	while (!_terminated)
	{

		process();
		Sleep(_waittimeout);
	}

}

void TempServerThread::process()
{

	if (!m_socket)
	{
		LOGI("m_socket error");
		if (!DoInit())
		{
			LOGE("监听启动失败!");
			m_socket = 0;
			Sleep(1000);
			return;
		}

	}
	DoRun();
}

int TempServerThread::responseClient(SOCKET sConn)
{
	char recvBuf[RECV_DATA_BUF];
	//char sendBuf[16];
	int sendSize = 0;
	int res = 0;
	//recv超时设置
	int timeout = RECV_TIMEOUT; //;
	res = setsockopt(sConn, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout));
	if (SOCKET_ERROR == res)
	{
		LOGFMTW("设置recv超时时间失败 WSAGetLastError:%d ", WSAGetLastError());
		return -1;
	}
	while (!_terminated)
	{
		memset(recvBuf, 0, sizeof(recvBuf));
		res = recv(sConn, recvBuf, sizeof(recvBuf), 0);
		if (SOCKET_ERROR == res)
		{
			LOGFMTW("recv ERROR :%d", WSAGetLastError());
			break;
		}
		else if (0 == res)
		{
			printf("接收为空\n");
			break;
		}
		LOGFMTT("%s", CrcUtil::DecToHex(recvBuf, res).c_str());
		if (22 == res)
		{
			//接收到采集器上送的登录命令
			res = SendPromisedData(sConn, recvBuf, res);
			if (-1 == res)
			{
				break;
			}
		}
		else if (39 == res)
		{
			res = SendPromisedData(sConn, recvBuf, res);
			if (-1 == res)
			{
				break;
			}
			//对上送数据进行解析
			short data = (recvBuf[13] - 0x33) & 0xff | (((recvBuf[14] - 0x33) & 0xff) << 8);
			if (1000 == data)
			{
				LOGD("当前有雨雪");
			}
			else
			{
				LOGD("当前没有雨雪");
			}
			LOGFMTT("data=%d  %#4X\n", data, data & 0xffff);
		}
		else
		{
			LOGFMTI("错误的数据包%s", CrcUtil::DecToHex(recvBuf, res).c_str());
		}

	}
	res = shutdown(sConn, 2);
	if (SOCKET_ERROR == res)
	{
		LOGFMTW("%s shutdown失败 WSAGetLastError:%d \n", __FUNCTION__, WSAGetLastError());
	}
	res = closesocket(sConn);
	if (SOCKET_ERROR == res)
	{
		LOGFMTW("%s closesocket失败 WSAGetLastError:%d \n", __FUNCTION__, WSAGetLastError());
	}
	LOGI("断开客户端连接");
	return 0;
}

int TempServerThread::CheckSum(const char * dat, const int len)
{
	if (NULL == dat)
	{
		return -1;
	}
	int sum = 0;
	for (int i = 0; i < len; ++i)
	{
		sum += dat[i] & 0xff;

	}

	return sum;
}

int TempServerThread::SendPromisedData(const SOCKET sConn, const char * recvbuf, const int size)
{
	if (NULL == recvbuf)
	{
		LOGI("recvbuf NULL");
		return -1;
	}

	if (size != 22 && size != 39)
	{
		LOGW("数据包大小错误");
		return -1;
	}

	int sub = size - 2;						//校验码的下标
											//计算校验码 验证接收的数据
	int sum = CheckSum(recvbuf, sub);
	if ((sum & 0xff) != (recvbuf[sub] & 0xff))
	{
		LOGFMTW("sum = %#2X recvBuf[20] = %#2X\n", sum & 0xff, recvbuf[sub]);
		return -1;
	}
	unsigned char sendBuf[] = { 0x68, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x68, 0x20, 0x06, 0x33, 0x1d, 0xb9, 0x8c, 0x33, 0x63, 0x59, 0x16 };
	if (size == 22)
	{
		LOGT("响应RSWIFI-M模块登录命令");
		sendBuf[10] = 0x33;
	}
	else if (size == 39)
	{
		LOGT("响应RSWIFI-M模块上报数据命令");
		sendBuf[10] = 0x36;
	}
	//复制RSWIFI-M模块设备出厂号(终端地址)
	for (int i = 0; i < 4; ++i)
	{
		sendBuf[12 + i] = recvbuf[sub - 4 + i];
	}

	//计算校验码
	sum = CheckSum((char *)sendBuf, 16);
	sendBuf[16] = sum & 0xff;
	//发送响应
	int res = send(sConn, (char *)sendBuf, sizeof(sendBuf), 0);
	if (SOCKET_ERROR == res)
	{
		printf("send ERROR :%d\n", WSAGetLastError());
		return -1;
	}
	return 0;
}

