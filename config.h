#ifndef __CONFIG_H__
#define __CONFIG_H__
#include <Windows.h>

// 定义WMA文件头标识  
typedef struct _WMA_HEADFLAG_INFO_
{
	unsigned char WmaFlag[16];          // 固定的16个字节内容表示是否WMA文件  
}WMAHEADFLAG;

// 定义WMA文件头常量标识  
const WMAHEADFLAG WMA_HEAD_FLAGCONST = { 0x30, 0x26, 0xB2, 0x75, 0x8E, 0x66, 0xCF, 0x11, 0xA6, 0xD9, 0x00, 0xAA, 0x00, 0x62, 0xCE, 0x6C };

// 定义WMA文件头大小  
typedef struct _WMA_HEADSIZE_INFO_
{
	unsigned char Size[8];     // 8个字节表示大小（由后向前存储数据）  
}WMAHEADSIZE;

// 定义WMA文件头结构  
typedef struct _WMA_HEADINFO_
{
	WMAHEADFLAG HeadFlag;      // WMA文件标识  
	WMAHEADSIZE HeadSize;      // WMA文件头大小  
	unsigned char Unknown[6];  // 6个字节具体代表什么不清楚（有的说是帧的数量）  
}WMAHEADINFO;


typedef struct HeadFileProperties
{
	unsigned char		ObjectID[16];
	long long			ObjectSize;
	unsigned char		FileID[16];
	long long			FileSize;
	long long			CreatetionDate;
	long long			DataPacketsCount;
	long long			PlayDuration;		//持续时间
	long long			SendDuration;		//
	long long			Preroll;			//前滚
	int					Flags;
	int					Reserved_Seekable_BroadcastFlag; //高30位 是Reserved 最低位是BroadcastFlag
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
