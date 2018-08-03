
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
		LOGI("��ʼ��ʧ��");
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
	//�������ڼ������׽���,������˵��׽���
	m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	SOCKADDR_IN addrSrv;
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(_localserverport); //1024���ϵĶ˿ں�
									/**
									* INADDR_ANY����ָ����ַΪ0.0.0.0�ĵ�ַ�������ַ��ʵ�ϱ�ʾ��ȷ����ַ�������е�ַ�����������ַ���� һ����˵���ڸ���ϵͳ�о������Ϊ0ֵ��
									*/
	addrSrv.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	int retVal = ::bind(m_socket, (LPSOCKADDR)&addrSrv, sizeof(SOCKADDR_IN));
	if (retVal == SOCKET_ERROR) 
	{
		LOGFMTE("����ʧ��:%d\n", WSAGetLastError());
		return false;
	}

	if (listen(m_socket, 10) == SOCKET_ERROR)
	{
		LOGFMTE("����ʧ��:%d", WSAGetLastError());
		return false;
	}

	return true;

}

void TempServerThread::DoRun()
{
	////ҵ����
	SOCKADDR_IN addrClient;
	int len = sizeof(SOCKADDR);
	printf("������ʼ\n");

	while (!_terminated)
	{
		//�ȴ��ͻ�������
		SOCKET sConn = accept(m_socket, (SOCKADDR *)&addrClient, &len);
		if (sConn == SOCKET_ERROR)
		{
			LOGFMTE("�ȴ�����ʧ��:%d", WSAGetLastError());
			break;
		}

		LOGFMTT("�ͻ��˵�IP��:[%s]\n", inet_ntoa((addrClient.sin_addr)));
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
			LOGE("��������ʧ��!");
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
	//recv��ʱ����
	int timeout = RECV_TIMEOUT; //;
	res = setsockopt(sConn, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout));
	if (SOCKET_ERROR == res)
	{
		LOGFMTW("����recv��ʱʱ��ʧ�� WSAGetLastError:%d ", WSAGetLastError());
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
			printf("����Ϊ��\n");
			break;
		}
		LOGFMTT("%s", CrcUtil::DecToHex(recvBuf, res).c_str());
		if (22 == res)
		{
			//���յ��ɼ������͵ĵ�¼����
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
			//���������ݽ��н���
			short data = (recvBuf[13] - 0x33) & 0xff | (((recvBuf[14] - 0x33) & 0xff) << 8);
			if (1000 == data)
			{
				LOGD("��ǰ����ѩ");
			}
			else
			{
				LOGD("��ǰû����ѩ");
			}
			LOGFMTT("data=%d  %#4X\n", data, data & 0xffff);
		}
		else
		{
			LOGFMTI("��������ݰ�%s", CrcUtil::DecToHex(recvBuf, res).c_str());
		}

	}
	res = shutdown(sConn, 2);
	if (SOCKET_ERROR == res)
	{
		LOGFMTW("%s shutdownʧ�� WSAGetLastError:%d \n", __FUNCTION__, WSAGetLastError());
	}
	res = closesocket(sConn);
	if (SOCKET_ERROR == res)
	{
		LOGFMTW("%s closesocketʧ�� WSAGetLastError:%d \n", __FUNCTION__, WSAGetLastError());
	}
	LOGI("�Ͽ��ͻ�������");
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
		LOGW("���ݰ���С����");
		return -1;
	}

	int sub = size - 2;						//У������±�
											//����У���� ��֤���յ�����
	int sum = CheckSum(recvbuf, sub);
	if ((sum & 0xff) != (recvbuf[sub] & 0xff))
	{
		LOGFMTW("sum = %#2X recvBuf[20] = %#2X\n", sum & 0xff, recvbuf[sub]);
		return -1;
	}
	unsigned char sendBuf[] = { 0x68, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x68, 0x20, 0x06, 0x33, 0x1d, 0xb9, 0x8c, 0x33, 0x63, 0x59, 0x16 };
	if (size == 22)
	{
		LOGT("��ӦRSWIFI-Mģ���¼����");
		sendBuf[10] = 0x33;
	}
	else if (size == 39)
	{
		LOGT("��ӦRSWIFI-Mģ���ϱ���������");
		sendBuf[10] = 0x36;
	}
	//����RSWIFI-Mģ���豸������(�ն˵�ַ)
	for (int i = 0; i < 4; ++i)
	{
		sendBuf[12 + i] = recvbuf[sub - 4 + i];
	}

	//����У����
	sum = CheckSum((char *)sendBuf, 16);
	sendBuf[16] = sum & 0xff;
	//������Ӧ
	int res = send(sConn, (char *)sendBuf, sizeof(sendBuf), 0);
	if (SOCKET_ERROR == res)
	{
		printf("send ERROR :%d\n", WSAGetLastError());
		return -1;
	}
	return 0;
}

