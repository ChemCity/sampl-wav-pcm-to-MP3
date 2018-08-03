#ifndef _CMY_WMA_TAG_H_  
#define _CMY_WMA_TAG_H_  
//////////////////////////////////////////////////////////////////////////  
//#include "CMyPicInfo.h"  
#include <string>  
#include <vector>  
using namespace std;
//////////////////////////////////////////////////////////////////////////  
/*************************************************************************
// 分为文件头和各个帧数据
+--------------------------------------------------------------+
|      Header (30 bytes)   HeadFlag:16; HeadSize:8; Unknow:6   |
+--------------------------------------------------------------+
|      Frames (1....n)                                         |
+--------------------------------------------------------------+
// 所有的TAG信息存放在标准帧和扩展帧中，其他帧可以不予考虑
// 所有的信息都是UNICODE编码
// 标准帧结构
+--------------------------------------------------------------+
|      Header (24 bytes)   HeadFlag:16; HeadSize:8;            |
+--------------------------------------------------------------+
|      标题信息大小（2 bytes）                                  |
+--------------------------------------------------------------+
|      艺术家信息大小（2 bytes）                                |
+--------------------------------------------------------------+
|      版权信息大小（2 bytes）                                  |
+--------------------------------------------------------------+
|      备注信息大小（2 bytes）                                  |
+--------------------------------------------------------------+
|      未知信息大小（2 bytes）                                  |
+--------------------------------------------------------------+
|      标题信息内容(0x00 0x00结束)                              |
+--------------------------------------------------------------+
|      艺术家信息内容(0x00 0x00结束)                            |
+--------------------------------------------------------------+
|      版权信息内容(0x00 0x00结束)                              |
+--------------------------------------------------------------+
|      备注信息内容(0x00 0x00结束)                              |
+--------------------------------------------------------------+
|      未知信息内容(0x00 0x00结束)                              |
+--------------------------------------------------------------+
// 扩展帧结构
+--------------------------------------------------------------+
|      Header (24 bytes)   HeadFlag:16; HeadSize:8;            |
+--------------------------------------------------------------+
|      扩展信息个数EXNO（2 bytes）                              |
+--------------------------------------------------------------+
|      EXINFO (1....EXNO)                                      |
+--------------------------------------------------------------+
// 每个扩展信息EXINFO结构
+--------------------------------------------------------------+
|      EXINFO NAME Size (2 bytes)   扩展信息名字大小            |
+--------------------------------------------------------------+
|      扩展信息名称                                             |
+--------------------------------------------------------------+
|      标志FLAG   (2 bytes)                                    |
+--------------------------------------------------------------+
|      值的大小   (2 bytes)                                     |
+--------------------------------------------------------------+
|      实际的值   (图片格式参考ID3V2.3)                         |
+--------------------------------------------------------------+
当扩展信息名字为WMFSDKVersion时，这个值表示的是这个WMA文件的版本；
当扩展信息名字为WM/AlbumTitle时，这个值代表的就是专辑名；
当扩展信息名字为WM/Genre时，这个值代表的就是流派；
下面再来看看那个标志Flag，这个基本上是为没什么用的（通常值为0），
对WM/TrackNumber和WM/Track这两个扩展信息名字有用，
当Flag为3的时候后面的值（也就是曲目信息）是以4个字节的整数的形式表示，
当Flag为0的时候，曲目信息是以普通的字符串形式表示的。
// 查看http://msdn.microsoft.com/en-us/library/ms867702.aspx
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
// 定义帧类型  
enum WMA_FRAMESTYLE_T
{
	WMA_FRAMESTYLE_STANDARD,   // 标准帧  
	WMA_FRAMESTYLE_EXTEND,     // 扩展帧  
	WMA_FRAMESTYLE_UNKNOWN,    // 未知帧  
};

// 定义帧标识类型ID  
enum WMA_FRAMEID_T
{
	// 基础类型标准帧中的内容  
	WMA_FRAMEID_AUTHOR = 0,
	WMA_FRAMEID_COPYRIGHT,
	WMA_FRAMEID_DESCRIPTION,
	WMA_FRAMEID_RATING,
	WMA_FRAMEID_TITLE,

	// 其他类型按字母排序  
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
							WMA_FRAMEID_UNKNOWN, // 未知  

};
//////////////////////////////////////////////////////////////////////////  
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

// 定义帧头标识  
typedef struct _WMA_FRAMEHEADFLAG_INFO_
{
	unsigned char Flag[16];   // 固定的16个字节表示帧类型  
}WMAFRAMEHEADFLAG;

// 定义WMA标准帧及扩展帧常量标识  
const WMAFRAMEHEADFLAG WMA_FRAMEHEAD_STANDARD = { 0x33, 0x26, 0xB2, 0x75, 0x8E, 0x66, 0xCF, 0x11, 0xA6, 0xD9, 0x00, 0xAA, 0x00, 0x62, 0xCE, 0x6C }; //Data Object
const WMAFRAMEHEADFLAG WMA_FRAMEHEAD_EXTEND = { 0x40, 0xA4, 0xD0, 0xD2, 0x07, 0xE3, 0xD2, 0x11, 0x97, 0xF0, 0x00, 0xA0, 0xC9, 0x5E, 0xA8, 0x50 };

// 定义帧大小  
typedef struct _WMA_FRAMEHEADSIZE_INFO_
{
	unsigned char Size[8];     // 8个字节表示帧大小（由后向前存储数据）  
}WMAFRAMEHEADSIZE;

// 定义帧头结构  
typedef struct _WMA_FRAMEHEAD_INFO_
{
	WMAFRAMEHEADFLAG FrameFlag;
	WMAFRAMEHEADSIZE FrameSize;
}WMAFRAMEHEADINFO;

// 定义解析值的类型  
enum WMA_ATTRSTYLE_T
{
	WMA_ATTRSTYLE_TXT = 0, // 字符串属性  
	WMA_ATTRSTYLE_VAL,     // 值属性  
};

// 定义属性解析内容结构  
typedef struct _WMA_ATTR_INFO_
{
	WMA_FRAMEID_T ID;           // ID表示解析的类型  
	WMA_ATTRSTYLE_T AttrStyle;  // 值的类型  
	wstring StrContent;         // 解析的TXT内容  
	unsigned long ValContent;   // 解析的值内容  
}WMAATTRINFO;

// 定义属性解析内容动态数组存放解析的内容  
typedef vector<WMAATTRINFO> CWMAAttrArray;

// 定义图片解析内容结构  
typedef struct _WMA_PICITEMINFO_
{
	wstring MimeType;         // "image/png" or "image/jpeg"   
	char PicType;             // 说明如下  
	wstring Description;      // 图片描述  
	//CMYPICINFO PicData;       // 图片数据  
}WMAPICITEMINFO;

// 定义PIC属性解析内容动态数组存放解析的内容(可能含有多张图片)  
typedef vector<WMAPICITEMINFO> CWMAPicArray;


//////////////////////////////////////////////////////////////////////////  
class CMyWMATag
{
public:
	// 构造函数  
	CMyWMATag();

	// 析构函数  
	~CMyWMATag();

public:
	// 接口函数  
	// 解析WMA文件，成功后调用下面接口获取信息  
	bool AnalyzeWMAFile(const char *filePathName);
	bool AnalyzeWMAFile(const wchar_t *filePathName);
	bool AnalyzeWMAFile(const wstring &filePathName);

	// 是否解析成功  
	bool BeSuccess();

	// 取得头大小  
	unsigned long GetHeadSize();

	//////////////////////////////////////////////////////////////////////////  
	// 取得Txt属性内容接口  
	bool GetTxtItemContent(WMA_FRAMEID_T frameID, wstring &itemContent);
	bool GetTxtItemContent(WMA_FRAMEID_T frameID, string &itemContent);

	//////////////////////////////////////////////////////////////////////////  
	// 取得图片属性内容接口  
	// 取得第一张图片  
	//bool GetFirstPic(CMYPICINFO &picInfo);

	// 保存第一张图片  
	bool SaveFirstPicToFile(const char *filePathName);
	bool SaveFirstPicToFile(const wchar_t *filePathName);

	// 取得第一张图片的地址及大小  
	const unsigned char* GetFirstPicAddr(unsigned long *pPicSize = NULL);

	// 取得第一行图片的大小  
	unsigned long GetFirstPicSize();

	// 取得图片数量  
	unsigned long GetPicCounts();

	// 取得第n张图片  
	//bool GetPic(unsigned long picIndex, CMYPICINFO &picInfo);

	// 取得第n张图片的地址  
	const unsigned char* GetPicAddr(unsigned long picIndex, unsigned long *pPicSize = NULL);

	// 保存第n张图片  
	bool SavePicToFile(unsigned long picIndex, const char *filePathName);
	bool SavePicToFile(unsigned long picIndex, const wchar_t *filePathName);

private:
	// 私有函数  
	// 根据文件头结构判断是否WMA文件  
	bool BeWMAFile(const WMAHEADFLAG &headFlag);

	// 清除信息  
	void ClearDataInfo();

	// 计算头大小  
	unsigned long GetHeadSize(const WMAHEADSIZE &headSize);

	// 得到帧类型  
	WMA_FRAMESTYLE_T GetFrameStyle(const WMAFRAMEHEADFLAG &frameFlag);

	// 判断两个帧头标识是否相等  
	bool BeSameFrameFlag(const WMAFRAMEHEADFLAG &firstFlag, const WMAFRAMEHEADFLAG &secondFlag);

	// 得到帧的大小  
	unsigned long GetFrameSize(const WMAFRAMEHEADSIZE &frameSize);

	// 计算由2个字节拼接成的整数大小  
	unsigned long CalUInt16Val(const unsigned char *pByteBuf, int bufSize);

	// 计算由4个字节拼接成的整数大小  
	unsigned long CalUInt32Val(const unsigned char *pByteBuf, int bufSize);

	// 解析得到的帧数据  
	unsigned long ParseFrameData(const unsigned char *pFrameData, unsigned long bufLength);

	// 得到帧头信息  
	bool GetFrameHeadInfo(const unsigned char *pFrameData, unsigned long bufLength, WMAFRAMEHEADINFO &frameHead);

	// 解析标准帧数据  
	bool ParseStandardFrame(const unsigned char *pItemFrameData, unsigned long frameSize, CWMAAttrArray &WMAAttrArray);

	// 解析扩展帧数据  
	bool ParseExtendFrame(const unsigned char *pItemFrameData, unsigned long frameSize, CWMAAttrArray &WMAAttrArray, CWMAPicArray &WMAPicArray);

	// 得到string  
	wstring GetWString(const unsigned char* pUCharBuf, unsigned long charsNums);

	// wstring转换为string  
	string TransFromWString(const wstring &wstrVal);

	// 取得扩展帧属性ID  
	WMA_FRAMEID_T GetExFrameID(const wstring &exName);

	// 取得字符串根据0x00,0x00  
	unsigned long GetWstringByWChars(const unsigned char* pPicDataBuf, unsigned long bufSize, wstring &strVal);

private:
	// 成员变量  
	WMAHEADINFO m_HeadInfo;     // 存放头信息  
	bool m_bAnalyzeSuccess;     // 保存解析是否成功  
	CWMAAttrArray m_WMAAttrArray; // 存放解析到的TAG信息  
	CWMAPicArray m_WMAPicArray;   // 存放解析的图片信息  
};
//////////////////////////////////////////////////////////////////////////  
#endif  
