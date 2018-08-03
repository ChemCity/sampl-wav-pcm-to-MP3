#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif // !_CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "Wma.h"

Wma::Wma()
{
}

Wma::~Wma()
{
}

int Wma::WriteWmaFile(const char * buf, const int size)
{
	FILE *fp = fopen("wave.wma", "wb");
	if (NULL == fp)
	{
		printf("文件打开错误");
		return -1;
	}
	fwrite(WMA_HEAD_FLAGCONST.WmaFlag, 1, sizeof(WMA_HEAD_FLAGCONST.WmaFlag), fp);
	char tol[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x01, 0x02 };//文件大小 8个字节 + 位置6字节
	fwrite(tol, 1, sizeof(tol), fp);
	return 0;
}
