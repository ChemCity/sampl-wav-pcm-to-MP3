// CVoice.cpp : 定义控制台应用程序的入口点。
//
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif // !_CRT_SECURE_NO_WARNINGS


#include "stdafx.h"


#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "mmsystem.h"
#include "config.h"
#include "lame3/lame.h"				//需要libmp3lame.lib 和 libmp3lame.dll 使用lame库lame-3.100
#include "VoiceSample.h"
#pragma comment(lib, "winmm.lib")

#define FRAGMENT_SIZE 4096        // 设置缓存区大小  
#define FRAGMENT_NUM 4            // 设置缓存区个数  
#define VOICE_HZ		44100		//采样频率
#define CHANNEL			2			//声道
#define SAMPL_BITS		16			//采样位
#define RECORD_TIME		20000		//采样时间
#define INBUFSIZE 4096  
#define MP3BUFSIZE (int) (1.25 * INBUFSIZE) + 7200  
void PlayMusi();
void WaveInitFormat(LPWAVEFORMATEX m_WaveFormat, WORD nCh, DWORD nSampleRate, WORD BitsPerSample);
DWORD CALLBACK MicCallback(HWAVEIN hwavein, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2);
//采样
void RecordWave(const FILE *fp);
void WriteHeadObjects(FILE *fp);
int addWavHeader(FILE* fd, WAVHEAD &DestionFileHeader, unsigned int bits, unsigned int chn, unsigned int hz);
int finishWavHeader(FILE *fd, WAVHEAD &DestionFileHeader, unsigned long nFileLen);
int encode(char* inPath, char* outPath);
//保存成wav格式
void RecordWaveSaveWav();
//保存成mp3格式
void RecordWaveSaveMp3();

void main()
{
	VoiceSample Vsampl;
	Vsampl.RecordWaveSaveMp3("mp3.mp3");
	system("pause"); 
}
void RecordWave(const FILE *fp)
{
	int count = waveInGetNumDevs();//1
	printf("\n音频输入数量：%d\n", count);
	/*
	MMRESULT waveInGetDevCaps(
		UINT_PTR   uDeviceID,
		LPWAVEINCAPS pwic,
		UINT     cbwic
	);
	
	uDeviceID 音频输入设备标识,也可以为一个打开的音频输入设备的句柄.
		个人认为如果上一步获得了多个设备，可以用索引标识每一个设备.
	  
	pwic 对WAVEINCAPS结构体的一个指针,包含设备的音频特性.
	
	cbwic WAVEINCAPS结构体的大小,使用sizeof即可.
	
	MMRESULT 函数执行的结果
	MMSYSERR_NOERROR 表示执行成功
	MMSYSERR_BADDEVICEID 索引越界 
	MMSYSERR_NODRIVER 没有就绪的设备 
	*/
	/*
	
	typedef struct { 
	  WORD   wMid;        //音频设备制造商定义的驱动程序标识
	  WORD   wPid;        //音频输入设备的产品标识
	  MMVERSION vDriverVersion;    //驱动程序版本号
	  TCHAR   szPname[MAXPNAMELEN];//制造商名称
	  DWORD   dwFormats;      //支持的格式,参见MSDN
	  WORD   wChannels;      //支持的声道数
	  WORD   wReserved1;      //保留参数
	} WAVEINCAPS;
	
	*/
	WAVEINCAPS waveIncaps;
	MMRESULT mmResult = waveInGetDevCaps(0, &waveIncaps, sizeof(WAVEINCAPS));//2
	printf("\n音频输入设备：%ws\n", waveIncaps.szPname);

	printf("\n支持的格式: %#2X", waveIncaps.dwFormats);		//0xfffff支持所有格式
	printf("\n支持的声道数: %d\n", waveIncaps.wChannels);
	if (MMSYSERR_NOERROR == mmResult)
	{
		HWAVEIN hWave;
		/*
		
		typedef struct { 
		  WORD wFormatTag;    //波形声音的格式,单声道双声道使用WAVE_FORMAT_PCM.当包含在WAVEFORMATEXTENSIBLE结构中时,使用WAVE_FORMAT_EXTENSIBLE.
		  WORD nChannels;    //声道数量
		  DWORD nSamplesPerSec;  //采样率.wFormatTag为WAVE_FORMAT_PCM时,有8.0kHz,11.025kHz,22.05kHz,和44.1kHz.
		  DWORD nAvgBytesPerSec;  //每秒的采样字节数.通过nSamplesPerSec * nChannels * wBitsPerSample / 8计算
		  WORD nBlockAlign;    //每次采样的字节数.通过nChannels * wBitsPerSample / 8计算
		  WORD wBitsPerSample;  //采样位数.wFormatTag为WAVE_FORMAT_PCM时,为8或者16
		  WORD cbSize;      //wFormatTag为WAVE_FORMAT_PCM时,忽略此参数
		} WAVEFORMATEX;

		*/
		WAVEFORMATEX pwfx;
		WaveInitFormat(&pwfx, CHANNEL, VOICE_HZ, SAMPL_BITS);
		printf("\n请求打开音频输入设备");
		//printf("\n采样参数：双声道 8kHz 8bit\n");
		/*
		
		MMRESULT waveInOpen(
		 LPHWAVEIN    phwi,        //接收打开的音频输入设备标识的HWAVEIN结构的指针
		 UINT_PTR    uDeviceID,      //指定一个需要打开的设备标识.可以使用WAVE_MAPPER选择一个按指定录音格式录音的设备
		 LPWAVEFORMATEX pwfx,        //一个所需的格式进行录音的WAVEFORMATEX结构的指针 
		 DWORD_PTR   dwCallback,    //指向一个回调函数、事件句柄、窗口句柄、线程标识,对录音事件进行处理.
		 DWORD_PTR   dwCallbackInstance, //传给回调机制的参数 (要保存到文件需要文件指针)
		 DWORD     fdwOpen      //打开设备的方法标识,指定回调的类型.参见CSDN

		*/
		mmResult = waveInOpen(&hWave, WAVE_MAPPER, &pwfx, (DWORD)(MicCallback), (DWORD_PTR)fp, CALLBACK_FUNCTION);//3

		if (MMSYSERR_NOERROR == mmResult)
		{
			//WAVEHDR pwh1;
			//char buffer1[10240];
			//pwh1.lpData = buffer1;
			//pwh1.dwBufferLength = 10240;
			//pwh1.dwUser = 1;
			//pwh1.dwFlags = 0;
			//mmResult = waveInPrepareHeader(phwi, &pwh1, sizeof(WAVEHDR));//4
			//printf("\n准备缓冲区1");
			/*
			
			typedef struct wavehdr_tag { 
			  LPSTR   lpData;     //指向波形格式的缓冲区
			  DWORD   dwBufferLength; //缓冲区的大小
			  DWORD   dwBytesRecorded; //当前存储了多少数据
			  DWORD_PTR dwUser;     //用户数据
			  DWORD   dwFlags;      //为缓冲区提供的信息,在waveInPrepareHeader函数中使用WHDR_PREPARED
			  DWORD   dwLoops;     //输出时使用,标识播放次数
			  struct wavehdr_tag * lpNext;//reserved
			  DWORD_PTR reserved;     //reserved
			} WAVEHDR, *LPWAVEHDR; 
 
			*/
			static WAVEHDR waveHDR[FRAGMENT_NUM];
			for (int i = 0; i < FRAGMENT_NUM; i++)
			{
				waveHDR[i].lpData = new char[FRAGMENT_SIZE];
				waveHDR[i].dwBufferLength = FRAGMENT_SIZE;
				waveHDR[i].dwBytesRecorded = 0;
				waveHDR[i].dwUser = i;
				waveHDR[i].dwFlags = 0;
				waveHDR[i].dwLoops = 1;
				waveHDR[i].lpNext = NULL;
				waveHDR[i].reserved = 0;
				waveInUnprepareHeader(hWave, &waveHDR[i], sizeof(WAVEHDR));
				//准备缓冲区
				waveInPrepareHeader(hWave, &waveHDR[i], sizeof(WAVEHDR));
				//添加buffer到音频采集
				waveInAddBuffer(hWave, &waveHDR[i], sizeof(WAVEHDR));
			}
			printf("\n将缓冲区1加入音频输入设备");

			if (MMSYSERR_NOERROR == mmResult)
			{
				//开始录音
				mmResult = waveInStart(hWave);//6
				printf("\n请求开始录音\n");
			}
			Sleep(RECORD_TIME);
			printf("录音结束\n");
			mmResult = waveInStop(hWave);
			if (MMSYSERR_NOERROR == mmResult)
			{
				printf("waveInStop\n");
			}
			
			mmResult = waveInReset(hWave);
			if (MMSYSERR_NOERROR == mmResult)
			{
				printf("waveInReset\n");
			}
			Sleep(1000); 
			for (int i = 0; i < FRAGMENT_NUM; i++)
			{
				waveInUnprepareHeader(hWave, &waveHDR[i], sizeof(WAVEHDR));
				delete waveHDR[i].lpData;
				waveHDR[i].lpData = NULL;
				printf("%d\n", i);
			}

			waveInClose(hWave);
		}
		
	}
	
}

void WriteHeadObjects(FILE *fp)
{
	unsigned char HeadDat[] = { 0x30, 0x26, 0xB2, 0x75, 0x8E, 0x66, 0xCF, 0x11, 0xA6, 0xD9, 0x00, 0xAA, 0x00, 0x62, 0xCE, 0x6C };	//固定格式		
	unsigned char tol[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x01, 0x02 };//文件大小 8个字节 + 位置6字节

	fwrite(HeadDat, 1, sizeof(HeadDat), fp);
	//printf("HeadDatftell=%d\n", ftell(fp));
	fwrite(tol, 1, sizeof(tol), fp);
	// Header Object 之 Fileproperties object
	unsigned char Fileproperties[] = { 0xA1, 0xDC, 0xAB, 0x8C, 0x47, 0xA9, 0xCF, 0x11 , 0x8E, 0xE4, 0x00, 0xC0, 0x0C, 0x20, 0x53, 0x65 };
	fwrite(Fileproperties, 1, sizeof(Fileproperties), fp);
	//Data Object//36 26 B2 75 8E 66 CF 11 A6 D9 00 AA 00 62 CE 6C
	unsigned char DatObject[] = { 0x36, 0x26, 0xB2, 0x75, 0x8E, 0x66, 0xCF, 0x11, 0xA6, 0xD9, 0x00, 0xAA, 0x00, 0x62, 0xCE, 0x6C };
	BITMAPINFOHEADER;
	printf("tol__ftell=%d\n", ftell(fp));
	printf("long long size=%d", sizeof(long long));
}

/*
void CALLBACK waveInProc(
HWAVEIN hwi,     //回调此函数的设备句柄
UINT uMsg,      //波形声音输入信息,标识关闭(WIM_CLOSE)、缓冲区满(WIM_DATA)、打开(WIM_OPEN).
DWORD_PTR dwInstance, //用户在waveInOpen指定的数据
DWORD_PTR dwParam1,  //(LPWAVEHDR)dwParam1,用户指定的缓冲区
DWORD_PTR dwParam2
);
*/
DWORD CALLBACK MicCallback(HWAVEIN hwavein, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
{
	switch (uMsg)
	{
	case WIM_OPEN:
		printf("\n设备已经打开...\n");
		break;

	case WIM_DATA:
		if (((LPWAVEHDR)dwParam1)->dwBytesRecorded < FRAGMENT_SIZE)
		{
			printf("缓冲区没有满 大小=%d", ((LPWAVEHDR)dwParam1)->dwBytesRecorded);
			fwrite(((LPWAVEHDR)dwParam1)->lpData, 1, ((LPWAVEHDR)dwParam1)->dwBytesRecorded, (FILE*)dwInstance);
			break;
		}
		printf("\n缓冲区%d存满...\n", ((LPWAVEHDR)dwParam1)->dwUser);
		//写出数据
		fwrite(((LPWAVEHDR)dwParam1)->lpData, 1, ((LPWAVEHDR)dwParam1)->dwBytesRecorded, (FILE*)dwInstance);   
		printf("\n成功写入文件%d字节\n", ((LPWAVEHDR)dwParam1)->dwBytesRecorded);

		waveInAddBuffer(hwavein, (LPWAVEHDR)dwParam1, sizeof(WAVEHDR));
		break;

	case WIM_CLOSE:
		printf("\n设备已经关闭...\n");
		break;
	default:
		break;
	}
	return 0;
}
void WaveInitFormat(LPWAVEFORMATEX m_WaveFormat, WORD nCh, DWORD nSampleRate, WORD BitsPerSample)
{
	m_WaveFormat->wFormatTag = WAVE_FORMAT_PCM;
	m_WaveFormat->nChannels = nCh;
	m_WaveFormat->nSamplesPerSec = nSampleRate;
	m_WaveFormat->nAvgBytesPerSec = nSampleRate * nCh * BitsPerSample / 8;
	m_WaveFormat->nBlockAlign = m_WaveFormat->nChannels * BitsPerSample / 8;
	m_WaveFormat->wBitsPerSample = BitsPerSample;
	m_WaveFormat->cbSize = 0;
}
void PlayMusi()
{
	int error = mciSendString(L"C:\\Users\\Administrator\\Desktop\\陈奕迅-十年.mp3 alias myDivece", NULL, 0, NULL);
	if (error == 0)
	{
		mciSendString(L"play myDivece", NULL, 0, NULL); //播放
	}
}



int addWavHeader( FILE* fd, WAVHEAD &DestionFileHeader, unsigned int bits, unsigned int chn, unsigned int hz)
{
	// 开始准备WAV的文件头 
	DestionFileHeader.RIFFNAME[0] = 'R';
	DestionFileHeader.RIFFNAME[1] = 'I';
	DestionFileHeader.RIFFNAME[2] = 'F';
	DestionFileHeader.RIFFNAME[3] = 'F';

	DestionFileHeader.WAVNAME[0] = 'W';
	DestionFileHeader.WAVNAME[1] = 'A';
	DestionFileHeader.WAVNAME[2] = 'V';
	DestionFileHeader.WAVNAME[3] = 'E';

	DestionFileHeader.FMTNAME[0] = 'f';
	DestionFileHeader.FMTNAME[1] = 'm';
	DestionFileHeader.FMTNAME[2] = 't';
	DestionFileHeader.FMTNAME[3] = 0x20;
	DestionFileHeader.nFMTLength = 16; // 表示 FMT 的长度 
	DestionFileHeader.nAudioFormat = 1; //这个表示a law PCM

	DestionFileHeader.DATANAME[0] = 'd';
	DestionFileHeader.DATANAME[1] = 'a';
	DestionFileHeader.DATANAME[2] = 't';
	DestionFileHeader.DATANAME[3] = 'a';
	DestionFileHeader.nBitsPerSample = bits;
	DestionFileHeader.nBytesPerSample = bits*chn / 8; // 
	DestionFileHeader.nSampleRate = hz; // 
	DestionFileHeader.nBytesPerSecond = bits*hz*chn / 8;
	DestionFileHeader.nChannleNumber = chn;

	size_t nSize = sizeof(DestionFileHeader);

	int nWrite = fwrite(&DestionFileHeader, 1, nSize, fd);
	if (nWrite != nSize)
	{
		fprintf(stderr, "write %d bytes/n", nWrite);
		perror("ERROR:");
		return -1;
	}

	return 0;
}

int finishWavHeader(FILE *fd, WAVHEAD &DestionFileHeader,  unsigned long nFileLen)
{
	fseek(fd, 0, SEEK_SET);
	size_t nSize = sizeof(DestionFileHeader);
	DestionFileHeader.nRIFFLength = nFileLen - 8 + nSize;
	DestionFileHeader.nDataLength = nFileLen;
	int nWrite = fwrite(&DestionFileHeader, 1, nSize, fd);
	if (nWrite != nSize)
	{
		return -1;
	}

	return 0;
}

int encode(char* inPath, char* outPath)
{
	int status = 0;
	lame_global_flags* gfp;
	int ret_code;
	FILE* infp;
	FILE* outfp;
	short* input_buffer;
	int input_samples;
	unsigned char * mp3_buffer;
	int mp3_bytes;

	gfp = lame_init();
	if (gfp == NULL)
	{
		printf("lame_init failed/n");
		status = -1;
		goto exit;
	}

	ret_code = lame_init_params(gfp);
	if (ret_code < 0)
	{
		printf("lame_init_params returned %d/n", ret_code);
		status = -1;
		goto close_lame;
	}

	infp = fopen(inPath, "rb");
	outfp = fopen(outPath, "wb");

	input_buffer = (short*)malloc(INBUFSIZE * 2);
	mp3_buffer = (unsigned char *)malloc(MP3BUFSIZE);

	do
	{
		input_samples = fread(input_buffer, 2, INBUFSIZE, infp);
		printf("input_samples is %d./n", input_samples);
		//fprintf(stderr, "input_samples is %d./n", input_samples);  
		mp3_bytes = lame_encode_buffer_interleaved(gfp, input_buffer, input_samples / 2, mp3_buffer, MP3BUFSIZE);
		//fprintf(stderr, "mp3_bytes is %d./n", mp3_bytes);  
		if (mp3_bytes < 0)
		{
			printf("lame_encode_buffer_interleaved returned %d/n", mp3_bytes);
			status = -1;
			goto free_buffers;
		}
		else if (mp3_bytes > 0)
		{
			fwrite(mp3_buffer, 1, mp3_bytes, outfp);
		}
	} while (input_samples == INBUFSIZE);

	mp3_bytes = lame_encode_flush(gfp, mp3_buffer, sizeof(mp3_buffer));
	if (mp3_bytes > 0)
	{
		printf("writing %d mp3 bytes/n", mp3_bytes);
		fwrite(mp3_buffer, 1, mp3_bytes, outfp);
	}
free_buffers:
	free(mp3_buffer);
	free(input_buffer);

	fclose(outfp);
	fclose(infp);
close_lame:
	lame_close(gfp);
exit:
	return status;
}

void RecordWaveSaveWav()
{
	FILE * fp = fopen("./ecord.wav", "wb");
	if (fp == NULL)
	{
		printf("文件打开失败");
		return;
	}
	//增加文件头信息
	WAVHEAD wavhead;
	addWavHeader(fp, wavhead, SAMPL_BITS, CHANNEL, VOICE_HZ);
	//开始录音
	RecordWave(fp);
	long size = ftell(fp);
	printf("-------%#4X\n", size);
	//将文件大小写入文件头信息中
	finishWavHeader(fp, wavhead, size);
	printf("ftell=%d", ftell(fp));
	fclose(fp);
}

void RecordWaveSaveMp3()
{
	FILE * fp = fopen("./ecord.pcm", "wb");
	if (fp == NULL)
	{
		printf("文件打开失败");
		return;
	}
	//录音保存信息格式为PCM格式
	RecordWave(fp);
	long size = ftell(fp);
	printf("-------%#4X\n", size);
	fclose(fp);
	//将PCM格式转为mp3格式
	encode("ecord.pcm", "MP3.mp3");
}

