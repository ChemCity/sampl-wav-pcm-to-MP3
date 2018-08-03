#ifndef __TEMPSERVERTHREADF_H__
#define __TEMPSERVERTHREADF_H__

#include <IceUtil/IceUtil.h>

using namespace IceUtil;

class TempServerThreadF : public IceUtil::Shared
{
public:
	virtual void Start() = 0;
	virtual void terminate() = 0;
public:
	//��ʼ��
	virtual bool DoInit() = 0;

	//�����߼�ִ��
	virtual void DoRun() = 0;
};

typedef IceUtil::Handle<TempServerThreadF> TempServerThreadFPtr;






#endif //
