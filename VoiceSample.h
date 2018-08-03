#ifndef __VOICESAMPLE_H__	
#define __VOICESAMPLE_H__

class VoiceSample
{
public:
	VoiceSample();
	~VoiceSample();
public:
	//声音采集
	void RecordWave(const FILE *fp);
	//从PCM转mp3
	int encode(const char* inPath, const char* outPath);
	//
	void WaveInitFormat(LPWAVEFORMATEX m_WaveFormat, WORD nCh, DWORD nSampleRate, WORD BitsPerSample);
	//保存为MP3格式
	void RecordWaveSaveMp3(const char * outPath);
};
#endif	
