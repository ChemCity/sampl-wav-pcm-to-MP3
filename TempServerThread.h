#ifndef __TEMPSERVER_H__
#define __TEMPSERVER_H__

/*
	���մ���RSWIFI-M�ϱ�������
*/

#include "TempServerThreadF.h"
#include "GGRobotF.h"
#include "Config.h"



class TempServerThread :public TempServerThreadF, virtual public ::IceUtil::Thread
{
public:
	TempServerThread(GGRobotFPtr robot);
	~TempServerThread();


	void Start();
	void terminate();
public:
	//��ʼ��
	bool DoInit();

	//�����߼�ִ��
	void DoRun();

private:
	void run();

	void process();
private:
	int responseClient(SOCKET sConn);
	int CheckSum(const char * dat, const int len);
	int SendPromisedData(const SOCKET sConn, const char* recvbuf, const int size);
private:

	DWORD						_waittimeout;
private:
	bool					_terminated;
	GGRobotFPtr				_robot;
	SystemSettingPtr		m_syscfg;

	int						_localserverport;
	SOCKET					m_socket;
};

typedef IceUtil::Handle<TempServerThread> TempServerThreadPtr;


#endif
