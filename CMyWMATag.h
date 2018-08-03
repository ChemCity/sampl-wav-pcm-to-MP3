#ifndef _CMY_WMA_TAG_H_  
#define _CMY_WMA_TAG_H_  
//////////////////////////////////////////////////////////////////////////  
//#include "CMyPicInfo.h"  
#include <string>  
#include <vector>  
using namespace std;
//////////////////////////////////////////////////////////////////////////  
/*************************************************************************
// ��Ϊ�ļ�ͷ�͸���֡����
+--------------------------------------------------------------+
|      Header (30 bytes)   HeadFlag:16; HeadSize:8; Unknow:6   |
+--------------------------------------------------------------+
|      Frames (1....n)                                         |
+--------------------------------------------------------------+
// ���е�TAG��Ϣ����ڱ�׼֡����չ֡�У�����֡���Բ��迼��
// ���е���Ϣ����UNICODE����
// ��׼֡�ṹ
+--------------------------------------------------------------+
|      Header (24 bytes)   HeadFlag:16; HeadSize:8;            |
+--------------------------------------------------------------+
|      ������Ϣ��С��2 bytes��                                  |
+--------------------------------------------------------------+
|      ��������Ϣ��С��2 bytes��                                |
+--------------------------------------------------------------+
|      ��Ȩ��Ϣ��С��2 bytes��                                  |
+--------------------------------------------------------------+
|      ��ע��Ϣ��С��2 bytes��                                  |
+--------------------------------------------------------------+
|      δ֪��Ϣ��С��2 bytes��                                  |
+--------------------------------------------------------------+
|      ������Ϣ����(0x00 0x00����)                              |
+--------------------------------------------------------------+
|      ��������Ϣ����(0x00 0x00����)                            |
+--------------------------------------------------------------+
|      ��Ȩ��Ϣ����(0x00 0x00����)                              |
+--------------------------------------------------------------+
|      ��ע��Ϣ����(0x00 0x00����)                              |
+--------------------------------------------------------------+
|      δ֪��Ϣ����(0x00 0x00����)                              |
+--------------------------------------------------------------+
// ��չ֡�ṹ
+--------------------------------------------------------------+
|      Header (24 bytes)   HeadFlag:16; HeadSize:8;            |
+--------------------------------------------------------------+
|      ��չ��Ϣ����EXNO��2 bytes��                              |
+--------------------------------------------------------------+
|      EXINFO (1....EXNO)                                      |
+--------------------------------------------------------------+
// ÿ����չ��ϢEXINFO�ṹ
+--------------------------------------------------------------+
|      EXINFO NAME Size (2 bytes)   ��չ��Ϣ���ִ�С            |
+--------------------------------------------------------------+
|      ��չ��Ϣ����                                             |
+--------------------------------------------------------------+
|      ��־FLAG   (2 bytes)                                    |
+--------------------------------------------------------------+
|      ֵ�Ĵ�С   (2 bytes)                                     |
+--------------------------------------------------------------+
|      ʵ�ʵ�ֵ   (ͼƬ��ʽ�ο�ID3V2.3)                         |
+--------------------------------------------------------------+
����չ��Ϣ����ΪWMFSDKVersionʱ�����ֵ��ʾ�������WMA�ļ��İ汾��
����չ��Ϣ����ΪWM/AlbumTitleʱ�����ֵ����ľ���ר������
����չ��Ϣ����ΪWM/Genreʱ�����ֵ����ľ������ɣ�
�������������Ǹ���־Flag�������������Ϊûʲô�õģ�ͨ��ֵΪ0����
��WM/TrackNumber��WM/Track��������չ��Ϣ�������ã�
��FlagΪ3��ʱ������ֵ��Ҳ������Ŀ��Ϣ������4���ֽڵ���������ʽ��ʾ��
��FlagΪ0��ʱ����Ŀ��Ϣ������ͨ���ַ�����ʽ��ʾ�ġ�
// �鿴http://msdn.microsoft.com/en-us/library/ms867702.aspx
Author
Copyright
Description
Rating
Title
WM/AlbumArtist
WM/AlbumCoverURL
WM/AlbumTitle
WM/AudioFileURL
WM/AudioSourceURL
WM/AuthorURL
WM/BeatsPerMinute
WM/Category
WM/Composer
WM/Conductor
WM/ContentDistributor
WM/ContentGroupDescription
WM/Director
WM/DVDID
WM/EncodedBy
WM/EncodingSettings
WM/EncodingTime
WM/Genre
WM/GenreID
WM/InitialKey
WM/ISRC
WM/Language
WM/Lyrics
WM/Lyrics_Synchronised
WM/MCDI
WM/MediaClassPrimaryID
WM/MediaClassSecondaryID
WM/MediaCredits
WM/MediaIsDelay
WM/MediaIsFinale
WM/MediaIsLive
WM/MediaIsPremiere
WM/MediaIsRepeat
WM/MediaIsSAP
WM/MediaIsSubtitled
WM/MediaIsStereo
WM/MediaNetworkAffiliation
WM/MediaOriginalBroadcastDateTime
WM/MediaOriginalChannel
WM/MediaStationCallSign
WM/MediaStationName
WM/ModifiedBy
WM/Mood
WM/OriginalAlbumTitle
WM/OriginalArtist
WM/OriginalFilename
WM/OriginalLyricist
WM/OriginalReleaseTime
WM/OriginalReleaseYear
WM/ParentalRating
WM/ParentalRatingReason
WM/PartOfSet
WM/Period
WM/Picture
WM/Producer
WM/PromotionURL
WM/Provider
WM/ProviderCopyright
WM/ProviderRating
WM/ProviderStyle
WM/Publisher
WM/RadioStationName
WM/RadioStationOwner
WM/SharedUserRating
WM/SubTitle
WM/SubTitleDescription
WM/Text
WM/ToolName
WM/ToolVersion
WM/Track
WM/TrackNumber
WM/UniqueFileIdentifier
WM/UserWebURL
WM/WMCollectionGroupID
WM/WMCollectionID
WM/WMContentID
WM/Writer
WM/Year

WMFSDKVersion
WMFSDKNeeded
IsVBR
*************************************************************************/
//////////////////////////////////////////////////////////////////////////  
// ����֡����  
enum WMA_FRAMESTYLE_T
{
	WMA_FRAMESTYLE_STANDARD,   // ��׼֡  
	WMA_FRAMESTYLE_EXTEND,     // ��չ֡  
	WMA_FRAMESTYLE_UNKNOWN,    // δ֪֡  
};

// ����֡��ʶ����ID  
enum WMA_FRAMEID_T
{
	// �������ͱ�׼֡�е�����  
	WMA_FRAMEID_AUTHOR = 0,
	WMA_FRAMEID_COPYRIGHT,
	WMA_FRAMEID_DESCRIPTION,
	WMA_FRAMEID_RATING,
	WMA_FRAMEID_TITLE,

	// �������Ͱ���ĸ����  
	WMA_FRAMEID_ALBUMARTIST,
	WMA_FRAMEID_ALBUMCOVERURL,
	WMA_FRAMEID_ALBUMTITLE,
	WMA_FRAMEID_AUDIOFILEURL,
	WMA_FRAMEID_AUDIOSOURCEURL,
	WMA_FRAMEID_AUTHORURL,
	WMA_FRAMEID_BPMT,       // WM/BeatsPerMinute  
	WMA_FRAMEID_CATEGORY,
	WMA_FRAMEID_COMPOSER,
	WMA_FRAMEID_CONDUCTOR,
	WMA_FRAMEID_CONTENTDISTRIBUTOR,
	WMA_FRAMEID_CGDPN, // WM/ContentGroupDescription  
	WMA_FRAMEID_DIRECTOR,
	WMA_FRAMEID_DVDID,
	WMA_FRAMEID_ENCODEDBY,
	WMA_FRAMEID_ENCODINGSETTINGS,
	WMA_FRAMEID_ENCODINGTIME,
	WMA_FRAMEID_GENRE,
	WMA_FRAMEID_GENREID,
	WMA_FRAMEID_INITIALKEY,
	WMA_FRAMEID_ISRC,
	WMA_FRAMEID_LANGUAGE,
	WMA_FRAMEID_LYRICS,
	WMA_FRAMEID_LYRICSSYNC, // WM/Lyrics_Synchronised  
	WMA_FRAMEID_MCDI,
	WMA_FRAMEID_MCPID, // WM/MediaClassPrimaryID  
	WMA_FRAMEID_MCSID, // WM/MediaClassSecondaryID  
	WMA_FRAMEID_MDACREDITS, // WM/MediaCredits  
	WMA_FRAMEID_MDAISDELAY,   // WM/MediaIsDelay  
	WMA_FRAMEID_MDAISFINALE,  // WM/MediaIsFinale  
	WMA_FRAMEID_MDAISLIVE,    // WM/MediaIsLive  
	WMA_FRAMEID_MDAISPREMIERE,  // WM/MediaIsPremiere  
	WMA_FRAMEID_MDAISREPEAT,    // WM/MediaIsRepeat  
	WMA_FRAMEID_MDAISSAP,       // WM/MediaIsSAP  
	WMA_FRAMEID_MDAISSUBTITLED,  // WM/MediaIsSubtitled  
	WMA_FRAMEID_MDAISSTEREO,     // WM/MediaIsStereo  
	WMA_FRAMEID_MDANETWORKAFFILIATION,  // WM/MediaNetworkAffiliation  
	WMA_FRAMEID_MDAORIGINALBDT,         // WM/MediaOriginalBroadcastDateTime  
	WMA_FRAMEID_MDAORIGINALCHANNEL,     // WM/MediaOriginalChannel  
	WMA_FRAMEID_MDASTATIONCALLSIGN,     // WM/MediaStationCallSign  
	WMA_FRAMEID_MDASTATIONNAME,         // WM/MediaStationName  
	WMA_FRAMEID_MODIFIEDBY,
	WMA_FRAMEID_MOOD,
	WMA_FRAMEID_ORIGALBUMTITLE,     // WM/OriginalAlbumTitle  
	WMA_FRAMEID_ORIGARTIST,         // WM/OriginalArtist  
	WMA_FRAMEID_ORIGFILENAME,       // WM/OriginalFilename  
	WMA_FRAMEID_ORIGLYRICIST,       // WM/OriginalLyricist  
	WMA_FRAMEID_ORIGRELEASETIME,    // WM/OriginalReleaseTime  
	WMA_FRAMEID_ORIGRELEASEYEAR,    // WM/OriginalReleaseYear  
	WMA_FRAMEID_PARENTALRATING,
	WMA_FRAMEID_PARENTALRATINGREASON,    // WM/ParentalRatingReason  
	WMA_FRAMEID_PARTOFSET,
	WMA_FRAMEID_PERIOD,
	WMA_FRAMEID_PICTURE,
	WMA_FRAMEID_PRODUCER,
	WMA_FRAMEID_PROMOTIONURL,
	WMA_FRAMEID_PROVIDER,
	WMA_FRAMEID_PROVIDERCPYRIGHT,
	WMA_FRAMEID_PROVIDERRATING,
	WMA_FRAMEID_PROVIDERSTYLE,
	WMA_FRAMEID_PUBLISHER,
	WMA_FRAMEID_RADIOSTATIONNAME,
	WMA_FRAMEID_RADIOSTATIONOWNER,
	WMA_FRAMEID_SHAREUSERRATING,
	WMA_FRAMEID_SUBTITLE,
	WMA_FRAMEID_SUBTITLEDESCRIPTION,
	WMA_FRAMEID_TEXT,
	WMA_FRAMEID_TOOLNAME,
	WMA_FRAMEID_TOOLVERSION,
	WMA_FRAMEID_TRACK,
	WMA_FRAMEID_TRACKNUMBER,
	WMA_FRAMEID_UNIQUEFILEIDENTIFIER,
	WMA_FRAMEID_USERWEBURL,
	WMA_FRAMEID_WMCOLLECTIONGROUPID,
	WMA_FRAMEID_WMCOLLECTIONID,
	WMA_FRAMEID_CONTENTID,
	WMA_FRAMEID_WRITER,
	WMA_FRAMEID_YEAR,

	WMA_FRAMEID_SDKVERSION, // WMFSDKVersion  
	WMA_FRAMEID_SDKNEEDED,  // WMFSDKNeeded  
	WMA_FRAMEID_ISVBR,      // IsVBR  


							////////////////////////////////////////////////////////////////////  
							WMA_FRAMEID_UNKNOWN, // δ֪  

};
//////////////////////////////////////////////////////////////////////////  
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

// ����֡ͷ��ʶ  
typedef struct _WMA_FRAMEHEADFLAG_INFO_
{
	unsigned char Flag[16];   // �̶���16���ֽڱ�ʾ֡����  
}WMAFRAMEHEADFLAG;

// ����WMA��׼֡����չ֡������ʶ  
const WMAFRAMEHEADFLAG WMA_FRAMEHEAD_STANDARD = { 0x33, 0x26, 0xB2, 0x75, 0x8E, 0x66, 0xCF, 0x11, 0xA6, 0xD9, 0x00, 0xAA, 0x00, 0x62, 0xCE, 0x6C }; //Data Object
const WMAFRAMEHEADFLAG WMA_FRAMEHEAD_EXTEND = { 0x40, 0xA4, 0xD0, 0xD2, 0x07, 0xE3, 0xD2, 0x11, 0x97, 0xF0, 0x00, 0xA0, 0xC9, 0x5E, 0xA8, 0x50 };

// ����֡��С  
typedef struct _WMA_FRAMEHEADSIZE_INFO_
{
	unsigned char Size[8];     // 8���ֽڱ�ʾ֡��С���ɺ���ǰ�洢���ݣ�  
}WMAFRAMEHEADSIZE;

// ����֡ͷ�ṹ  
typedef struct _WMA_FRAMEHEAD_INFO_
{
	WMAFRAMEHEADFLAG FrameFlag;
	WMAFRAMEHEADSIZE FrameSize;
}WMAFRAMEHEADINFO;

// �������ֵ������  
enum WMA_ATTRSTYLE_T
{
	WMA_ATTRSTYLE_TXT = 0, // �ַ�������  
	WMA_ATTRSTYLE_VAL,     // ֵ����  
};

// �������Խ������ݽṹ  
typedef struct _WMA_ATTR_INFO_
{
	WMA_FRAMEID_T ID;           // ID��ʾ����������  
	WMA_ATTRSTYLE_T AttrStyle;  // ֵ������  
	wstring StrContent;         // ������TXT����  
	unsigned long ValContent;   // ������ֵ����  
}WMAATTRINFO;

// �������Խ������ݶ�̬�����Ž���������  
typedef vector<WMAATTRINFO> CWMAAttrArray;

// ����ͼƬ�������ݽṹ  
typedef struct _WMA_PICITEMINFO_
{
	wstring MimeType;         // "image/png" or "image/jpeg"   
	char PicType;             // ˵������  
	wstring Description;      // ͼƬ����  
	//CMYPICINFO PicData;       // ͼƬ����  
}WMAPICITEMINFO;

// ����PIC���Խ������ݶ�̬�����Ž���������(���ܺ��ж���ͼƬ)  
typedef vector<WMAPICITEMINFO> CWMAPicArray;


//////////////////////////////////////////////////////////////////////////  
class CMyWMATag
{
public:
	// ���캯��  
	CMyWMATag();

	// ��������  
	~CMyWMATag();

public:
	// �ӿں���  
	// ����WMA�ļ����ɹ����������ӿڻ�ȡ��Ϣ  
	bool AnalyzeWMAFile(const char *filePathName);
	bool AnalyzeWMAFile(const wchar_t *filePathName);
	bool AnalyzeWMAFile(const wstring &filePathName);

	// �Ƿ�����ɹ�  
	bool BeSuccess();

	// ȡ��ͷ��С  
	unsigned long GetHeadSize();

	//////////////////////////////////////////////////////////////////////////  
	// ȡ��Txt�������ݽӿ�  
	bool GetTxtItemContent(WMA_FRAMEID_T frameID, wstring &itemContent);
	bool GetTxtItemContent(WMA_FRAMEID_T frameID, string &itemContent);

	//////////////////////////////////////////////////////////////////////////  
	// ȡ��ͼƬ�������ݽӿ�  
	// ȡ�õ�һ��ͼƬ  
	//bool GetFirstPic(CMYPICINFO &picInfo);

	// �����һ��ͼƬ  
	bool SaveFirstPicToFile(const char *filePathName);
	bool SaveFirstPicToFile(const wchar_t *filePathName);

	// ȡ�õ�һ��ͼƬ�ĵ�ַ����С  
	const unsigned char* GetFirstPicAddr(unsigned long *pPicSize = NULL);

	// ȡ�õ�һ��ͼƬ�Ĵ�С  
	unsigned long GetFirstPicSize();

	// ȡ��ͼƬ����  
	unsigned long GetPicCounts();

	// ȡ�õ�n��ͼƬ  
	//bool GetPic(unsigned long picIndex, CMYPICINFO &picInfo);

	// ȡ�õ�n��ͼƬ�ĵ�ַ  
	const unsigned char* GetPicAddr(unsigned long picIndex, unsigned long *pPicSize = NULL);

	// �����n��ͼƬ  
	bool SavePicToFile(unsigned long picIndex, const char *filePathName);
	bool SavePicToFile(unsigned long picIndex, const wchar_t *filePathName);

private:
	// ˽�к���  
	// �����ļ�ͷ�ṹ�ж��Ƿ�WMA�ļ�  
	bool BeWMAFile(const WMAHEADFLAG &headFlag);

	// �����Ϣ  
	void ClearDataInfo();

	// ����ͷ��С  
	unsigned long GetHeadSize(const WMAHEADSIZE &headSize);

	// �õ�֡����  
	WMA_FRAMESTYLE_T GetFrameStyle(const WMAFRAMEHEADFLAG &frameFlag);

	// �ж�����֡ͷ��ʶ�Ƿ����  
	bool BeSameFrameFlag(const WMAFRAMEHEADFLAG &firstFlag, const WMAFRAMEHEADFLAG &secondFlag);

	// �õ�֡�Ĵ�С  
	unsigned long GetFrameSize(const WMAFRAMEHEADSIZE &frameSize);

	// ������2���ֽ�ƴ�ӳɵ�������С  
	unsigned long CalUInt16Val(const unsigned char *pByteBuf, int bufSize);

	// ������4���ֽ�ƴ�ӳɵ�������С  
	unsigned long CalUInt32Val(const unsigned char *pByteBuf, int bufSize);

	// �����õ���֡����  
	unsigned long ParseFrameData(const unsigned char *pFrameData, unsigned long bufLength);

	// �õ�֡ͷ��Ϣ  
	bool GetFrameHeadInfo(const unsigned char *pFrameData, unsigned long bufLength, WMAFRAMEHEADINFO &frameHead);

	// ������׼֡����  
	bool ParseStandardFrame(const unsigned char *pItemFrameData, unsigned long frameSize, CWMAAttrArray &WMAAttrArray);

	// ������չ֡����  
	bool ParseExtendFrame(const unsigned char *pItemFrameData, unsigned long frameSize, CWMAAttrArray &WMAAttrArray, CWMAPicArray &WMAPicArray);

	// �õ�string  
	wstring GetWString(const unsigned char* pUCharBuf, unsigned long charsNums);

	// wstringת��Ϊstring  
	string TransFromWString(const wstring &wstrVal);

	// ȡ����չ֡����ID  
	WMA_FRAMEID_T GetExFrameID(const wstring &exName);

	// ȡ���ַ�������0x00,0x00  
	unsigned long GetWstringByWChars(const unsigned char* pPicDataBuf, unsigned long bufSize, wstring &strVal);

private:
	// ��Ա����  
	WMAHEADINFO m_HeadInfo;     // ���ͷ��Ϣ  
	bool m_bAnalyzeSuccess;     // ��������Ƿ�ɹ�  
	CWMAAttrArray m_WMAAttrArray; // ��Ž�������TAG��Ϣ  
	CWMAPicArray m_WMAPicArray;   // ��Ž�����ͼƬ��Ϣ  
};
//////////////////////////////////////////////////////////////////////////  
#endif  
