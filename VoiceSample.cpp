#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "mmsystem.h"
#include "config.h"
#include "lame3/lame.h"
#include "VoiceSample.h"

#define FRAGMENT_SIZE 4096        // ���û�������С  
#define FRAGMENT_NUM 4            // ���û���������  
#define VOICE_HZ		44100	
#define CHANNEL			2
#define SAMPL_BITS		16
#define RECORD_TIME		20000
#define INBUFSIZE 4096  
#define MP3BUFSIZE (int) (1.25 * INBUFSIZE) + 7200  

static DWORD CALLBACK MicCallback(HWAVEIN hwavein, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2);

VoiceSample::VoiceSample()
{
}

VoiceSample::~VoiceSample()
{
}

int VoiceSample::encode(const char * inPath, const char * outPath)
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

void VoiceSample::RecordWave(const FILE *fp)
{
	//��ȡ�ɼ������豸����
	int count = waveInGetNumDevs();//1
	printf("\n��Ƶ����������%d\n", count);
	//��ȡ�豸��Ϣ
	WAVEINCAPS waveIncaps;
	MMRESULT mmResult = waveInGetDevCaps(0, &waveIncaps, sizeof(WAVEINCAPS));//2
	printf("\n��Ƶ�����豸��%ws\n", waveIncaps.szPname);

	printf("\n֧�ֵĸ�ʽ: %#2X", waveIncaps.dwFormats);		//0xfffff֧�����и�ʽ
	printf("\n֧�ֵ�������: %d\n", waveIncaps.wChannels);
	if (MMSYSERR_NOERROR == mmResult)
	{
		HWAVEIN hWave;
		WAVEFORMATEX pwfx;
		WaveInitFormat(&pwfx, CHANNEL, VOICE_HZ, SAMPL_BITS);
		printf("\n�������Ƶ�����豸");

		mmResult = waveInOpen(&hWave, WAVE_MAPPER, &pwfx, (DWORD)(MicCallback), (DWORD_PTR)fp, CALLBACK_FUNCTION);//3

		if (MMSYSERR_NOERROR == mmResult)
		{
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
			//10s �����ļ�160K
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
void VoiceSample::WaveInitFormat(LPWAVEFORMATEX m_WaveFormat, WORD nCh, DWORD nSampleRate, WORD BitsPerSample)
{
	m_WaveFormat->wFormatTag = WAVE_FORMAT_PCM;
	m_WaveFormat->nChannels = nCh;
	m_WaveFormat->nSamplesPerSec = nSampleRate;
	m_WaveFormat->nAvgBytesPerSec = nSampleRate * nCh * BitsPerSample / 8;
	m_WaveFormat->nBlockAlign = m_WaveFormat->nChannels * BitsPerSample / 8;
	m_WaveFormat->wBitsPerSample = BitsPerSample;
	m_WaveFormat->cbSize = 0;
}
void VoiceSample::RecordWaveSaveMp3(const char * outPath)
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
	encode("ecord.pcm", outPath);
}