// CVoice.cpp : �������̨Ӧ�ó������ڵ㡣
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
#include "lame3/lame.h"				//��Ҫlibmp3lame.lib �� libmp3lame.dll ʹ��lame��lame-3.100
#include "VoiceSample.h"
#pragma comment(lib, "winmm.lib")

#define FRAGMENT_SIZE 4096        // ���û�������С  
#define FRAGMENT_NUM 4            // ���û���������  
#define VOICE_HZ		44100		//����Ƶ��
#define CHANNEL			2			//����
#define SAMPL_BITS		16			//����λ
#define RECORD_TIME		20000		//����ʱ��
#define INBUFSIZE 4096  
#define MP3BUFSIZE (int) (1.25 * INBUFSIZE) + 7200  
void PlayMusi();
void WaveInitFormat(LPWAVEFORMATEX m_WaveFormat, WORD nCh, DWORD nSampleRate, WORD BitsPerSample);
DWORD CALLBACK MicCallback(HWAVEIN hwavein, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2);
//����
void RecordWave(const FILE *fp);
void WriteHeadObjects(FILE *fp);
int addWavHeader(FILE* fd, WAVHEAD &DestionFileHeader, unsigned int bits, unsigned int chn, unsigned int hz);
int finishWavHeader(FILE *fd, WAVHEAD &DestionFileHeader, unsigned long nFileLen);
int encode(char* inPath, char* outPath);
//�����wav��ʽ
void RecordWaveSaveWav();
//�����mp3��ʽ
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
	printf("\n��Ƶ����������%d\n", count);
	/*
	MMRESULT waveInGetDevCaps(
		UINT_PTR   uDeviceID,
		LPWAVEINCAPS pwic,
		UINT     cbwic
	);
	
	uDeviceID ��Ƶ�����豸��ʶ,Ҳ����Ϊһ���򿪵���Ƶ�����豸�ľ��.
		������Ϊ�����һ������˶���豸��������������ʶÿһ���豸.
	  
	pwic ��WAVEINCAPS�ṹ���һ��ָ��,�����豸����Ƶ����.
	
	cbwic WAVEINCAPS�ṹ��Ĵ�С,ʹ��sizeof����.
	
	MMRESULT ����ִ�еĽ��
	MMSYSERR_NOERROR ��ʾִ�гɹ�
	MMSYSERR_BADDEVICEID ����Խ�� 
	MMSYSERR_NODRIVER û�о������豸 
	*/
	/*
	
	typedef struct { 
	  WORD   wMid;        //��Ƶ�豸�����̶�������������ʶ
	  WORD   wPid;        //��Ƶ�����豸�Ĳ�Ʒ��ʶ
	  MMVERSION vDriverVersion;    //��������汾��
	  TCHAR   szPname[MAXPNAMELEN];//����������
	  DWORD   dwFormats;      //֧�ֵĸ�ʽ,�μ�MSDN
	  WORD   wChannels;      //֧�ֵ�������
	  WORD   wReserved1;      //��������
	} WAVEINCAPS;
	
	*/
	WAVEINCAPS waveIncaps;
	MMRESULT mmResult = waveInGetDevCaps(0, &waveIncaps, sizeof(WAVEINCAPS));//2
	printf("\n��Ƶ�����豸��%ws\n", waveIncaps.szPname);

	printf("\n֧�ֵĸ�ʽ: %#2X", waveIncaps.dwFormats);		//0xfffff֧�����и�ʽ
	printf("\n֧�ֵ�������: %d\n", waveIncaps.wChannels);
	if (MMSYSERR_NOERROR == mmResult)
	{
		HWAVEIN hWave;
		/*
		
		typedef struct { 
		  WORD wFormatTag;    //���������ĸ�ʽ,������˫����ʹ��WAVE_FORMAT_PCM.��������WAVEFORMATEXTENSIBLE�ṹ��ʱ,ʹ��WAVE_FORMAT_EXTENSIBLE.
		  WORD nChannels;    //��������
		  DWORD nSamplesPerSec;  //������.wFormatTagΪWAVE_FORMAT_PCMʱ,��8.0kHz,11.025kHz,22.05kHz,��44.1kHz.
		  DWORD nAvgBytesPerSec;  //ÿ��Ĳ����ֽ���.ͨ��nSamplesPerSec * nChannels * wBitsPerSample / 8����
		  WORD nBlockAlign;    //ÿ�β������ֽ���.ͨ��nChannels * wBitsPerSample / 8����
		  WORD wBitsPerSample;  //����λ��.wFormatTagΪWAVE_FORMAT_PCMʱ,Ϊ8����16
		  WORD cbSize;      //wFormatTagΪWAVE_FORMAT_PCMʱ,���Դ˲���
		} WAVEFORMATEX;

		*/
		WAVEFORMATEX pwfx;
		WaveInitFormat(&pwfx, CHANNEL, VOICE_HZ, SAMPL_BITS);
		printf("\n�������Ƶ�����豸");
		//printf("\n����������˫���� 8kHz 8bit\n");
		/*
		
		MMRESULT waveInOpen(
		 LPHWAVEIN    phwi,        //���մ򿪵���Ƶ�����豸��ʶ��HWAVEIN�ṹ��ָ��
		 UINT_PTR    uDeviceID,      //ָ��һ����Ҫ�򿪵��豸��ʶ.����ʹ��WAVE_MAPPERѡ��һ����ָ��¼����ʽ¼�����豸
		 LPWAVEFORMATEX pwfx,        //һ������ĸ�ʽ����¼����WAVEFORMATEX�ṹ��ָ�� 
		 DWORD_PTR   dwCallback,    //ָ��һ���ص��������¼���������ھ�����̱߳�ʶ,��¼���¼����д���.
		 DWORD_PTR   dwCallbackInstance, //�����ص����ƵĲ��� (Ҫ���浽�ļ���Ҫ�ļ�ָ��)
		 DWORD     fdwOpen      //���豸�ķ�����ʶ,ָ���ص�������.�μ�CSDN

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
			//printf("\n׼��������1");
			/*
			
			typedef struct wavehdr_tag { 
			  LPSTR   lpData;     //ָ���θ�ʽ�Ļ�����
			  DWORD   dwBufferLength; //�������Ĵ�С
			  DWORD   dwBytesRecorded; //��ǰ�洢�˶�������
			  DWORD_PTR dwUser;     //�û�����
			  DWORD   dwFlags;      //Ϊ�������ṩ����Ϣ,��waveInPrepareHeader������ʹ��WHDR_PREPARED
			  DWORD   dwLoops;     //���ʱʹ��,��ʶ���Ŵ���
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
				//׼��������
				waveInPrepareHeader(hWave, &waveHDR[i], sizeof(WAVEHDR));
				//���buffer����Ƶ�ɼ�
				waveInAddBuffer(hWave, &waveHDR[i], sizeof(WAVEHDR));
			}
			printf("\n��������1������Ƶ�����豸");

			if (MMSYSERR_NOERROR == mmResult)
			{
				//��ʼ¼��
				mmResult = waveInStart(hWave);//6
				printf("\n����ʼ¼��\n");
			}
			Sleep(RECORD_TIME);
			printf("¼������\n");
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
	unsigned char HeadDat[] = { 0x30, 0x26, 0xB2, 0x75, 0x8E, 0x66, 0xCF, 0x11, 0xA6, 0xD9, 0x00, 0xAA, 0x00, 0x62, 0xCE, 0x6C };	//�̶���ʽ		
	unsigned char tol[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x01, 0x02 };//�ļ���С 8���ֽ� + λ��6�ֽ�

	fwrite(HeadDat, 1, sizeof(HeadDat), fp);
	//printf("HeadDatftell=%d\n", ftell(fp));
	fwrite(tol, 1, sizeof(tol), fp);
	// Header Object ֮ Fileproperties object
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
HWAVEIN hwi,     //�ص��˺������豸���
UINT uMsg,      //��������������Ϣ,��ʶ�ر�(WIM_CLOSE)����������(WIM_DATA)����(WIM_OPEN).
DWORD_PTR dwInstance, //�û���waveInOpenָ��������
DWORD_PTR dwParam1,  //(LPWAVEHDR)dwParam1,�û�ָ���Ļ�����
DWORD_PTR dwParam2
);
*/
DWORD CALLBACK MicCallback(HWAVEIN hwavein, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
{
	switch (uMsg)
	{
	case WIM_OPEN:
		printf("\n�豸�Ѿ���...\n");
		break;

	case WIM_DATA:
		if (((LPWAVEHDR)dwParam1)->dwBytesRecorded < FRAGMENT_SIZE)
		{
			printf("������û���� ��С=%d", ((LPWAVEHDR)dwParam1)->dwBytesRecorded);
			fwrite(((LPWAVEHDR)dwParam1)->lpData, 1, ((LPWAVEHDR)dwParam1)->dwBytesRecorded, (FILE*)dwInstance);
			break;
		}
		printf("\n������%d����...\n", ((LPWAVEHDR)dwParam1)->dwUser);
		//д������
		fwrite(((LPWAVEHDR)dwParam1)->lpData, 1, ((LPWAVEHDR)dwParam1)->dwBytesRecorded, (FILE*)dwInstance);   
		printf("\n�ɹ�д���ļ�%d�ֽ�\n", ((LPWAVEHDR)dwParam1)->dwBytesRecorded);

		waveInAddBuffer(hwavein, (LPWAVEHDR)dwParam1, sizeof(WAVEHDR));
		break;

	case WIM_CLOSE:
		printf("\n�豸�Ѿ��ر�...\n");
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
	int error = mciSendString(L"C:\\Users\\Administrator\\Desktop\\����Ѹ-ʮ��.mp3 alias myDivece", NULL, 0, NULL);
	if (error == 0)
	{
		mciSendString(L"play myDivece", NULL, 0, NULL); //����
	}
}



int addWavHeader( FILE* fd, WAVHEAD &DestionFileHeader, unsigned int bits, unsigned int chn, unsigned int hz)
{
	// ��ʼ׼��WAV���ļ�ͷ 
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
	DestionFileHeader.nFMTLength = 16; // ��ʾ FMT �ĳ��� 
	DestionFileHeader.nAudioFormat = 1; //�����ʾa law PCM

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
		printf("�ļ���ʧ��");
		return;
	}
	//�����ļ�ͷ��Ϣ
	WAVHEAD wavhead;
	addWavHeader(fp, wavhead, SAMPL_BITS, CHANNEL, VOICE_HZ);
	//��ʼ¼��
	RecordWave(fp);
	long size = ftell(fp);
	printf("-------%#4X\n", size);
	//���ļ���Сд���ļ�ͷ��Ϣ��
	finishWavHeader(fp, wavhead, size);
	printf("ftell=%d", ftell(fp));
	fclose(fp);
}

void RecordWaveSaveMp3()
{
	FILE * fp = fopen("./ecord.pcm", "wb");
	if (fp == NULL)
	{
		printf("�ļ���ʧ��");
		return;
	}
	//¼��������Ϣ��ʽΪPCM��ʽ
	RecordWave(fp);
	long size = ftell(fp);
	printf("-------%#4X\n", size);
	fclose(fp);
	//��PCM��ʽתΪmp3��ʽ
	encode("ecord.pcm", "MP3.mp3");
}

