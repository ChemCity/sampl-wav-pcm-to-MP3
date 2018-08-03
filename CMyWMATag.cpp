//////////////////////////////////////////////////////////////////////////  
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif // !_CRT_SECURE_NO_WARNINGS
#include <Windows.h>  
#include <string.h>
#include "CMyWMATag.h" 

//////////////////////////////////////////////////////////////////////////  
// 构造函数  
CMyWMATag::CMyWMATag()
{
	// 初始化成员变量  
	ClearDataInfo();

}

// 析构函数  
CMyWMATag::~CMyWMATag()
{

}

//////////////////////////////////////////////////////////////////////////  
// 接口函数  
// 解析WMA文件，成功后调用下面接口获取信息  
bool CMyWMATag::AnalyzeWMAFile(const char *filePathName)
{
	// 清除上次信息  
	ClearDataInfo();

	// 参数有效性  
	if (filePathName == NULL)
	{
		return false;
	}

	// 打开文件  
	FILE *pWMAFile = fopen(filePathName, "rb");
	if (pWMAFile == NULL)
	{
		return false;
	}

	// 读取WMA头结构  
	memset(&m_HeadInfo, 0, sizeof(m_HeadInfo));
	if (fread(&m_HeadInfo, sizeof(m_HeadInfo), 1, pWMAFile) != 1)
	{
		fclose(pWMAFile);
		return false;
	}

	// 判断是否WMA文件  
	m_bAnalyzeSuccess = BeWMAFile(m_HeadInfo.HeadFlag);
	if (!m_bAnalyzeSuccess)
	{
		memset(&m_HeadInfo, 0, sizeof(m_HeadInfo));
		fclose(pWMAFile);
		return false;
	}

	// 计算要读取帧数据的字节数  
	unsigned long lFrameDataSize = GetHeadSize(m_HeadInfo.HeadSize) - sizeof(WMAHEADINFO);

	// 为帧数据分配空间  
	unsigned char *pFrameData = new unsigned char[lFrameDataSize];
	if (pFrameData == NULL)
	{
		fclose(pWMAFile);
		m_bAnalyzeSuccess = false;
		return false;
	}

	// 读取帧数据  
	memset(pFrameData, 0, lFrameDataSize);
	if (fread(pFrameData, lFrameDataSize, 1, pWMAFile) != 1)
	{
		// 释放帧数据  
		if (pFrameData != NULL)
		{
			delete[] pFrameData;
			pFrameData = NULL;
		}

		fclose(pWMAFile);
		m_bAnalyzeSuccess = false;
		return false;
	}

	// 读取成功关闭文件  
	fclose(pWMAFile);

	// 解析帧数据  
	ParseFrameData(pFrameData, lFrameDataSize);

	// 释放帧数据  
	if (pFrameData != NULL)
	{
		delete[] pFrameData;
		pFrameData = NULL;
	}

	// 返回分析结果  
	return m_bAnalyzeSuccess;
}

// 解析WMA文件  
bool CMyWMATag::AnalyzeWMAFile(const wchar_t *filePathName)
{
	// 参数有效性  
	if (filePathName == NULL)
	{
		return false;
	}

	// 计算转换为多字节字节数  
	int nChars = WideCharToMultiByte(CP_ACP, 0, filePathName, -1, NULL, 0, NULL, NULL);

	// 申请空间  
	char *pFileName = new char[nChars + 1];
	if (pFileName == NULL)
	{
		return false;
	}

	// 转换为多字节  
	memset(pFileName, 0, nChars + 1);
	WideCharToMultiByte(CP_ACP, 0, filePathName, -1, pFileName, nChars, NULL, NULL);

	// 解析  
	bool bResult = AnalyzeWMAFile(pFileName);

	// 释放  
	if (pFileName != NULL)
	{
		delete[] pFileName;
		pFileName = NULL;
	}

	// 返回结果  
	return bResult;
}

// 解析WMA文件  
bool CMyWMATag::AnalyzeWMAFile(const wstring &filePathName)
{
	return AnalyzeWMAFile(filePathName.c_str());
}

// 是否解析成功  
bool CMyWMATag::BeSuccess()
{
	return m_bAnalyzeSuccess;
}

// 取得头大小  
unsigned long CMyWMATag::GetHeadSize()
{
	return GetHeadSize(m_HeadInfo.HeadSize);
}

//////////////////////////////////////////////////////////////////////////  
// 取得Txt属性内容接口  
bool CMyWMATag::GetTxtItemContent(WMA_FRAMEID_T frameID, wstring &itemContent)
{
	// 解析失败  
	if (!BeSuccess())
	{
		return false;
	}

	// 查找  
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

	// 没有找到  
	return false;
}

// 取得Txt属性内容接口  
bool CMyWMATag::GetTxtItemContent(WMA_FRAMEID_T frameID, string &itemContent)
{
	wstring wContent;
	bool bOK = GetTxtItemContent(frameID, wContent);
	itemContent = TransFromWString(wContent);
	return bOK;
}

//////////////////////////////////////////////////////////////////////////  
// 取得图片属性内容接口  
// 取得第一张图片  
//bool CMyWMATag::GetFirstPic(CMYPICINFO &picInfo)
//{
//	// 解析失败  
//	if (!BeSuccess())
//	{
//		return false;
//	}
//
//	// 查找是否有图片  
//	if (m_WMAPicArray.size() == 0)
//	{
//		return false;
//	}
//
//	// 得到第一张图片  
//	picInfo = m_WMAPicArray[0].PicData;
//	return true;
//}

// 保存第一张图片  
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

// 取得第一张图片的地址及大小  
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

// 取得第一行图片的大小  
unsigned long CMyWMATag::GetFirstPicSize()
{
	//if (BeSuccess() && m_WMAPicArray.size()>0)
	//{
	//	return m_WMAPicArray[0].PicData.GetPicBufSize();
	//}

	return 0;
}

// 取得图片数量  
unsigned long CMyWMATag::GetPicCounts()
{
	return (unsigned long)m_WMAPicArray.size();
}

// 取得第n张图片  
//bool CMyWMATag::GetPic(unsigned long picIndex, CMYPICINFO &picInfo)
//{
//	// 解析失败  
//	if (!BeSuccess())
//	{
//		return false;
//	}
//
//	// 得到图片数量  
//	unsigned long picCounts = GetPicCounts();
//
//	// 查找是否有图片  
//	if (picCounts == 0)
//	{
//		return false;
//	}
//
//	// 索引是否有效  
//	if (picIndex >= picCounts)
//	{
//		return false;
//	}
//
//	// 得到第picIndex张图片  
//	picInfo = m_WMAPicArray[picIndex].PicData;
//	return true;
//}

// 取得第n张图片的地址  
const unsigned char* CMyWMATag::GetPicAddr(unsigned long picIndex, unsigned long *pPicSize)
{
	// 解析失败  
	if (!BeSuccess())
	{
		return NULL;
	}

	// 得到图片数量  
	unsigned long picCounts = GetPicCounts();

	// 查找是否有图片  
	if (picCounts == 0)
	{
		return NULL;
	}

	// 索引是否有效  
	if (picIndex >= picCounts)
	{
		return NULL;
	}
	return NULL;
	// 取得图片大小  
	//if (pPicSize != NULL)
	//{
	//	*pPicSize = m_WMAPicArray[picIndex].PicData.GetPicBufSize();
	//}

	//// 得到第picIndex张图片地址  
	//return m_WMAPicArray[picIndex].PicData.GetPicBufAddr();
}

// 保存第n张图片  
bool CMyWMATag::SavePicToFile(unsigned long picIndex, const char *filePathName)
{
	// 参数有效性  
	if (filePathName == NULL)
	{
		return false;
	}

	// 解析失败  
	if (!BeSuccess())
	{
		return false;
	}

	// 得到图片数量  
	unsigned long picCounts = GetPicCounts();

	// 查找是否有图片  
	if (picCounts == 0)
	{
		return false;
	}

	// 索引是否有效  
	if (picIndex >= picCounts)
	{
		return false;
	}
	return false;
	//return m_WMAPicArray[picIndex].PicData.SavePicToFile(filePathName);
}

// 保存第n张图片  
bool CMyWMATag::SavePicToFile(unsigned long picIndex, const wchar_t *filePathName)
{
	// 参数有效性  
	if (filePathName == NULL)
	{
		return false;
	}

	// 解析失败  
	if (!BeSuccess())
	{
		return false;
	}

	// 得到图片数量  
	unsigned long picCounts = GetPicCounts();

	// 查找是否有图片  
	if (picCounts == 0)
	{
		return false;
	}

	// 索引是否有效  
	if (picIndex >= picCounts)
	{
		return false;
	}
	return false;
	//return m_WMAPicArray[picIndex].PicData.SavePicToFile(filePathName);
}
//////////////////////////////////////////////////////////////////////////  
// 私有函数  
// 根据文件头结构判断是否WMA文件  
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

// 清除信息  
void CMyWMATag::ClearDataInfo()
{
	memset(&m_HeadInfo, 0, sizeof(m_HeadInfo));
	m_bAnalyzeSuccess = false;
	m_WMAAttrArray.clear();
	m_WMAPicArray.clear();
}

// 计算头大小  
unsigned long CMyWMATag::GetHeadSize(const WMAHEADSIZE &headSize)
{
	return (unsigned long)((headSize.Size[3] << 24) | (headSize.Size[2] << 16) | (headSize.Size[1] << 8) | headSize.Size[0]);
}

// 得到帧类型  
WMA_FRAMESTYLE_T CMyWMATag::GetFrameStyle(const WMAFRAMEHEADFLAG &frameFlag)
{
	// 判断是否标准帧  
	if (BeSameFrameFlag(frameFlag, WMA_FRAMEHEAD_STANDARD))
	{
		return WMA_FRAMESTYLE_STANDARD;
	}

	// 判断是否扩展帧  
	if (BeSameFrameFlag(frameFlag, WMA_FRAMEHEAD_EXTEND))
	{
		return WMA_FRAMESTYLE_EXTEND;
	}

	// 返回位置帧  
	return WMA_FRAMESTYLE_UNKNOWN;
}

// 判断两个帧头标识是否相等  
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

// 得到帧的大小  
unsigned long CMyWMATag::GetFrameSize(const WMAFRAMEHEADSIZE &frameSize)
{
	return (unsigned long)((frameSize.Size[3] << 24) | (frameSize.Size[2] << 16) | (frameSize.Size[1] << 8) | frameSize.Size[0]);
}

// 计算由2个字节拼接成的整数大小  
unsigned long CMyWMATag::CalUInt16Val(const unsigned char *pByteBuf, int bufSize)
{
	// 参数有效性  
	if (pByteBuf == NULL || bufSize<2)
	{
		return 0;
	}

	// 返回值  
	return ((pByteBuf[1] << 8) | pByteBuf[0]);
}

// 计算由4个字节拼接成的整数大小  
unsigned long CMyWMATag::CalUInt32Val(const unsigned char *pByteBuf, int bufSize)
{
	// 参数有效性  
	if (pByteBuf == NULL || bufSize<4)
	{
		return 0;
	}

	// 返回值  
	return ((pByteBuf[3] << 24) | (pByteBuf[2] << 16) | (pByteBuf[1] << 8) | pByteBuf[0]);
}

// 解析得到的帧数据  
unsigned long CMyWMATag::ParseFrameData(const unsigned char *pFrameData, unsigned long bufLength)
{
	// 清除上次数据  
	m_WMAAttrArray.clear();
	m_WMAPicArray.clear();

	// 参数有效性  
	if (pFrameData == NULL || bufLength == 0)
	{
		return 0;
	}

	// 存放临时变量  
	WMAFRAMEHEADINFO frameHead;
	WMA_FRAMESTYLE_T frameStyle = WMA_FRAMESTYLE_UNKNOWN;
	unsigned long frameSize = 0;
	WMAATTRINFO attrItemInfo;


	// 遍历解析内容  
	unsigned long pos = 0;
	while (pos + 24 < bufLength)
	{
		// 得到帧头失败  
		if (!GetFrameHeadInfo(pFrameData + pos, bufLength - pos, frameHead))
		{
			break;
		}

		// 得到帧风格类型  
		frameStyle = GetFrameStyle(frameHead.FrameFlag);

		// 是正确风格，取得帧大小  
		frameSize = GetFrameSize(frameHead.FrameSize);

		// 取得帧体内容     
		if (frameSize >= 24 && pos + 23 + frameSize<bufLength)
		{
			// 是标准帧，解析标准帧  
			if (frameStyle == WMA_FRAMESTYLE_STANDARD)
			{
				ParseStandardFrame(pFrameData + pos + 24, frameSize - 24, m_WMAAttrArray);
			}
			// 是扩展帧  
			else if (frameStyle == WMA_FRAMESTYLE_EXTEND)
			{
				ParseExtendFrame(pFrameData + pos + 24, frameSize - 24, m_WMAAttrArray, m_WMAPicArray);
			}

			// 改变索引  
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

	// 返回解析的属性数量  
	return (unsigned long)m_WMAAttrArray.size() + (unsigned long)m_WMAPicArray.size();
}

// 得到帧头信息  
bool CMyWMATag::GetFrameHeadInfo(const unsigned char *pFrameData, unsigned long bufLength, WMAFRAMEHEADINFO &frameHead)
{
	// 参数有效性  
	if (pFrameData == NULL || bufLength<24)
	{
		return false;
	}

	// 得到帧标识  
	int i = 0;
	for (i = 0; i<16; i++)
	{
		frameHead.FrameFlag.Flag[i] = pFrameData[i];
	}

	// 得到帧大小  
	for (i = 0; i<8; i++)
	{
		frameHead.FrameSize.Size[i] = pFrameData[i + 16];
	}

	return true;
}

// 解析标准帧数据  
bool CMyWMATag::ParseStandardFrame(const unsigned char *pItemFrameData, unsigned long frameSize, CWMAAttrArray &WMAAttrArray)
{
	// 参数有效性  
	if (pItemFrameData == NULL || frameSize == 0)
	{
		return false;
	}

	// 计算标题信息大小  
	unsigned long titleSize = CalUInt16Val(pItemFrameData, 2);

	// 计算艺术家信息大小  
	unsigned long artistSize = CalUInt16Val(pItemFrameData + 2, 2);

	// 计算版权信息大小  
	unsigned long copyRightSize = CalUInt16Val(pItemFrameData + 4, 2);

	// 计算备注信息大小  
	unsigned long commentSize = CalUInt16Val(pItemFrameData + 6, 2);

	// 计算未知信息大小  
	unsigned long unknownSize = CalUInt16Val(pItemFrameData + 8, 2);

	// 偏移量  
	unsigned long offset = 10;

	// 临时存放信息  
	WMAATTRINFO attrInfo;
	memset(&attrInfo, 0, sizeof(attrInfo));
	attrInfo.AttrStyle = WMA_ATTRSTYLE_TXT;

	// 取得标题信息  
	attrInfo.ID = WMA_FRAMEID_TITLE;
	attrInfo.StrContent = GetWString(pItemFrameData + offset, titleSize);
	WMAAttrArray.push_back(attrInfo);
	offset += titleSize;

	// 取得艺术家信息  
	attrInfo.ID = WMA_FRAMEID_AUTHOR;
	attrInfo.StrContent = GetWString(pItemFrameData + offset, artistSize);
	WMAAttrArray.push_back(attrInfo);
	offset += artistSize;

	// 取得版权信息  
	attrInfo.ID = WMA_FRAMEID_COPYRIGHT;
	attrInfo.StrContent = GetWString(pItemFrameData + offset, copyRightSize);
	WMAAttrArray.push_back(attrInfo);
	offset += copyRightSize;

	// 取得备注信息  
	attrInfo.ID = WMA_FRAMEID_DESCRIPTION;
	attrInfo.StrContent = GetWString(pItemFrameData + offset, commentSize);
	WMAAttrArray.push_back(attrInfo);
	offset += commentSize;

	// 取得未知信息  
	attrInfo.ID = WMA_FRAMEID_RATING;
	attrInfo.StrContent = GetWString(pItemFrameData + offset, unknownSize);
	WMAAttrArray.push_back(attrInfo);
	offset += unknownSize;

	// 返回成功  
	return true;
}

// 得到string  
wstring CMyWMATag::GetWString(const unsigned char* pUCharBuf, unsigned long charsNums)
{
	// 返回值  
	wstring strRet = L"";

	// 参数有效性  
	if (pUCharBuf == NULL || charsNums == 0)
	{
		return strRet;
	}

	// 得到字符串  
	strRet.append((const wchar_t*)pUCharBuf, charsNums / 2);
	return strRet;
}

// wstring转换为string  
string CMyWMATag::TransFromWString(const wstring &wstrVal)
{
	// 计算字符数  
	int nCharNums = (int)wstrVal.length();
	int mulBufSize = 2 * nCharNums + 10;

	// 申请空间  
	char *pCharBuf = new char[mulBufSize];
	if (pCharBuf == NULL)
	{
		return "";
	}
	memset(pCharBuf, 0, mulBufSize);

	// 转换  
	WideCharToMultiByte(CP_ACP, 0, wstrVal.c_str(), -1, pCharBuf, mulBufSize, NULL, NULL);

	// 得到结果  
	string strVal(pCharBuf);

	// 释放空间  
	if (pCharBuf != NULL)
	{
		delete[] pCharBuf;
		pCharBuf = NULL;
	}

	return strVal;
}

// 解析扩展帧数据  
bool CMyWMATag::ParseExtendFrame(const unsigned char *pItemFrameData, unsigned long frameSize, CWMAAttrArray &WMAAttrArray, CWMAPicArray &WMAPicArray)
{
	// 参数有效性  
	if (pItemFrameData == NULL || frameSize == 0)
	{
		return false;
	}

	// 计算扩展信息数量  
	unsigned long exInfoCounts = CalUInt16Val(pItemFrameData, 2);

	// 地址偏移量  
	unsigned long offset = 2;

	// 临时存放扩展信息  
	unsigned long exNameSize = 0;
	wstring exName = L"";
	unsigned long exFlagVal = 0;
	unsigned long valSize = 0;
	WMA_FRAMEID_T frameID = WMA_FRAMEID_UNKNOWN;
	WMAATTRINFO attrInfo;
	WMAPICITEMINFO picItem;
	unsigned lenTmp = 0;

	// 取扩展信息  
	for (unsigned exIndex = 0; exIndex<exInfoCounts; exIndex++)
	{
		// 清除上次内容  
		memset(&attrInfo, 0, sizeof(attrInfo));

		// 计算扩展名大小  
		exNameSize = CalUInt16Val(pItemFrameData + offset, 2);
		offset += 2;

		// 取得扩展名称  
		exName = GetWString(pItemFrameData + offset, exNameSize);
		offset += exNameSize;

		// 取得扩展标识FLAG值  
		exFlagVal = CalUInt16Val(pItemFrameData + offset, 2);
		offset += 2;

		// 取得实际值的大小  
		valSize = CalUInt16Val(pItemFrameData + offset, 2);
		offset += 2;

		// 计算扩展属性ID  
		frameID = GetExFrameID(exName);

		// 是否有效ID  
		if (frameID != WMA_FRAMEID_UNKNOWN)
		{
			// 得到属性ID  
			attrInfo.ID = frameID;

			// 图片得到图片信息  
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
				// 扩展FLAG标识是字符串类型还是值类型  
				// 表示值类型  
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

		// 改变地址偏移量  
		offset += valSize;
	}

	// 返回成功  
	return true;
}

// 取得字符串根据0x00,0x00  
unsigned long CMyWMATag::GetWstringByWChars(const unsigned char* pPicDataBuf, unsigned long bufSize, wstring &strVal)
{
	// 返回数量  
	unsigned long lResult = 0;

	// 参数有效性  
	if (pPicDataBuf == NULL || bufSize == 0)
	{
		return lResult;
	}

	// 遍历  
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

	// 得到字符串  
	strVal = L"";
	strVal.append(pWBuf, wPos + 1);

	// 返回  
	return lResult;
}

// 取得扩展帧属性ID  
WMA_FRAMEID_T CMyWMATag::GetExFrameID(const wstring &exName)
{
	// 用于返回值  
	WMA_FRAMEID_T rtExFrameID = WMA_FRAMEID_UNKNOWN;

	// 与标识比较  
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
