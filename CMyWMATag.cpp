//////////////////////////////////////////////////////////////////////////  
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif // !_CRT_SECURE_NO_WARNINGS
#include <Windows.h>  
#include <string.h>
#include "CMyWMATag.h" 

//////////////////////////////////////////////////////////////////////////  
// ���캯��  
CMyWMATag::CMyWMATag()
{
	// ��ʼ����Ա����  
	ClearDataInfo();

}

// ��������  
CMyWMATag::~CMyWMATag()
{

}

//////////////////////////////////////////////////////////////////////////  
// �ӿں���  
// ����WMA�ļ����ɹ����������ӿڻ�ȡ��Ϣ  
bool CMyWMATag::AnalyzeWMAFile(const char *filePathName)
{
	// ����ϴ���Ϣ  
	ClearDataInfo();

	// ������Ч��  
	if (filePathName == NULL)
	{
		return false;
	}

	// ���ļ�  
	FILE *pWMAFile = fopen(filePathName, "rb");
	if (pWMAFile == NULL)
	{
		return false;
	}

	// ��ȡWMAͷ�ṹ  
	memset(&m_HeadInfo, 0, sizeof(m_HeadInfo));
	if (fread(&m_HeadInfo, sizeof(m_HeadInfo), 1, pWMAFile) != 1)
	{
		fclose(pWMAFile);
		return false;
	}

	// �ж��Ƿ�WMA�ļ�  
	m_bAnalyzeSuccess = BeWMAFile(m_HeadInfo.HeadFlag);
	if (!m_bAnalyzeSuccess)
	{
		memset(&m_HeadInfo, 0, sizeof(m_HeadInfo));
		fclose(pWMAFile);
		return false;
	}

	// ����Ҫ��ȡ֡���ݵ��ֽ���  
	unsigned long lFrameDataSize = GetHeadSize(m_HeadInfo.HeadSize) - sizeof(WMAHEADINFO);

	// Ϊ֡���ݷ���ռ�  
	unsigned char *pFrameData = new unsigned char[lFrameDataSize];
	if (pFrameData == NULL)
	{
		fclose(pWMAFile);
		m_bAnalyzeSuccess = false;
		return false;
	}

	// ��ȡ֡����  
	memset(pFrameData, 0, lFrameDataSize);
	if (fread(pFrameData, lFrameDataSize, 1, pWMAFile) != 1)
	{
		// �ͷ�֡����  
		if (pFrameData != NULL)
		{
			delete[] pFrameData;
			pFrameData = NULL;
		}

		fclose(pWMAFile);
		m_bAnalyzeSuccess = false;
		return false;
	}

	// ��ȡ�ɹ��ر��ļ�  
	fclose(pWMAFile);

	// ����֡����  
	ParseFrameData(pFrameData, lFrameDataSize);

	// �ͷ�֡����  
	if (pFrameData != NULL)
	{
		delete[] pFrameData;
		pFrameData = NULL;
	}

	// ���ط������  
	return m_bAnalyzeSuccess;
}

// ����WMA�ļ�  
bool CMyWMATag::AnalyzeWMAFile(const wchar_t *filePathName)
{
	// ������Ч��  
	if (filePathName == NULL)
	{
		return false;
	}

	// ����ת��Ϊ���ֽ��ֽ���  
	int nChars = WideCharToMultiByte(CP_ACP, 0, filePathName, -1, NULL, 0, NULL, NULL);

	// ����ռ�  
	char *pFileName = new char[nChars + 1];
	if (pFileName == NULL)
	{
		return false;
	}

	// ת��Ϊ���ֽ�  
	memset(pFileName, 0, nChars + 1);
	WideCharToMultiByte(CP_ACP, 0, filePathName, -1, pFileName, nChars, NULL, NULL);

	// ����  
	bool bResult = AnalyzeWMAFile(pFileName);

	// �ͷ�  
	if (pFileName != NULL)
	{
		delete[] pFileName;
		pFileName = NULL;
	}

	// ���ؽ��  
	return bResult;
}

// ����WMA�ļ�  
bool CMyWMATag::AnalyzeWMAFile(const wstring &filePathName)
{
	return AnalyzeWMAFile(filePathName.c_str());
}

// �Ƿ�����ɹ�  
bool CMyWMATag::BeSuccess()
{
	return m_bAnalyzeSuccess;
}

// ȡ��ͷ��С  
unsigned long CMyWMATag::GetHeadSize()
{
	return GetHeadSize(m_HeadInfo.HeadSize);
}

//////////////////////////////////////////////////////////////////////////  
// ȡ��Txt�������ݽӿ�  
bool CMyWMATag::GetTxtItemContent(WMA_FRAMEID_T frameID, wstring &itemContent)
{
	// ����ʧ��  
	if (!BeSuccess())
	{
		return false;
	}

	// ����  
	for (unsigned long i = 0; i<m_WMAAttrArray.size(); i++)
	{
		if (frameID == m_WMAAttrArray[i].ID)
		{
			if (m_WMAAttrArray[i].AttrStyle == WMA_ATTRSTYLE_TXT)
			{
				itemContent = m_WMAAttrArray[i].StrContent;
				return true;
			}
			else
			{
				return false;
			}
		}
	}

	// û���ҵ�  
	return false;
}

// ȡ��Txt�������ݽӿ�  
bool CMyWMATag::GetTxtItemContent(WMA_FRAMEID_T frameID, string &itemContent)
{
	wstring wContent;
	bool bOK = GetTxtItemContent(frameID, wContent);
	itemContent = TransFromWString(wContent);
	return bOK;
}

//////////////////////////////////////////////////////////////////////////  
// ȡ��ͼƬ�������ݽӿ�  
// ȡ�õ�һ��ͼƬ  
//bool CMyWMATag::GetFirstPic(CMYPICINFO &picInfo)
//{
//	// ����ʧ��  
//	if (!BeSuccess())
//	{
//		return false;
//	}
//
//	// �����Ƿ���ͼƬ  
//	if (m_WMAPicArray.size() == 0)
//	{
//		return false;
//	}
//
//	// �õ���һ��ͼƬ  
//	picInfo = m_WMAPicArray[0].PicData;
//	return true;
//}

// �����һ��ͼƬ  
bool CMyWMATag::SaveFirstPicToFile(const char *filePathName)
{
	//if (BeSuccess() && m_WMAPicArray.size()>0)
	//{
	//	return m_WMAPicArray[0].PicData.SavePicToFile(filePathName);
	//}

	return false;
}
bool CMyWMATag::SaveFirstPicToFile(const wchar_t *filePathName)
{
	//if (BeSuccess() && m_WMAPicArray.size()>0)
	//{
	//	return m_WMAPicArray[0].PicData.SavePicToFile(filePathName);
	//}

	return false;
}

// ȡ�õ�һ��ͼƬ�ĵ�ַ����С  
const unsigned char* CMyWMATag::GetFirstPicAddr(unsigned long *pPicSize)
{
	//if (BeSuccess() && m_WMAPicArray.size()>0)
	//{
	//	if (pPicSize != NULL)
	//	{
	//		*pPicSize = m_WMAPicArray[0].PicData.GetPicBufSize();
	//	}

	//	return m_WMAPicArray[0].PicData.GetPicBufAddr();
	//}

	return NULL;
}

// ȡ�õ�һ��ͼƬ�Ĵ�С  
unsigned long CMyWMATag::GetFirstPicSize()
{
	//if (BeSuccess() && m_WMAPicArray.size()>0)
	//{
	//	return m_WMAPicArray[0].PicData.GetPicBufSize();
	//}

	return 0;
}

// ȡ��ͼƬ����  
unsigned long CMyWMATag::GetPicCounts()
{
	return (unsigned long)m_WMAPicArray.size();
}

// ȡ�õ�n��ͼƬ  
//bool CMyWMATag::GetPic(unsigned long picIndex, CMYPICINFO &picInfo)
//{
//	// ����ʧ��  
//	if (!BeSuccess())
//	{
//		return false;
//	}
//
//	// �õ�ͼƬ����  
//	unsigned long picCounts = GetPicCounts();
//
//	// �����Ƿ���ͼƬ  
//	if (picCounts == 0)
//	{
//		return false;
//	}
//
//	// �����Ƿ���Ч  
//	if (picIndex >= picCounts)
//	{
//		return false;
//	}
//
//	// �õ���picIndex��ͼƬ  
//	picInfo = m_WMAPicArray[picIndex].PicData;
//	return true;
//}

// ȡ�õ�n��ͼƬ�ĵ�ַ  
const unsigned char* CMyWMATag::GetPicAddr(unsigned long picIndex, unsigned long *pPicSize)
{
	// ����ʧ��  
	if (!BeSuccess())
	{
		return NULL;
	}

	// �õ�ͼƬ����  
	unsigned long picCounts = GetPicCounts();

	// �����Ƿ���ͼƬ  
	if (picCounts == 0)
	{
		return NULL;
	}

	// �����Ƿ���Ч  
	if (picIndex >= picCounts)
	{
		return NULL;
	}
	return NULL;
	// ȡ��ͼƬ��С  
	//if (pPicSize != NULL)
	//{
	//	*pPicSize = m_WMAPicArray[picIndex].PicData.GetPicBufSize();
	//}

	//// �õ���picIndex��ͼƬ��ַ  
	//return m_WMAPicArray[picIndex].PicData.GetPicBufAddr();
}

// �����n��ͼƬ  
bool CMyWMATag::SavePicToFile(unsigned long picIndex, const char *filePathName)
{
	// ������Ч��  
	if (filePathName == NULL)
	{
		return false;
	}

	// ����ʧ��  
	if (!BeSuccess())
	{
		return false;
	}

	// �õ�ͼƬ����  
	unsigned long picCounts = GetPicCounts();

	// �����Ƿ���ͼƬ  
	if (picCounts == 0)
	{
		return false;
	}

	// �����Ƿ���Ч  
	if (picIndex >= picCounts)
	{
		return false;
	}
	return false;
	//return m_WMAPicArray[picIndex].PicData.SavePicToFile(filePathName);
}

// �����n��ͼƬ  
bool CMyWMATag::SavePicToFile(unsigned long picIndex, const wchar_t *filePathName)
{
	// ������Ч��  
	if (filePathName == NULL)
	{
		return false;
	}

	// ����ʧ��  
	if (!BeSuccess())
	{
		return false;
	}

	// �õ�ͼƬ����  
	unsigned long picCounts = GetPicCounts();

	// �����Ƿ���ͼƬ  
	if (picCounts == 0)
	{
		return false;
	}

	// �����Ƿ���Ч  
	if (picIndex >= picCounts)
	{
		return false;
	}
	return false;
	//return m_WMAPicArray[picIndex].PicData.SavePicToFile(filePathName);
}
//////////////////////////////////////////////////////////////////////////  
// ˽�к���  
// �����ļ�ͷ�ṹ�ж��Ƿ�WMA�ļ�  
bool CMyWMATag::BeWMAFile(const WMAHEADFLAG &headFlag)
{
	for (int i = 0; i<sizeof(WMA_HEAD_FLAGCONST.WmaFlag); i++)
	{
		if (headFlag.WmaFlag[i] != WMA_HEAD_FLAGCONST.WmaFlag[i])
		{
			return false;
		}
	}

	return true;
}

// �����Ϣ  
void CMyWMATag::ClearDataInfo()
{
	memset(&m_HeadInfo, 0, sizeof(m_HeadInfo));
	m_bAnalyzeSuccess = false;
	m_WMAAttrArray.clear();
	m_WMAPicArray.clear();
}

// ����ͷ��С  
unsigned long CMyWMATag::GetHeadSize(const WMAHEADSIZE &headSize)
{
	return (unsigned long)((headSize.Size[3] << 24) | (headSize.Size[2] << 16) | (headSize.Size[1] << 8) | headSize.Size[0]);
}

// �õ�֡����  
WMA_FRAMESTYLE_T CMyWMATag::GetFrameStyle(const WMAFRAMEHEADFLAG &frameFlag)
{
	// �ж��Ƿ��׼֡  
	if (BeSameFrameFlag(frameFlag, WMA_FRAMEHEAD_STANDARD))
	{
		return WMA_FRAMESTYLE_STANDARD;
	}

	// �ж��Ƿ���չ֡  
	if (BeSameFrameFlag(frameFlag, WMA_FRAMEHEAD_EXTEND))
	{
		return WMA_FRAMESTYLE_EXTEND;
	}

	// ����λ��֡  
	return WMA_FRAMESTYLE_UNKNOWN;
}

// �ж�����֡ͷ��ʶ�Ƿ����  
bool CMyWMATag::BeSameFrameFlag(const WMAFRAMEHEADFLAG &firstFlag, const WMAFRAMEHEADFLAG &secondFlag)
{
	for (int i = 0; i<16; i++)
	{
		if (firstFlag.Flag[i] != secondFlag.Flag[i])
		{
			return false;
		}
	}

	return true;
}

// �õ�֡�Ĵ�С  
unsigned long CMyWMATag::GetFrameSize(const WMAFRAMEHEADSIZE &frameSize)
{
	return (unsigned long)((frameSize.Size[3] << 24) | (frameSize.Size[2] << 16) | (frameSize.Size[1] << 8) | frameSize.Size[0]);
}

// ������2���ֽ�ƴ�ӳɵ�������С  
unsigned long CMyWMATag::CalUInt16Val(const unsigned char *pByteBuf, int bufSize)
{
	// ������Ч��  
	if (pByteBuf == NULL || bufSize<2)
	{
		return 0;
	}

	// ����ֵ  
	return ((pByteBuf[1] << 8) | pByteBuf[0]);
}

// ������4���ֽ�ƴ�ӳɵ�������С  
unsigned long CMyWMATag::CalUInt32Val(const unsigned char *pByteBuf, int bufSize)
{
	// ������Ч��  
	if (pByteBuf == NULL || bufSize<4)
	{
		return 0;
	}

	// ����ֵ  
	return ((pByteBuf[3] << 24) | (pByteBuf[2] << 16) | (pByteBuf[1] << 8) | pByteBuf[0]);
}

// �����õ���֡����  
unsigned long CMyWMATag::ParseFrameData(const unsigned char *pFrameData, unsigned long bufLength)
{
	// ����ϴ�����  
	m_WMAAttrArray.clear();
	m_WMAPicArray.clear();

	// ������Ч��  
	if (pFrameData == NULL || bufLength == 0)
	{
		return 0;
	}

	// �����ʱ����  
	WMAFRAMEHEADINFO frameHead;
	WMA_FRAMESTYLE_T frameStyle = WMA_FRAMESTYLE_UNKNOWN;
	unsigned long frameSize = 0;
	WMAATTRINFO attrItemInfo;


	// ������������  
	unsigned long pos = 0;
	while (pos + 24 < bufLength)
	{
		// �õ�֡ͷʧ��  
		if (!GetFrameHeadInfo(pFrameData + pos, bufLength - pos, frameHead))
		{
			break;
		}

		// �õ�֡�������  
		frameStyle = GetFrameStyle(frameHead.FrameFlag);

		// ����ȷ���ȡ��֡��С  
		frameSize = GetFrameSize(frameHead.FrameSize);

		// ȡ��֡������     
		if (frameSize >= 24 && pos + 23 + frameSize<bufLength)
		{
			// �Ǳ�׼֡��������׼֡  
			if (frameStyle == WMA_FRAMESTYLE_STANDARD)
			{
				ParseStandardFrame(pFrameData + pos + 24, frameSize - 24, m_WMAAttrArray);
			}
			// ����չ֡  
			else if (frameStyle == WMA_FRAMESTYLE_EXTEND)
			{
				ParseExtendFrame(pFrameData + pos + 24, frameSize - 24, m_WMAAttrArray, m_WMAPicArray);
			}

			// �ı�����  
			pos += frameSize;
		}
		else
		{
			pos++;
		}
	}

	for (unsigned int i = 0; i<m_WMAAttrArray.size(); i++)
	{
		printf("ID:%d Content:%s/n", m_WMAAttrArray[i].ID, TransFromWString(m_WMAAttrArray[i].StrContent).c_str());
	}

	// ���ؽ�������������  
	return (unsigned long)m_WMAAttrArray.size() + (unsigned long)m_WMAPicArray.size();
}

// �õ�֡ͷ��Ϣ  
bool CMyWMATag::GetFrameHeadInfo(const unsigned char *pFrameData, unsigned long bufLength, WMAFRAMEHEADINFO &frameHead)
{
	// ������Ч��  
	if (pFrameData == NULL || bufLength<24)
	{
		return false;
	}

	// �õ�֡��ʶ  
	int i = 0;
	for (i = 0; i<16; i++)
	{
		frameHead.FrameFlag.Flag[i] = pFrameData[i];
	}

	// �õ�֡��С  
	for (i = 0; i<8; i++)
	{
		frameHead.FrameSize.Size[i] = pFrameData[i + 16];
	}

	return true;
}

// ������׼֡����  
bool CMyWMATag::ParseStandardFrame(const unsigned char *pItemFrameData, unsigned long frameSize, CWMAAttrArray &WMAAttrArray)
{
	// ������Ч��  
	if (pItemFrameData == NULL || frameSize == 0)
	{
		return false;
	}

	// ���������Ϣ��С  
	unsigned long titleSize = CalUInt16Val(pItemFrameData, 2);

	// ������������Ϣ��С  
	unsigned long artistSize = CalUInt16Val(pItemFrameData + 2, 2);

	// �����Ȩ��Ϣ��С  
	unsigned long copyRightSize = CalUInt16Val(pItemFrameData + 4, 2);

	// ���㱸ע��Ϣ��С  
	unsigned long commentSize = CalUInt16Val(pItemFrameData + 6, 2);

	// ����δ֪��Ϣ��С  
	unsigned long unknownSize = CalUInt16Val(pItemFrameData + 8, 2);

	// ƫ����  
	unsigned long offset = 10;

	// ��ʱ�����Ϣ  
	WMAATTRINFO attrInfo;
	memset(&attrInfo, 0, sizeof(attrInfo));
	attrInfo.AttrStyle = WMA_ATTRSTYLE_TXT;

	// ȡ�ñ�����Ϣ  
	attrInfo.ID = WMA_FRAMEID_TITLE;
	attrInfo.StrContent = GetWString(pItemFrameData + offset, titleSize);
	WMAAttrArray.push_back(attrInfo);
	offset += titleSize;

	// ȡ����������Ϣ  
	attrInfo.ID = WMA_FRAMEID_AUTHOR;
	attrInfo.StrContent = GetWString(pItemFrameData + offset, artistSize);
	WMAAttrArray.push_back(attrInfo);
	offset += artistSize;

	// ȡ�ð�Ȩ��Ϣ  
	attrInfo.ID = WMA_FRAMEID_COPYRIGHT;
	attrInfo.StrContent = GetWString(pItemFrameData + offset, copyRightSize);
	WMAAttrArray.push_back(attrInfo);
	offset += copyRightSize;

	// ȡ�ñ�ע��Ϣ  
	attrInfo.ID = WMA_FRAMEID_DESCRIPTION;
	attrInfo.StrContent = GetWString(pItemFrameData + offset, commentSize);
	WMAAttrArray.push_back(attrInfo);
	offset += commentSize;

	// ȡ��δ֪��Ϣ  
	attrInfo.ID = WMA_FRAMEID_RATING;
	attrInfo.StrContent = GetWString(pItemFrameData + offset, unknownSize);
	WMAAttrArray.push_back(attrInfo);
	offset += unknownSize;

	// ���سɹ�  
	return true;
}

// �õ�string  
wstring CMyWMATag::GetWString(const unsigned char* pUCharBuf, unsigned long charsNums)
{
	// ����ֵ  
	wstring strRet = L"";

	// ������Ч��  
	if (pUCharBuf == NULL || charsNums == 0)
	{
		return strRet;
	}

	// �õ��ַ���  
	strRet.append((const wchar_t*)pUCharBuf, charsNums / 2);
	return strRet;
}

// wstringת��Ϊstring  
string CMyWMATag::TransFromWString(const wstring &wstrVal)
{
	// �����ַ���  
	int nCharNums = (int)wstrVal.length();
	int mulBufSize = 2 * nCharNums + 10;

	// ����ռ�  
	char *pCharBuf = new char[mulBufSize];
	if (pCharBuf == NULL)
	{
		return "";
	}
	memset(pCharBuf, 0, mulBufSize);

	// ת��  
	WideCharToMultiByte(CP_ACP, 0, wstrVal.c_str(), -1, pCharBuf, mulBufSize, NULL, NULL);

	// �õ����  
	string strVal(pCharBuf);

	// �ͷſռ�  
	if (pCharBuf != NULL)
	{
		delete[] pCharBuf;
		pCharBuf = NULL;
	}

	return strVal;
}

// ������չ֡����  
bool CMyWMATag::ParseExtendFrame(const unsigned char *pItemFrameData, unsigned long frameSize, CWMAAttrArray &WMAAttrArray, CWMAPicArray &WMAPicArray)
{
	// ������Ч��  
	if (pItemFrameData == NULL || frameSize == 0)
	{
		return false;
	}

	// ������չ��Ϣ����  
	unsigned long exInfoCounts = CalUInt16Val(pItemFrameData, 2);

	// ��ַƫ����  
	unsigned long offset = 2;

	// ��ʱ�����չ��Ϣ  
	unsigned long exNameSize = 0;
	wstring exName = L"";
	unsigned long exFlagVal = 0;
	unsigned long valSize = 0;
	WMA_FRAMEID_T frameID = WMA_FRAMEID_UNKNOWN;
	WMAATTRINFO attrInfo;
	WMAPICITEMINFO picItem;
	unsigned lenTmp = 0;

	// ȡ��չ��Ϣ  
	for (unsigned exIndex = 0; exIndex<exInfoCounts; exIndex++)
	{
		// ����ϴ�����  
		memset(&attrInfo, 0, sizeof(attrInfo));

		// ������չ����С  
		exNameSize = CalUInt16Val(pItemFrameData + offset, 2);
		offset += 2;

		// ȡ����չ����  
		exName = GetWString(pItemFrameData + offset, exNameSize);
		offset += exNameSize;

		// ȡ����չ��ʶFLAGֵ  
		exFlagVal = CalUInt16Val(pItemFrameData + offset, 2);
		offset += 2;

		// ȡ��ʵ��ֵ�Ĵ�С  
		valSize = CalUInt16Val(pItemFrameData + offset, 2);
		offset += 2;

		// ������չ����ID  
		frameID = GetExFrameID(exName);

		// �Ƿ���ЧID  
		if (frameID != WMA_FRAMEID_UNKNOWN)
		{
			// �õ�����ID  
			attrInfo.ID = frameID;

			// ͼƬ�õ�ͼƬ��Ϣ  
			if (frameID == WMA_FRAMEID_PICTURE)
			{
				lenTmp = GetWstringByWChars(pItemFrameData + offset, frameSize - offset, picItem.MimeType);
				picItem.PicType = (char)((pItemFrameData + offset + lenTmp)[0]);
				lenTmp += 1;
				lenTmp += GetWstringByWChars(pItemFrameData + offset + lenTmp, frameSize - offset - lenTmp, picItem.Description);
				//picItem.PicData.SetPicBufData((const char*)(pItemFrameData + offset + lenTmp), valSize - lenTmp);
				m_WMAPicArray.push_back(picItem);
			}
			else
			{
				// ��չFLAG��ʶ���ַ������ͻ���ֵ����  
				// ��ʾֵ����  
				if (exFlagVal == 3)
				{
					attrInfo.AttrStyle = WMA_ATTRSTYLE_VAL;
					if (valSize >= 4)
					{
						attrInfo.ValContent = CalUInt32Val(pItemFrameData + offset, 4);
					}
					WMAAttrArray.push_back(attrInfo);
				}
				else
				{
					attrInfo.AttrStyle = WMA_ATTRSTYLE_TXT;
					attrInfo.StrContent = GetWString(pItemFrameData + offset, valSize);
					WMAAttrArray.push_back(attrInfo);
				}
			}
		}

		// �ı��ַƫ����  
		offset += valSize;
	}

	// ���سɹ�  
	return true;
}

// ȡ���ַ�������0x00,0x00  
unsigned long CMyWMATag::GetWstringByWChars(const unsigned char* pPicDataBuf, unsigned long bufSize, wstring &strVal)
{
	// ��������  
	unsigned long lResult = 0;

	// ������Ч��  
	if (pPicDataBuf == NULL || bufSize == 0)
	{
		return lResult;
	}

	// ����  
	const wchar_t *pWBuf = (const wchar_t*)pPicDataBuf;
	unsigned long wBufSize = bufSize / 2;
	unsigned long wPos = 0;
	while (wPos < wBufSize)
	{
		if (pWBuf[wPos] == 0x00)
		{
			lResult = 2 * (wPos + 1);
			break;
		}
		else
		{
			wPos++;
		}
	}

	// �õ��ַ���  
	strVal = L"";
	strVal.append(pWBuf, wPos + 1);

	// ����  
	return lResult;
}

// ȡ����չ֡����ID  
WMA_FRAMEID_T CMyWMATag::GetExFrameID(const wstring &exName)
{
	// ���ڷ���ֵ  
	WMA_FRAMEID_T rtExFrameID = WMA_FRAMEID_UNKNOWN;

	// ���ʶ�Ƚ�  
	if (wcscmp(exName.c_str(), L"WM/AlbumArtist") == 0)
	{
		rtExFrameID = WMA_FRAMEID_ALBUMARTIST;
	}
	else if (wcscmp(exName.c_str(), L"WM/AlbumCoverURL") == 0)
	{
		rtExFrameID = WMA_FRAMEID_ALBUMCOVERURL;
	}
	else if (wcscmp(exName.c_str(), L"WM/AlbumTitle") == 0)
	{
		rtExFrameID = WMA_FRAMEID_ALBUMTITLE;
	}
	else if (wcscmp(exName.c_str(), L"WM/AudioFileURL") == 0)
	{
		rtExFrameID = WMA_FRAMEID_AUDIOFILEURL;
	}
	else if (wcscmp(exName.c_str(), L"WM/AudioSourceURL") == 0)
	{
		rtExFrameID = WMA_FRAMEID_AUDIOSOURCEURL;
	}
	else if (wcscmp(exName.c_str(), L"WM/AuthorURL") == 0)
	{
		rtExFrameID = WMA_FRAMEID_AUTHORURL;
	}
	else if (wcscmp(exName.c_str(), L"WM/BeatsPerMinute") == 0)
	{
		rtExFrameID = WMA_FRAMEID_BPMT;
	}
	else if (wcscmp(exName.c_str(), L"WM/Category") == 0)
	{
		rtExFrameID = WMA_FRAMEID_CATEGORY;
	}
	else if (wcscmp(exName.c_str(), L"WM/Composer") == 0)
	{
		rtExFrameID = WMA_FRAMEID_COMPOSER;
	}
	else if (wcscmp(exName.c_str(), L"WM/Conductor") == 0)
	{
		rtExFrameID = WMA_FRAMEID_CONDUCTOR;
	}
	else if (wcscmp(exName.c_str(), L"WM/ContentDistributor") == 0)
	{
		rtExFrameID = WMA_FRAMEID_CONTENTDISTRIBUTOR;
	}
	else if (wcscmp(exName.c_str(), L"WM/ContentGroupDescription") == 0)
	{
		rtExFrameID = WMA_FRAMEID_CGDPN;
	}
	else if (wcscmp(exName.c_str(), L"WM/Director") == 0)
	{
		rtExFrameID = WMA_FRAMEID_DIRECTOR;
	}
	else if (wcscmp(exName.c_str(), L"WM/DVDID") == 0)
	{
		rtExFrameID = WMA_FRAMEID_DVDID;
	}
	else if (wcscmp(exName.c_str(), L"WM/EncodedBy") == 0)
	{
		rtExFrameID = WMA_FRAMEID_ENCODEDBY;
	}
	else if (wcscmp(exName.c_str(), L"WM/EncodingSettings") == 0)
	{
		rtExFrameID = WMA_FRAMEID_ENCODINGSETTINGS;
	}
	else if (wcscmp(exName.c_str(), L"WM/EncodingTime") == 0)
	{
		rtExFrameID = WMA_FRAMEID_ENCODINGTIME;
	}
	else if (wcscmp(exName.c_str(), L"WM/Genre") == 0)
	{
		rtExFrameID = WMA_FRAMEID_GENRE;
	}
	else if (wcscmp(exName.c_str(), L"WM/GenreID") == 0)
	{
		rtExFrameID = WMA_FRAMEID_GENREID;
	}
	else if (wcscmp(exName.c_str(), L"WM/InitialKey") == 0)
	{
		rtExFrameID = WMA_FRAMEID_INITIALKEY;
	}
	else if (wcscmp(exName.c_str(), L"WM/ISRC") == 0)
	{
		rtExFrameID = WMA_FRAMEID_ISRC;
	}
	else if (wcscmp(exName.c_str(), L"WM/Language") == 0)
	{
		rtExFrameID = WMA_FRAMEID_LANGUAGE;
	}
	else if (wcscmp(exName.c_str(), L"WM/Lyrics") == 0)
	{
		rtExFrameID = WMA_FRAMEID_LYRICS;
	}
	else if (wcscmp(exName.c_str(), L"WM/Lyrics_Synchronised") == 0)
	{
		rtExFrameID = WMA_FRAMEID_LYRICSSYNC;
	}
	else if (wcscmp(exName.c_str(), L"WM/MCDI") == 0)
	{
		rtExFrameID = WMA_FRAMEID_MCDI;
	}
	else if (wcscmp(exName.c_str(), L"WM/MediaClassPrimaryID") == 0)
	{
		rtExFrameID = WMA_FRAMEID_MCPID;
	}
	else if (wcscmp(exName.c_str(), L"WM/MediaClassSecondaryID") == 0)
	{
		rtExFrameID = WMA_FRAMEID_MCSID;
	}
	else if (wcscmp(exName.c_str(), L"WM/MediaCredits") == 0)
	{
		rtExFrameID = WMA_FRAMEID_MDACREDITS;
	}
	else if (wcscmp(exName.c_str(), L"WM/MediaIsDelay") == 0)
	{
		rtExFrameID = WMA_FRAMEID_MDAISDELAY;
	}
	else if (wcscmp(exName.c_str(), L"WM/MediaIsFinale") == 0)
	{
		rtExFrameID = WMA_FRAMEID_MDAISFINALE;
	}
	else if (wcscmp(exName.c_str(), L"WM/MediaIsLive") == 0)
	{
		rtExFrameID = WMA_FRAMEID_MDAISLIVE;
	}
	else if (wcscmp(exName.c_str(), L"WM/MediaIsPremiere") == 0)
	{
		rtExFrameID = WMA_FRAMEID_MDAISPREMIERE;
	}
	else if (wcscmp(exName.c_str(), L"WM/MediaIsRepeat") == 0)
	{
		rtExFrameID = WMA_FRAMEID_MDAISREPEAT;
	}
	else if (wcscmp(exName.c_str(), L"WM/MediaIsSAP") == 0)
	{
		rtExFrameID = WMA_FRAMEID_MDAISSAP;
	}
	else if (wcscmp(exName.c_str(), L"WM/MediaIsSubtitled") == 0)
	{
		rtExFrameID = WMA_FRAMEID_MDAISSUBTITLED;
	}
	else if (wcscmp(exName.c_str(), L"WM/MediaIsStereo") == 0)
	{
		rtExFrameID = WMA_FRAMEID_MDAISSTEREO;
	}
	else if (wcscmp(exName.c_str(), L"WM/MediaNetworkAffiliation") == 0)
	{
		rtExFrameID = WMA_FRAMEID_MDANETWORKAFFILIATION;
	}
	else if (wcscmp(exName.c_str(), L"WM/MediaOriginalBroadcastDateTime") == 0)
	{
		rtExFrameID = WMA_FRAMEID_MDAORIGINALBDT;
	}
	else if (wcscmp(exName.c_str(), L"WM/MediaOriginalChannel") == 0)
	{
		rtExFrameID = WMA_FRAMEID_MDAORIGINALCHANNEL;
	}
	else if (wcscmp(exName.c_str(), L"WM/MediaStationCallSign") == 0)
	{
		rtExFrameID = WMA_FRAMEID_MDASTATIONCALLSIGN;
	}
	else if (wcscmp(exName.c_str(), L"WM/MediaStationName") == 0)
	{
		rtExFrameID = WMA_FRAMEID_MDASTATIONNAME;
	}
	else if (wcscmp(exName.c_str(), L"WM/ModifiedBy") == 0)
	{
		rtExFrameID = WMA_FRAMEID_MODIFIEDBY;
	}
	else if (wcscmp(exName.c_str(), L"WM/Mood") == 0)
	{
		rtExFrameID = WMA_FRAMEID_MOOD;
	}
	else if (wcscmp(exName.c_str(), L"WM/OriginalAlbumTitle") == 0)
	{
		rtExFrameID = WMA_FRAMEID_ORIGALBUMTITLE;
	}
	else if (wcscmp(exName.c_str(), L"WM/OriginalArtist") == 0)
	{
		rtExFrameID = WMA_FRAMEID_ORIGARTIST;
	}
	else if (wcscmp(exName.c_str(), L"WM/OriginalFilename") == 0)
	{
		rtExFrameID = WMA_FRAMEID_ORIGFILENAME;
	}
	else if (wcscmp(exName.c_str(), L"WM/OriginalLyricist") == 0)
	{
		rtExFrameID = WMA_FRAMEID_ORIGLYRICIST;
	}
	else if (wcscmp(exName.c_str(), L"WM/OriginalReleaseTime") == 0)
	{
		rtExFrameID = WMA_FRAMEID_ORIGRELEASETIME;
	}
	else if (wcscmp(exName.c_str(), L"WM/OriginalReleaseYear") == 0)
	{
		rtExFrameID = WMA_FRAMEID_ORIGRELEASEYEAR;
	}
	else if (wcscmp(exName.c_str(), L"WM/ParentalRating") == 0)
	{
		rtExFrameID = WMA_FRAMEID_PARENTALRATING;
	}
	else if (wcscmp(exName.c_str(), L"WM/ParentalRatingReason") == 0)
	{
		rtExFrameID = WMA_FRAMEID_PARENTALRATINGREASON;
	}
	else if (wcscmp(exName.c_str(), L"WM/PartOfSet") == 0)
	{
		rtExFrameID = WMA_FRAMEID_PARTOFSET;
	}
	else if (wcscmp(exName.c_str(), L"WM/Period") == 0)
	{
		rtExFrameID = WMA_FRAMEID_PERIOD;
	}
	else if (wcscmp(exName.c_str(), L"WM/Picture") == 0)
	{
		rtExFrameID = WMA_FRAMEID_PICTURE;
	}
	else if (wcscmp(exName.c_str(), L"WM/Producer") == 0)
	{
		rtExFrameID = WMA_FRAMEID_PRODUCER;
	}
	else if (wcscmp(exName.c_str(), L"WM/PromotionURL") == 0)
	{
		rtExFrameID = WMA_FRAMEID_PROMOTIONURL;
	}
	else if (wcscmp(exName.c_str(), L"WM/Provider") == 0)
	{
		rtExFrameID = WMA_FRAMEID_PROVIDER;
	}
	else if (wcscmp(exName.c_str(), L"WM/ProviderCopyright") == 0)
	{
		rtExFrameID = WMA_FRAMEID_PROVIDERCPYRIGHT;
	}
	else if (wcscmp(exName.c_str(), L"WM/ProviderRating") == 0)
	{
		rtExFrameID = WMA_FRAMEID_PROVIDERRATING;
	}
	else if (wcscmp(exName.c_str(), L"WM/ProviderStyle") == 0)
	{
		rtExFrameID = WMA_FRAMEID_PROVIDERSTYLE;
	}
	else if (wcscmp(exName.c_str(), L"WM/Publisher") == 0)
	{
		rtExFrameID = WMA_FRAMEID_PUBLISHER;
	}
	else if (wcscmp(exName.c_str(), L"WM/RadioStationName") == 0)
	{
		rtExFrameID = WMA_FRAMEID_RADIOSTATIONNAME;
	}
	else if (wcscmp(exName.c_str(), L"WM/RadioStationOwner") == 0)
	{
		rtExFrameID = WMA_FRAMEID_RADIOSTATIONOWNER;
	}
	else if (wcscmp(exName.c_str(), L"WM/SharedUserRating") == 0)
	{
		rtExFrameID = WMA_FRAMEID_SHAREUSERRATING;
	}
	else if (wcscmp(exName.c_str(), L"WM/SubTitle") == 0)
	{
		rtExFrameID = WMA_FRAMEID_SUBTITLE;
	}
	else if (wcscmp(exName.c_str(), L"WM/SubTitleDescription") == 0)
	{
		rtExFrameID = WMA_FRAMEID_SUBTITLEDESCRIPTION;
	}
	else if (wcscmp(exName.c_str(), L"WM/Text") == 0)
	{
		rtExFrameID = WMA_FRAMEID_TEXT;
	}
	else if (wcscmp(exName.c_str(), L"WM/ToolName") == 0)
	{
		rtExFrameID = WMA_FRAMEID_TOOLNAME;
	}
	else if (wcscmp(exName.c_str(), L"WM/ToolVersion") == 0)
	{
		rtExFrameID = WMA_FRAMEID_TOOLVERSION;
	}
	else if (wcscmp(exName.c_str(), L"WM/Track") == 0)
	{
		rtExFrameID = WMA_FRAMEID_TRACK;
	}
	else if (wcscmp(exName.c_str(), L"WM/TrackNumber") == 0)
	{
		rtExFrameID = WMA_FRAMEID_TRACKNUMBER;
	}
	else if (wcscmp(exName.c_str(), L"WM/UniqueFileIdentifier") == 0)
	{
		rtExFrameID = WMA_FRAMEID_UNIQUEFILEIDENTIFIER;
	}
	else if (wcscmp(exName.c_str(), L"WM/UserWebURL") == 0)
	{
		rtExFrameID = WMA_FRAMEID_USERWEBURL;
	}
	else if (wcscmp(exName.c_str(), L"WM/WMCollectionGroupID") == 0)
	{
		rtExFrameID = WMA_FRAMEID_WMCOLLECTIONGROUPID;
	}
	else if (wcscmp(exName.c_str(), L"WM/WMCollectionID") == 0)
	{
		rtExFrameID = WMA_FRAMEID_WMCOLLECTIONID;
	}
	else if (wcscmp(exName.c_str(), L"WM/WMContentID") == 0)
	{
		rtExFrameID = WMA_FRAMEID_CONTENTID;
	}
	else if (wcscmp(exName.c_str(), L"WM/Writer") == 0)
	{
		rtExFrameID = WMA_FRAMEID_WRITER;
	}
	else if (wcscmp(exName.c_str(), L"WM/Year") == 0)
	{
		rtExFrameID = WMA_FRAMEID_YEAR;
	}
	else if (wcscmp(exName.c_str(), L"WMFSDKVersion") == 0)
	{
		rtExFrameID = WMA_FRAMEID_SDKVERSION;
	}
	else if (wcscmp(exName.c_str(), L"WMFSDKNeeded") == 0)
	{
		rtExFrameID = WMA_FRAMEID_SDKNEEDED;
	}
	else if (wcscmp(exName.c_str(), L"IsVBR") == 0)
	{
		rtExFrameID = WMA_FRAMEID_ISVBR;
	}
	else
	{
		rtExFrameID = WMA_FRAMEID_UNKNOWN;
	}

	return rtExFrameID;
}
