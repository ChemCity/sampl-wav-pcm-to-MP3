#ifndef __VOICESAMPLE_H__	
#define __VOICESAMPLE_H__

class VoiceSample
{
public:
	VoiceSample();
	~VoiceSample();
public:
	//�����ɼ�
	void RecordWave(const FILE *fp);
	//��PCMתmp3
	int encode(const char* inPath, const char* outPath);
	//
	void WaveInitFormat(LPWAVEFORMATEX m_WaveFormat, WORD nCh, DWORD nSampleRate, WORD BitsPerSample);
	//����ΪMP3��ʽ
	void RecordWaveSaveMp3(const char * outPath);
};
#endif	
