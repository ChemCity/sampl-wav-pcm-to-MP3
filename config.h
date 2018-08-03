#ifndef __CONFIG_H__
#define __CONFIG_H__
#include <Windows.h>

// ����WMA�ļ�ͷ��ʶ  
typedef struct _WMA_HEADFLAG_INFO_
{
	unsigned char WmaFlag[16];          // �̶���16���ֽ����ݱ�ʾ�Ƿ�WMA�ļ�  
}WMAHEADFLAG;

// ����WMA�ļ�ͷ������ʶ  
const WMAHEADFLAG WMA_HEAD_FLAGCONST = { 0x30, 0x26, 0xB2, 0x75, 0x8E, 0x66, 0xCF, 0x11, 0xA6, 0xD9, 0x00, 0xAA, 0x00, 0x62, 0xCE, 0x6C };

// ����WMA�ļ�ͷ��С  
typedef struct _WMA_HEADSIZE_INFO_
{
	unsigned char Size[8];     // 8���ֽڱ�ʾ��С���ɺ���ǰ�洢���ݣ�  
}WMAHEADSIZE;

// ����WMA�ļ�ͷ�ṹ  
typedef struct _WMA_HEADINFO_
{
	WMAHEADFLAG HeadFlag;      // WMA�ļ���ʶ  
	WMAHEADSIZE HeadSize;      // WMA�ļ�ͷ��С  
	unsigned char Unknown[6];  // 6���ֽھ������ʲô��������е�˵��֡��������  
}WMAHEADINFO;


typedef struct HeadFileProperties
{
	unsigned char		ObjectID[16];
	long long			ObjectSize;
	unsigned char		FileID[16];
	long long			FileSize;
	long long			CreatetionDate;
	long long			DataPacketsCount;
	long long			PlayDuration;		//����ʱ��
	long long			SendDuration;		//
	long long			Preroll;			//ǰ��
	int					Flags;
	int					Reserved_Seekable_BroadcastFlag; //��30λ ��Reserved ���λ��BroadcastFlag
	int					MinimumDataPacketSize;
	int					MaximumDataPacketSize;
	int					MaximumBitrate;
}HEADFILEPRO;


struct WAVHEAD
{
	char RIFFNAME[4];
	unsigned long nRIFFLength;
	char WAVNAME[4];
	char FMTNAME[4];
	unsigned long nFMTLength;
	unsigned short nAudioFormat;
	unsigned short nChannleNumber;
	//unsigned short nSampleRate; 
	unsigned long nSampleRate;
	unsigned long nBytesPerSecond;
	unsigned short nBytesPerSample;
	unsigned short nBitsPerSample;
	char DATANAME[4];
	unsigned long nDataLength;
};

#endif
