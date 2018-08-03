#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "mmsystem.h"
#include "config.h"
#include "lame3/lame.h"
#include "VoiceSample.h"

#define FRAGMENT_SIZE 4096        // 设置缓存区大小  
#define FRAGMENT_NUM 4            // 设置缓存区个数  
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
	//获取采集声音设备数量
	int count = waveInGetNumDevs();//1
	printf("\n音频输入数量：%d\n", count);
	//获取设备信息
	WAVEINCAPS waveIncaps;
	MMRESULT mmResult = waveInGetDevCaps(0, &waveIncaps, sizeof(WAVEINCAPS));//2
	printf("\n音频输入设备：%ws\n", waveIncaps.szPname);

	printf("\n支持的格式: %#2X", waveIncaps.dwFormats);		//0xfffff支持所有格式
	printf("\n支持的声道数: %d\n", waveIncaps.wChannels);
	if (MMSYSERR_NOERROR == mmResult)
	{
		HWAVEIN hWave;
		WAVEFORMATEX pwfx;
		WaveInitFormat(&pwfx, CHANNEL, VOICE_HZ, SAMPL_BITS);
		printf("\n请求打开音频输入设备");

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
			//10s 生成文件160K
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
		printf("文件打开失败");
		return;
	}
	//录音保存信息格式为PCM格式
	RecordWave(fp);
	long size = ftell(fp);
	printf("-------%#4X\n", size);
	fclose(fp);
	//将PCM格式转为mp3格式
	encode("ecord.pcm", outPath);
}