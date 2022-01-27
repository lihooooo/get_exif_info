#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include "exif.h"


#define EXIF_LITTLE_ENDIAN		0
#define EXIF_BIG_ENDIAN			1
#define JPEG_ENDIAN 			1

//以16进制显示读到的数据
static void showDataInHex(const unsigned char buffer[], int n)
{
	
	printf("\n\n Addr  00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\n");
	int i = 0;
	for(; i < n; i++)
	{
		if(i % 16 == 0)
		{
			printf("\n0x%04X ", i);
		}
		
		printf("%02x ", buffer[i]);
	}
	
	printf("\n");
	
	return;
}


//功能：获得本机的大小端
//返回：LITTLE_ENDIAN/BIG_ENDIAN
static char EXIF_GetLocalEndian(void)
{
	u8 a[2] = {0x00, 0xff};
	
	u16 *test = (short *)a;
	if(*test == 0x00ff)
	{
		return EXIF_BIG_ENDIAN;
	}
	else if(*test == 0xff00)
	{
		return EXIF_LITTLE_ENDIAN;
	}
	
	return 0;
}

//功能：读1个字节
//参数：pfd：文件指针
//		loc_endian：本地大小端
//		rem_endian：图片的大小端
//		buffer：数据缓存区
//返回：成功0，失败-1
static char EXIF_Read1Byte(FILE *pfd, char loc_endian, char rem_endian, char *buffer)
{
	u32 n = 0;
	
	n = fread(buffer, 1, 1, pfd);
	if(n < 0)
	{
		return -1;
	}
	return 0;
}

//功能：读2个字节
//参数：pfd：文件指针
//		loc_endian：本地大小端
//		rem_endian：图片的大小端
//		buffer：数据缓存区
//返回：成功0，失败-1
static char EXIF_Read2Byte(FILE *pfd, char loc_endian, char rem_endian, char *buffer)
{
	u8 temp_buffer[2] = {0};
	u32 n = 0;
	
	n = fread(temp_buffer, 1, 2, pfd);
	if(n < 0)
	{
		return -1;
	}
	
	if(loc_endian == rem_endian)
	{
		buffer[0] = temp_buffer[0];
		buffer[1] = temp_buffer[1];
	}
	else
	{
		buffer[0] = temp_buffer[1];
		buffer[1] = temp_buffer[0];
	}
	
	return 0;
}

//功能：读4个字节
//参数：pfd：文件指针
//		loc_endian：本地大小端
//		rem_endian：图片的大小端
//		buffer：数据缓存区
//返回：成功0，失败-1
static char EXIF_Read4Byte(FILE *pfd, char loc_endian, char rem_endian, char *buffer)
{
	u8 temp_buffer[4] = {0};
	u32 n = 0;
	
	n = fread(temp_buffer, 1, 4, pfd);
	if(n < 0)
	{
		return -1;
	}
	
	if(loc_endian == rem_endian)
	{
		buffer[0] = temp_buffer[0];
		buffer[1] = temp_buffer[1];
		buffer[2] = temp_buffer[2];
		buffer[3] = temp_buffer[3];
	}
	else
	{
		buffer[0] = temp_buffer[3];
		buffer[1] = temp_buffer[2];
		buffer[2] = temp_buffer[1];
		buffer[3] = temp_buffer[0];
	}
	
	return 0;
}

//功能：读8个字节
//参数：pfd：文件指针
//		loc_endian：本地大小端
//		rem_endian：图片的大小端
//		buffer：数据缓存区
//返回：成功0，失败-1
static char EXIF_Read8Byte(FILE *pfd, char loc_endian, char rem_endian, char *buffer)
{
	u8 temp_buffer[8] = {0};
	u32 n = 0;
	
	n = fread(temp_buffer, 1, 8, pfd);
	if(n < 0)
	{
		return -1;
	}
	
	if(loc_endian == rem_endian)
	{
		buffer[0] = temp_buffer[0];
		buffer[1] = temp_buffer[1];
		buffer[2] = temp_buffer[2];
		buffer[3] = temp_buffer[3];
		buffer[4] = temp_buffer[4];
		buffer[5] = temp_buffer[5];
		buffer[6] = temp_buffer[6];
		buffer[7] = temp_buffer[7];
	}
	else
	{
		buffer[0] = temp_buffer[7];
		buffer[1] = temp_buffer[6];
		buffer[2] = temp_buffer[5];
		buffer[3] = temp_buffer[4];
		buffer[4] = temp_buffer[3];
		buffer[5] = temp_buffer[2];
		buffer[6] = temp_buffer[1];
		buffer[7] = temp_buffer[0];
	}
	
	return 0;
}

//功能：根据tag名获得tag对应的标识
//参数：tag_name：标签名
//返回：成功对应的标识值，失败-1
static unsigned int EXIF_GetExifTagNumber(const char *tag_name)
{
	//IFD0(主图像)使用的标签
	if(!strcmp(tag_name, "ImageDescription"))
	{
		return 0x010e;
	}
	else if(!strcmp(tag_name, "Make"))
	{
		return 0x010f;
	}
	else if(!strcmp(tag_name, "Model"))
	{
		return 0x0110;
	}
	else if(!strcmp(tag_name, "Orientation"))
	{
		return 0x0112;
	}
	else if(!strcmp(tag_name, "XResolution"))
	{
		return 0x011a;
	}
	else if(!strcmp(tag_name, "YResolution"))
	{
		return 0x011b;
	}
	else if(!strcmp(tag_name, "ResolutionUnit"))
	{
		return 0x0128;
	}
	else if(!strcmp(tag_name, "Software"))
	{
		return 0x0131;
	}
	else if(!strcmp(tag_name, "DateTime"))
	{
		return 0x0132;
	}
	else if(!strcmp(tag_name, "WhitePoint"))
	{
		return 0x013e;
	}
	else if(!strcmp(tag_name, "PrimaryChromaticities"))
	{
		return 0x013f;
	}
	else if(!strcmp(tag_name, "YCbCrCoefficients"))
	{
		return 0x0211;
	}
	else if(!strcmp(tag_name, "YCbCrPositioning"))
	{
		return 0x0213;
	}
	else if(!strcmp(tag_name, "ReferenceBlackWhite"))
	{
		return 0x0214;
	}
	else if(!strcmp(tag_name, "Copyright"))
	{
		return 0x8298;
	}
	else if(!strcmp(tag_name, "ExifOffset"))
	{
		return 0x8769;
	}
	//exif子IFD使用的标签
	else if(!strcmp(tag_name, "ExposureTime"))
	{
		return 0x829a;
	}
	else if(!strcmp(tag_name, "FNumber"))
	{
		return 0x829d;
	}
	else if(!strcmp(tag_name, "ExposureProgram"))
	{
		return 0x8822;
	}
	else if(!strcmp(tag_name, "ISOSpeedRatings"))
	{
		return 0x8827;
	}
	else if(!strcmp(tag_name, "ExifVersion"))
	{
		return 0x9000;
	}
	else if(!strcmp(tag_name, "DateTimeOriginal"))
	{
		return 0x9003;
	}
	else if(!strcmp(tag_name, "DateTimeDigitized"))
	{
		return 0x9004;
	}
	else if(!strcmp(tag_name, "ComponentsConfiguration"))
	{
		return 0x9101;
	}
	else if(!strcmp(tag_name, "CompressedBitsPerPixel"))
	{
		return 0x9102;
	}
	else if(!strcmp(tag_name, "ShutterSpeedValue"))
	{
		return 0x9201;
	}
	else if(!strcmp(tag_name, "ApertureValue"))
	{
		return 0x9202;
	}
	else if(!strcmp(tag_name, "BrightnessValue"))
	{
		return 0x9203;
	}
	else if(!strcmp(tag_name, "ExposureBiasValue"))
	{
		return 0x9204;
	}
	else if(!strcmp(tag_name, "MaxApertureValue"))
	{
		return 0x9205;
	}
	else if(!strcmp(tag_name, "SubjectDistance"))
	{
		return 0x9206;
	}
	else if(!strcmp(tag_name, "MeteringMode"))
	{
		return 0x9207;
	}
	else if(!strcmp(tag_name, "LightSource"))
	{
		return 0x9208;
	}
	else if(!strcmp(tag_name, "Flash"))
	{
		return 0x9209;
	}
	else if(!strcmp(tag_name, "FocalLength"))
	{
		return 0x920a;
	}
	else if(!strcmp(tag_name, "MakerNote"))
	{
		return 0x927c;
	}
	else if(!strcmp(tag_name, "UserComment"))
	{
		return 0x9286;
	}
	else if(!strcmp(tag_name, "SubsecTime"))
	{
		return 0x9290;
	}
	else if(!strcmp(tag_name, "SubsecTimeOriginal"))
	{
		return 0x9291;
	}
	else if(!strcmp(tag_name, "SubsecTimeDigitized"))
	{
		return 0x9292;
	}
	else if(!strcmp(tag_name, "FlashPixVersion"))
	{
		return 0xa000;
	}
	else if(!strcmp(tag_name, "ColorSpace"))
	{
		return 0xa001;
	}
	else if(!strcmp(tag_name, "ExifImageWidth"))
	{
		return 0xa002;
	}
	else if(!strcmp(tag_name, "ExifImageHeight"))
	{
		return 0xa003;
	}
	else if(!strcmp(tag_name, "RelatedSoundFile"))
	{
		return 0xa004;
	}
	else if(!strcmp(tag_name, "ExifInteroperabilityOffset"))
	{
		return 0xa005;
	}
	else if(!strcmp(tag_name, "FocalPlaneXResolution"))
	{
		return 0xa20e;
	}
	else if(!strcmp(tag_name, "FocalPlaneYResolution"))
	{
		return 0xa20f;
	}
	else if(!strcmp(tag_name, "FocalPlaneResolutionUnit"))
	{
		return 0xa210;
	}
	else if(!strcmp(tag_name, "ExposureIndex"))
	{
		return 0xa215;
	}
	else if(!strcmp(tag_name, "SensingMethod"))
	{
		return 0xa217;
	}
	else if(!strcmp(tag_name, "FileSource"))
	{
		return 0xa300;
	}
	else if(!strcmp(tag_name, "SceneType"))
	{
		return 0xa301;
	}
	else if(!strcmp(tag_name, "CFAPattern"))
	{
		return 0xa302;
	}
	//InteroperabilityIFD使用的标签
	else if(!strcmp(tag_name, "InteroperabilityIndex"))
	{
		return 0x0001;
	}
	else if(!strcmp(tag_name, "InteroperabilityVersion"))
	{
		return 0x0002;
	}
	else if(!strcmp(tag_name, "RelatedImageFileFormat"))
	{
		return 0x1000;
	}
	else if(!strcmp(tag_name, "RelatedImageWidth"))
	{
		return 0x1001;
	}
	else if(!strcmp(tag_name, "RelatedImageLength"))
	{
		return 0x1001;
	}
	//IFD1(缩略图)使用的标签
	else if(!strcmp(tag_name, "ImageWidth"))
	{
		return 0x0100;
	}
	else if(!strcmp(tag_name, "ImageLength"))
	{
		return 0x0101;
	}
	else if(!strcmp(tag_name, "BitsPerSample"))
	{
		return 0x0102;
	}
	else if(!strcmp(tag_name, "Compression"))
	{
		return 0x0103;
	}
	else if(!strcmp(tag_name, "PhotometricInterpretation"))
	{
		return 0x0106;
	}
	else if(!strcmp(tag_name, "StripOffsets"))
	{
		return 0x0111;
	}
	else if(!strcmp(tag_name, "SamplesPerPixel"))
	{
		return 0x0115;
	}
	else if(!strcmp(tag_name, "RowsPerStrip"))
	{
		return 0x0116;
	}
	else if(!strcmp(tag_name, "StripByteConunts"))
	{
		return 0x0117;
	}
	else if(!strcmp(tag_name, "XResolution"))
	{
		return 0x011a;
	}
	else if(!strcmp(tag_name, "YResolution"))
	{
		return 0x011b;
	}
	else if(!strcmp(tag_name, "PlanarConfiguration"))
	{
		return 0x011c;
	}
	else if(!strcmp(tag_name, "ResolutionUnit"))
	{
		return 0x0128;
	}
	else if(!strcmp(tag_name, "JpegIFOffset"))
	{
		return 0x0201;
	}
	else if(!strcmp(tag_name, "JpegIFByteCount"))
	{
		return 0x0202;
	}
	else if(!strcmp(tag_name, "YCbCrCoefficients"))
	{
		return 0x0211;
	}
	else if(!strcmp(tag_name, "YCbCrSubSampling"))
	{
		return 0x0212;
	}
	else if(!strcmp(tag_name, "YCbCrPositioning"))
	{
		return 0x0213;
	}
	else if(!strcmp(tag_name, "ReferenceBlackWhite"))
	{
		return 0x0214;
	}
	//找不到标签
	else
	{
		return -1;
	}
}

//功能：初始化IFD0
//参数：ifd结构体指针
//返回：成功返回0
static char EXIF_InitIfd(EXIF_IFD **exif_ifd, FILE *pfd)
{
	*exif_ifd = (EXIF_IFD *)malloc(sizeof(EXIF_IFD));
	memset(*exif_ifd, 0, sizeof(EXIF_IFD));
	
	(*exif_ifd)->pfd = pfd;
	
	return 0;
}

//功能：判断图片是不是jpg图片
//参数：pfd：文件指针
//返回：成功0，失败-1
static char EXIF_IsJpg(FILE *pfd)
{
	//获得主机和图片的大小端
	u8 loc_endian = EXIF_GetLocalEndian();
	u8 rem_endian = JPEG_ENDIAN;
	
	char buffer[2] = {0};
	
	if(EXIF_Read2Byte(pfd, loc_endian, rem_endian, buffer) < 0)
	{
		return -1;
	}
	
	//printf("EXIF_IsJpg = %02x\n", *(u16 *)buffer);
	
	if(*((u16 *)buffer) == 0xFFD8)
	{
		return 0;
	}
	else
	{
		return -1;
	}
}

//功能：定位FFE1，找到就将文件定位指针定位到FFE1后一个字节
//参数：pfd：文件指针
//返回：成功0，失败-1
static char EXIF_LocateFFE1(FILE *pfd)
{
	//获得主机和图片的大小端
	u8 loc_endian = EXIF_GetLocalEndian();
	u8 rem_endian = JPEG_ENDIAN;
	
	char buffer[2] = {0};
	
	while(1)
	{
		if(EXIF_Read2Byte(pfd, loc_endian, rem_endian, buffer) < 0)
		{
			return -1;
		}
		//printf("EXIF_LocateFFE1 = %02x\n", *(u16 *)buffer);
		
		if(*((u16 *)buffer) == 0xFFE1)
		{
			return 0;
		}
		else if(*((u16 *)buffer) == 0xFFDA)
		{
			return -1;
		}
		else
		{
			//再读两字节获得下一个FF的偏移量
			if(EXIF_Read2Byte(pfd, loc_endian, rem_endian, buffer) < 0)
			{
				return -1;
			}
			
			fseek(pfd, *((u16 *)buffer) - 2, SEEK_CUR);
		}
	}
	
	return 0;
}

//功能：判断是不是exif头（45786966 0000）
//参数：pfd：文件指针
//		exif_ifd：IFD结构体
//返回：成功0，失败-1
static char EXIF_GetExifHead(FILE *pfd, EXIF_IFD *exif_ifd)
{
	//获得主机和图片的大小端
	u8 loc_endian = EXIF_GetLocalEndian();
	u8 rem_endian = JPEG_ENDIAN;
	
	//获得FFE1的大小
	char buffer2[2] = {0};
	if(EXIF_Read2Byte(pfd, loc_endian, rem_endian, buffer2) < 0)
	{
		return -1;
	}
	exif_ifd->FFE1_size = *(u16 *)buffer2;
	
	//获得exif标识
	char buffer4[4] = {0};
	if(EXIF_Read4Byte(pfd, loc_endian, rem_endian, buffer4) < 0)
	{
		return -1;
	}
	//printf("EXIF_GetExifHead = %04x\n", *(u32 *)buffer4);
	
	if(*((u32 *)buffer4) == 0x45786966)
	{
		char buffer2[2] = {0};
		if(EXIF_Read2Byte(pfd, loc_endian, rem_endian, buffer2) < 0)
		{
			return -1;
		}
		
		if(*((u16 *)buffer2) == 0x0000)
		{
			//获得IFD头
			exif_ifd->head_fseek = ftell(pfd);
			//printf("exif_ifd->head_fseek = %d\n", exif_ifd->head_fseek);
			
			return 0;
		}
	}
	else
	{
		return -1;
	}
}

//功能：判断是不是tiff头（49492A00 08000000/4D4D002A 00000008）
//		并获得tiff的大小端
//参数：pfd：文件指针
//		exif_ifd：IFD结构体
//返回：成功0，失败-1
static char EXIF_GetTiffHead(FILE *pfd, EXIF_IFD *exif_ifd)
{
	//获得主机和图片的大小端
	u8 loc_endian = EXIF_GetLocalEndian();
	u8 jpeg_endian = JPEG_ENDIAN;
	
	char buffer[4] = {0};
	
	if(EXIF_Read4Byte(pfd, loc_endian, jpeg_endian, buffer) < 0)
	{
		return -1;
	}
	//printf("EXIF_GetTiffHead = %08x\n", *(u32 *)buffer);
	
	//4949小端
	if(*((u32 *)buffer) == 0x49492A00)
	{
		//读IFD内容偏移
		exif_ifd->endian = EXIF_LITTLE_ENDIAN;
		
		if(EXIF_Read4Byte(pfd, loc_endian, exif_ifd->endian, buffer) < 0)
		{
			return -1;
		}
		
		exif_ifd->ifd_offset = *((u32 *)buffer);
		fseek(pfd, exif_ifd->head_fseek + exif_ifd->ifd_offset, SEEK_SET);
	}
	//4d4d大端
	else if(*((u32 *)buffer) == 0x4D4D002A)
	{
		//读IFD内容偏移
		exif_ifd->endian = EXIF_BIG_ENDIAN;
		
		if(EXIF_Read4Byte(pfd, loc_endian, exif_ifd->endian, buffer) < 0)
		{
			return -1;
		}
		//printf("EXIF_GetTiffHead = %08x\n", *(u32 *)buffer);
		
		exif_ifd->ifd_offset = *((u32 *)buffer);
		fseek(pfd, exif_ifd->head_fseek + exif_ifd->ifd_offset, SEEK_SET);
		//printf("fseek = %d\n", exif_ifd->head_fseek + exif_ifd->ifd_offset);
	}
	else 
	{
		return -1;
	}
	
	return 0;
}

//功能：获得该IFD中的Tag数量
//参数：pfd：文件指针
//		exif_ifd：IFD结构体
//返回：成功0，失败-1
static char EXIF_GetTagAmount(FILE *pfd, EXIF_IFD *exif_ifd)
{
	
	//获得主机的大小端
	u8 loc_endian = EXIF_GetLocalEndian();
	
	char buffer[2] = {0};
	if(EXIF_Read2Byte(pfd, loc_endian, exif_ifd->endian, buffer) < 0)
	{
		return -1;
	}
	//printf("EXIF_GetTagAmount = %02x\n", *(u16 *)buffer);
	
	exif_ifd->tag_amount = *(u16 *)buffer;
	
	return 0;
}


//功能：读一个tag,并加入到IFD
//参数：pfd：文件指针
//		exif_ifd：EXIF_IFD结构指针
//		exif_tag：EXIF_Tag结构体指针
//返回：成功0，失败-1
static char EXIF_ReadSingleTag(FILE *pfd, EXIF_IFD *exif_ifd)
{
	//获得主机的大小端
	u8 loc_endian = EXIF_GetLocalEndian();
	
	EXIF_Tag *exif_tag = (EXIF_Tag *)malloc(sizeof(EXIF_Tag));
	memset(exif_tag, 0, sizeof(EXIF_Tag));
	
	char buffer2[2] = {0};
	char buffer4[4] = {0};
	//获得TAG标识
	if(EXIF_Read2Byte(pfd, loc_endian, exif_ifd->endian, buffer2) < 0)
	{
		return -1;
	}
	exif_tag->tag_number = *((u16 *)buffer2);
	
	//获得数据类型
	if(EXIF_Read2Byte(pfd, loc_endian, exif_ifd->endian, buffer2) < 0)
	{
		return -1;
	}
	exif_tag->value_format = *((u16 *)buffer2);
	
	//获得数据数量
	if(EXIF_Read4Byte(pfd, loc_endian, exif_ifd->endian, buffer4) < 0)
	{
		return -1;
	}
	exif_tag->value_amount = *((u32 *)buffer4);
	
	//获得数据值或偏移值
	if(EXIF_Read4Byte(pfd, loc_endian, exif_ifd->endian, buffer4) < 0)
	{
		return -1;
	}
	exif_tag->value_or_offset = *((u32 *)buffer4);
	
	//将TAG加入到IFD
	if(exif_ifd->tag_next == NULL)
	{
		exif_ifd->tag_next = exif_tag;
		exif_ifd->last_tag = exif_tag;
	}
	else
	{
		(exif_ifd->last_tag)->next = exif_tag;
		exif_ifd->last_tag = exif_tag;
	}
	
	return 0;
}

//功能：读IFD中所有的TAG
//参数：pfd：文件指针
//		exif_ifd：EXIF_IFD结构指针
//返回：成功0，失败-1
static char EXIF_ReadTag(FILE *pfd, EXIF_IFD *exif_ifd)
{
	int tag_amount = exif_ifd->tag_amount;
	for(; tag_amount > 0; tag_amount--)
	{
		EXIF_ReadSingleTag(pfd, exif_ifd);
	}
	
	return 0;
}

//功能：获取IFD1的偏移
//参数：pfd：文件指针
//		exif_ifd：EXIF_IFD结构指针
//返回：成功0，失败-1
static char EXIF_GetIfd1Offset(FILE *pfd, EXIF_IFD *exif_ifd)
{
	//获得主机的大小端
	u8 loc_endian = EXIF_GetLocalEndian();
	
	//获得IFD1的偏移
	char buffer[4] = {0};
	EXIF_Read4Byte(pfd, loc_endian, exif_ifd->endian, buffer);
	
	exif_ifd->ifd1_offset = *(u32 *)buffer;
	
	return 0;
}

//功能：获取IFD1信息
//参数：pfd：文件指针
//		exif_ifd：EXIF_IFD结构指针
//返回：成功0，没有ifd1 -1
static char EXIF_GetIfd1(FILE *pfd, EXIF_IFD *exif_ifd)
{
	if(exif_ifd->ifd1_offset == 0)
	{
		return -1;
	}
	else
	{
		exif_ifd->ifd1 = (EXIF_IFD *)malloc(sizeof(EXIF_IFD));
		memset(exif_ifd->ifd1, 0, sizeof(EXIF_IFD));
		
		//获得IFD1的偏移基准
		(exif_ifd->ifd1)->head_fseek = exif_ifd->head_fseek;
		(exif_ifd->ifd1)->endian = exif_ifd->endian;
		
		//定位到IFD1
		fseek(pfd, exif_ifd->head_fseek + exif_ifd->ifd1_offset, SEEK_SET);
		
		//获得IFD1的标签数量
		EXIF_GetTagAmount(pfd, exif_ifd->ifd1);
		
		//获得IFD1的标签
		EXIF_ReadTag(pfd, exif_ifd->ifd1);
		
		return 0;
	}
	
	return 0;
}

//功能：获取Interoperability IFD信息
//参数：pfd：文件指针
//		exif_ifd：EXIF_IFD结构指针
//返回：成功0，没有Interoperability ifd -1
static char EXIF_GetInteroperabilityIfd(FILE *pfd, EXIF_IFD *exif_ifd)
{
	EXIF_Tag *exif_tag = exif_ifd->tag_next;
	for(; exif_tag != NULL; exif_tag = exif_tag->next)
	{
		if(exif_tag->tag_number == 0xa005)
		{
			//获得interoperabilityifd IFD的偏移
			exif_ifd->interoperabilityifd_offset = exif_tag->value_or_offset;
			
			//创建interoperabilityifd IFD
			exif_ifd->interoperabilityifd = (EXIF_IFD *)malloc(sizeof(EXIF_IFD));
			memset(exif_ifd->interoperabilityifd, 0, sizeof(EXIF_IFD));
			
			//获得子IFD的偏移和大小端
			(exif_ifd->interoperabilityifd)->head_fseek = exif_ifd->head_fseek;	//子IFD中的Tag的偏移量仍然以主IFD的head_fseek作基准
			(exif_ifd->interoperabilityifd)->endian = exif_ifd->endian;
			
			//定位到interoperabilityifd IFD
			fseek(pfd, exif_ifd->head_fseek + exif_ifd->interoperabilityifd_offset, SEEK_SET);
			
			//获得子IFD的标签的数量
			EXIF_GetTagAmount(pfd, exif_ifd->interoperabilityifd);
			
			//读子IFD的标签
			EXIF_ReadTag(pfd, exif_ifd->interoperabilityifd);
			
			return 0;
		}
	}
	
	return -1;
}

//功能：makernote IFD信息
//参数：pfd：文件指针
//		exif_ifd：EXIF_IFD结构指针
//返回：成功0，makernote ifd -1
static char EXIF_GetMakerNoteIfd(FILE *pfd, EXIF_IFD *exif_ifd)
{
	EXIF_Tag *exif_tag = exif_ifd->tag_next;
	for(; exif_tag != NULL; exif_tag = exif_tag->next)
	{
		if(exif_tag->tag_number == 0x927c)
		{
			//获得interoperabilityifd IFD的偏移
			exif_ifd->makernoteifd_offset = exif_tag->value_or_offset;
			
			//创建interoperabilityifd IFD
			exif_ifd->makernoteifd = (EXIF_IFD *)malloc(sizeof(EXIF_IFD));
			memset(exif_ifd->makernoteifd, 0, sizeof(EXIF_IFD));
			
			//获得子IFD的偏移和大小端
			(exif_ifd->makernoteifd)->head_fseek = exif_ifd->head_fseek;	//子IFD中的Tag的偏移量仍然以主IFD的head_fseek作基准
			(exif_ifd->makernoteifd)->endian = exif_ifd->endian;
			
			//定位到interoperabilityifd IFD
			fseek(pfd, exif_ifd->head_fseek + exif_ifd->makernoteifd_offset, SEEK_SET);
			
			//获得子IFD的标签的数量
			EXIF_GetTagAmount(pfd, exif_ifd->makernoteifd);
			
			//读子IFD的标签
			EXIF_ReadTag(pfd, exif_ifd->makernoteifd);
			
			return 0;
		}
	}
	
	return -1;
}

//功能：获取子IFD信息
//参数：pfd：文件指针
//		exif_ifd：EXIF_IFD结构指针
//返回：成功0，没有子ifd -1
static char EXIF_GetSubIfd(FILE *pfd, EXIF_IFD *exif_ifd)
{
	//循环找子进程标签
	EXIF_Tag *exif_tag = exif_ifd->tag_next;
	for(; exif_tag != NULL; exif_tag = exif_tag->next)
	{
		if(exif_tag->tag_number == 0x8769)
		{
			//获得子IFD的偏移
			exif_ifd->subifd_offset = exif_tag->value_or_offset;

			//创建子IFD
			exif_ifd->subifd = (EXIF_IFD *)malloc(sizeof(EXIF_IFD));
			memset(exif_ifd->subifd, 0, sizeof(EXIF_IFD));
			
			//获得子IFD的偏移和大小端
			(exif_ifd->subifd)->head_fseek = exif_ifd->head_fseek;	//子IFD中的Tag的偏移量仍然以主IFD的head_fseek作基准
			(exif_ifd->subifd)->endian = exif_ifd->endian;
			
			//定位到子IFD
			fseek(pfd, exif_ifd->head_fseek + exif_ifd->subifd_offset, SEEK_SET);

			//获得子IFD的标签的数量
			EXIF_GetTagAmount(pfd, exif_ifd->subifd);
			
			//读子IFD的标签
			EXIF_ReadTag(pfd, exif_ifd->subifd);
			
			//获得Interoperability IFD
			if(EXIF_GetInteroperabilityIfd(pfd, exif_ifd->subifd) < 0)
			{
				printf("havn't Interoperability IFD\n");
			}
			
			//获得MakeNote IFD
			//if(EXIF_GetMakerNoteIfd(pfd, exif_ifd->subifd) < 0)
			//{
			//	printf("havn't MakerNote IFD\n");
			//}
			
			return 0;
		}
	}
	
	return -1;
}











//功能：获取IFD0信息
//参数：pfd：文件指针
//		exif_ifd：EXIF_IFD结构指针
//返回：成功0，没有子ifd -1
static char EXIF_AllocIfd0(char *pic_path, EXIF_IFD **exif_ifd)
{
	*exif_ifd = NULL;
	
	//读图片
	FILE *pfd = NULL;
	pfd = fopen(pic_path, "rb");
	if(pfd == NULL)
	{
		printf("fopen error\n");
		return -1;
	}
	
	//判断开头是不是jpg文件
	if(EXIF_IsJpg(pfd) < 0)
	{
		printf("EXIF_IsJpg error，maybe is not a jpg file\n");
		return -1;
	}
	
	//定位FFE1
	if(EXIF_LocateFFE1(pfd) < 0)
	{
		printf("EXIF_LocateFFE1 error，maybe havn't exif head\n");
		return -1;
	}
	
	//初始化IFD0结构体
	EXIF_InitIfd(exif_ifd, pfd);
	EXIF_IFD *exif_ifd0 = *exif_ifd;
	
	//获得exif头
	if(EXIF_GetExifHead(pfd, exif_ifd0) < 0)
	{
		printf("EXIF_GetExifHead error，maybe havn't exif head\n");
		return -1;
	}
	
	//获得tiff头和大小端
	if(EXIF_GetTiffHead(pfd, exif_ifd0) < 0)
	{
		printf("EXIF_GetTiffHead error，maybe havn't exif head\n");
		return -1;
	}
	
	//获得ifd0中标签的数量
	if(EXIF_GetTagAmount(pfd, exif_ifd0) < 0)
	{
		printf("EXIF_GetTagAmount error\n");
		return -1;
	}
	
	//获得ifd0中标签的信息
	if(EXIF_ReadTag(pfd, exif_ifd0) < 0)
	{
		printf("EXIF_ReadTag error\n");
		return -1;
	}
	
	//获得ifd1的偏移
	if(EXIF_GetIfd1Offset(pfd, exif_ifd0) < 0)
	{
		printf("EXIF_GetIfd1Offset error\n");
	}
	
	//获得ifd1的信息
	if(EXIF_GetIfd1(pfd, exif_ifd0) < 0)
	{
		printf("havn't ifd1\n");
	}
	
	//获得子ifd的信息
	if(EXIF_GetSubIfd(pfd, exif_ifd0) < 0)
	{
		printf("havn't subifd\n");
	}
	
	return 0;
}

//功能：释放IFD0的数据
//参数：exif_ifd：EXIF_IFD结构指针
//返回：成功0，没有子ifd -1
static char EXIF_DestroyIfd0(EXIF_IFD **exif_ifd)
{
	if(*exif_ifd == NULL)
	{
		return 0;
	}
	
	EXIF_IFD *exif_ifd0 = *exif_ifd;
	
	//释放IFD1
	if(exif_ifd0->ifd1 != NULL)
	{
		EXIF_IFD *exif_ifd1 = exif_ifd0->ifd1;
		
		//释放每一个标签
		if(exif_ifd1->tag_next != NULL)
		{
			EXIF_Tag *exif_tag_pre = exif_ifd1->tag_next;
			EXIF_Tag *exif_tag_next = exif_tag_pre->next;
			
			for(; exif_tag_next != NULL; exif_tag_pre = exif_tag_next, exif_tag_next = exif_tag_next->next)
			{
				free(exif_tag_pre);
			}
			free(exif_tag_pre);
		}
		
		free(exif_ifd1);
	}
	
	//释放子IFD
	if(exif_ifd0->subifd != NULL)
	{
		EXIF_IFD *exif_subifd = exif_ifd0->subifd;
		
		//释放interoperability ifd
		if(exif_subifd->interoperabilityifd != NULL)
		{
			EXIF_IFD *exif_interoperabilityifd = exif_subifd->interoperabilityifd;
			
			//释放每一个标签
			if(exif_interoperabilityifd->tag_next != NULL)
			{
				EXIF_Tag *exif_tag_pre = exif_interoperabilityifd->tag_next;
				EXIF_Tag *exif_tag_next = exif_tag_pre->next;
				
				for(; exif_tag_next != NULL; exif_tag_pre = exif_tag_next, exif_tag_next = exif_tag_next->next)
				{
					free(exif_tag_pre);
				}
				free(exif_tag_pre);
			}
			
			free(exif_interoperabilityifd);
		}
		
		//释放makernote ifd
		if(exif_subifd->makernoteifd != NULL)
		{
			EXIF_IFD *exif_makernoteifd = exif_subifd->makernoteifd;
			
			//释放每一个标签
			if(exif_makernoteifd->tag_next != NULL)
			{
				EXIF_Tag *exif_tag_pre = exif_makernoteifd->tag_next;
				EXIF_Tag *exif_tag_next = exif_tag_pre->next;
				
				for(; exif_tag_next != NULL; exif_tag_pre = exif_tag_next, exif_tag_next = exif_tag_next->next)
				{
					free(exif_tag_pre);
				}
				free(exif_tag_pre);
			}
			
			free(exif_makernoteifd);
		}
		
		//释放子IFD
		//释放每一个标签
		if(exif_subifd->tag_next != NULL)
		{
			EXIF_Tag *exif_tag_pre = exif_subifd->tag_next;
			EXIF_Tag *exif_tag_next = exif_tag_pre->next;
			
			for(; exif_tag_next != NULL; exif_tag_pre = exif_tag_next, exif_tag_next = exif_tag_next->next)
			{
				free(exif_tag_pre);
			}
			free(exif_tag_pre);
		}
		
		free(exif_subifd);
	}
	
	//释放IFD0
	//释放每一个标签
	if(exif_ifd0->tag_next != NULL)
	{
		EXIF_Tag *exif_tag_pre = exif_ifd0->tag_next;
		EXIF_Tag *exif_tag_next = exif_tag_pre->next;
		
		for(; exif_tag_next != NULL; exif_tag_pre = exif_tag_next, exif_tag_next = exif_tag_next->next)
		{
			free(exif_tag_pre);
		}
		free(exif_tag_pre);
	}
	
	fclose(exif_ifd0->pfd);
	
	free(exif_ifd0);
	*exif_ifd = NULL;
	
	return 0;
}

//功能：显示所有的exif数据
//参数：exif_ifd：EXIF_IFD结构指针
//返回：成功0，失败-1
char EXIF_ShowAllIfdTag(char *pic_path)
{
	EXIF_IFD *exif_ifd;
	if(EXIF_AllocIfd0(pic_path, &exif_ifd) < 0)
	{
		return -1;
	}
	
	if(exif_ifd == NULL)
	{
		return -1;
	}
	
	printf("---------------------------IFD0--------------------------\n");
	//显示IFD0数据
	if(exif_ifd->tag_next != NULL)
	{
		EXIF_Tag *exif_tag = exif_ifd->tag_next;
		for(; exif_tag != NULL; exif_tag = exif_tag->next)
		{
			printf("-----------------------------------------\n");
			printf("tag_number = %04x\n", exif_tag->tag_number);
			printf("value_format = %04x\n", exif_tag->value_format);
			printf("value_amount = %08x\n", (unsigned int)exif_tag->value_amount);
			printf("value_or_offset = %08x\n", (unsigned int)exif_tag->value_or_offset);
			printf("\n");
		}
	}
	
	printf("---------------------------IFD1--------------------------\n");
	//显示IFD1数据
	if(exif_ifd->ifd1 != NULL)
	{
		EXIF_Tag *exif_tag = (exif_ifd->ifd1)->tag_next;
		for(; exif_tag != NULL; exif_tag = exif_tag->next)
		{
			printf("-----------------------------------------\n");
			printf("tag_number = %04x\n", exif_tag->tag_number);
			printf("value_format = %04x\n", exif_tag->value_format);
			printf("value_amount = %08x\n", (unsigned int)exif_tag->value_amount);
			printf("value_or_offset = %08x\n", (unsigned int)exif_tag->value_or_offset);
			printf("\n");
		}
	}
	
	printf("--------------------------sub ifd--------------------------\n");
	//显示子IFD数据
	if(exif_ifd->subifd != NULL)
	{
		EXIF_Tag *exif_tag = (exif_ifd->subifd)->tag_next;
		for(; exif_tag != NULL; exif_tag = exif_tag->next)
		{
			printf("-----------------------------------------\n");
			printf("tag_number = %04x\n", exif_tag->tag_number);
			printf("value_format = %04x\n", exif_tag->value_format);
			printf("value_amount = %08x\n", (unsigned int)exif_tag->value_amount);
			printf("value_or_offset = %08x\n", (unsigned int)exif_tag->value_or_offset);
			printf("\n");
		}
	}
	
	printf("--------------------interoperability ifd-------------------\n");
	//显示interoperability ifd
	if((exif_ifd->subifd)->interoperabilityifd != NULL)
	{
		EXIF_Tag *exif_tag = ((exif_ifd->subifd)->interoperabilityifd)->tag_next;
		for(; exif_tag != NULL; exif_tag = exif_tag->next)
		{
			printf("-----------------------------------------\n");
			printf("tag_number = %04x\n", exif_tag->tag_number);
			printf("value_format = %04x\n", exif_tag->value_format);
			printf("value_amount = %08x\n", (unsigned int)exif_tag->value_amount);
			printf("value_or_offset = %08x\n", (unsigned int)exif_tag->value_or_offset);
			printf("\n");
		}
	}
	
	printf("------------------------makernote ifd-----------------------\n");
	//显示makernote ifd
	if((exif_ifd->subifd)->makernoteifd != NULL)
	{
		EXIF_Tag *exif_tag = ((exif_ifd->subifd)->makernoteifd)->tag_next;
		for(; exif_tag != NULL; exif_tag = exif_tag->next)
		{
			printf("-----------------------------------------\n");
			printf("tag_number = %04x\n", exif_tag->tag_number);
			printf("value_format = %04x\n", exif_tag->value_format);
			printf("value_amount = %08x\n", (unsigned int)exif_tag->value_amount);
			printf("buffer = %02d%02d\n", (unsigned short)exif_tag->value_or_offset, (unsigned short)(exif_tag->value_or_offset + 2));
			//printf("value_or_offset = %08x\n", (unsigned int)exif_tag->value_or_offset);
			printf("\n");
		}
	}
	
	EXIF_DestroyIfd0(&exif_ifd);
	
	return 0;
}

//功能：寻找标签值对应的标签
//参数：tag_number：标签值
//		exif_ifd：EXIF_IFD指针
//		exif_tag：对应的标签
//返回：成功0，失败-1
static char EXIF_FindTag(u32 tag_number, EXIF_IFD *exif_ifd, EXIF_Tag **exif_tag)
{
	*exif_tag = NULL;
	
	if(exif_ifd == NULL)
	{
		return -1;
	}
	
	//显示IFD0数据
	if(exif_ifd->tag_next != NULL)
	{
		EXIF_Tag *temp_exif_tag = exif_ifd->tag_next;
		for(; temp_exif_tag != NULL; temp_exif_tag = temp_exif_tag->next)
		{
			if(temp_exif_tag->tag_number == tag_number)
			{
				*exif_tag = temp_exif_tag;
				return 0;
			}
		}
	}
	
	//显示IFD1数据
	if(exif_ifd->ifd1 != NULL)
	{
		EXIF_Tag *temp_exif_tag = (exif_ifd->ifd1)->tag_next;
		for(; temp_exif_tag != NULL; temp_exif_tag = temp_exif_tag->next)
		{
			
			
			if(temp_exif_tag->tag_number == tag_number)
			{
				*exif_tag = temp_exif_tag;		
				return 0;
			}
		}
	}
	
	//显示子IFD数据
	if(exif_ifd->subifd != NULL)
	{
		EXIF_Tag *temp_exif_tag = (exif_ifd->subifd)->tag_next;
		for(; temp_exif_tag != NULL; temp_exif_tag = temp_exif_tag->next)
		{
			if(temp_exif_tag->tag_number == tag_number)
			{
				*exif_tag = temp_exif_tag;
				return 0;
			}
		}
	}
	
	//显示interoperability ifd
	if((exif_ifd->subifd)->interoperabilityifd != NULL)
	{
		EXIF_Tag *temp_exif_tag = ((exif_ifd->subifd)->interoperabilityifd)->tag_next;
		for(; temp_exif_tag != NULL; temp_exif_tag = temp_exif_tag->next)
		{
			if(temp_exif_tag->tag_number == tag_number)
			{
				*exif_tag = temp_exif_tag;
				return 0;
			}
		}
	}
	
	//显示makernote ifd
	if((exif_ifd->subifd)->makernoteifd != NULL)
	{
		EXIF_Tag *temp_exif_tag = ((exif_ifd->subifd)->makernoteifd)->tag_next;
		for(; temp_exif_tag != NULL; temp_exif_tag = temp_exif_tag->next)
		{
			if(temp_exif_tag->tag_number == tag_number)
			{
				*exif_tag = temp_exif_tag;
				return 0;
			}
		}
	}
	
	return -1;
}

//功能：获得数据所占用的字节数（如果小于4字节value_or_offset == value，否则value_or_offset == offset）
//参数：EXIF_Tag指针
//返回：字节数
static u32 EXIF_AllocValueSize(EXIF_Tag *exif_tag)
{
	u32 bytes = 0;
	
	switch(exif_tag->value_format)
	{
		case 1:
			bytes = 1;
			break;
		case 2:
			bytes = 1;
			break;
		case 3:
			bytes = 2;
			break;
		case 4:
			bytes = 4;
			break;
		case 5:
			bytes = 8;
			break;
		case 6:
			bytes = 1;
			break;
		case 7:
			bytes = 1;
			break;
		case 8:
			bytes = 2;
			break;
		case 9:
			bytes = 4;
			break;
		case 10:
			bytes = 8;
			break;
		case 11:
			bytes = 4;
			break;
		case 12:
			bytes = 8;
			break;
		default :break;
	}
	
	return bytes;
}



//功能：获得Tag的值
//参数：tag_name：标签名
//		exif_value：EXIF_VALUE指针
//返回：成功0，失败-1
static char EXIF_AllocValue(char *tag_name, EXIF_IFD *exif_ifd, EXIF_VALUE *exif_value)
{
	//根据标签名获得对应的标识值
	u32 tag_number = EXIF_GetExifTagNumber(tag_name);
	if(tag_number < 0)
	{
		printf("EXIF_GetExifTagNumber can't find %s\n", tag_name);
		return -1;
	}
	
	//查找标签值是否存在
	EXIF_Tag *exif_tag = NULL;
	if(EXIF_FindTag(tag_number, exif_ifd, &exif_tag) < 0)
	{
		printf("can't find %s\n", tag_name);
		return -1;
	}
		
	//根据EXIF_Tag->value_format获得字节大小
	exif_value->value_size = EXIF_AllocValueSize(exif_tag);
	
	//获得值的数量
	exif_value->value_amount = exif_tag->value_amount;
	
	//分配内存空间存储值
	u32 buffer_size = exif_value->value_size * exif_value->value_amount;
	
	
	//根据值或偏移找到值
	if(buffer_size <= 4)
	{
		exif_value->value = (void *)malloc(4);
		memset(exif_value->value, 0, 4);
		memcpy(exif_value->value, &(exif_tag->value_or_offset), 4);
		
		int offset = (4 - buffer_size) * 8;
		if(exif_ifd->endian == EXIF_BIG_ENDIAN)
		{
			unsigned int temp = *(unsigned int *)(exif_value->value);
			temp = temp >> offset;
			memcpy(exif_value->value, &temp, 4);
		}
		else
		{
			unsigned int temp = *(unsigned int *)(exif_value->value);
			temp = temp << offset;
			temp = temp >> offset;
			memcpy(exif_value->value, &temp, 4);
		}
	}
	else
	{
		exif_value->value = (void *)malloc(buffer_size);
		memset(exif_value->value, 0, buffer_size);
		
		fseek(exif_ifd->pfd, exif_ifd->head_fseek + exif_tag->value_or_offset, SEEK_SET);
		fread(exif_value->value, exif_value->value_size, exif_value->value_amount, exif_ifd->pfd);
	}
	
	return 0;
}

//功能：销毁EXIF_VALUE.value占用的空间
//参数：EXIF_VALUE指针
//返回：成功0
static char EXIF_DestroyValue(EXIF_VALUE exif_value)
{
	if(exif_value.value != NULL)
	{
		free(exif_value.value);
	}
	
	exif_value.value = NULL;
	
	return 0;
} 














/*----------------------------------获取拍摄时间---------------------------------------*/

//作用：根据年月日时分秒来获得unix时间戳
//参数：年月日时分秒
//成功返回unix时间戳
static unsigned long long EXIF_GetUnixTime(char *exif_time)
{
	if(exif_time == NULL)
	{
		return -1;
	}
	
	char month_day[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
	int year = 0;
	int month = 0;
	int day = 0;
	int hour = 0;
	int minute = 0;
	int second = 0;
	
	//获得年月日时分秒
	sscanf(exif_time, "%d:%d:%d %d:%d:%d", &year, &month, &day, &hour, &minute, &second);
	
	unsigned long long unix_time = 0;
	//算年
	unix_time = (year - 1970) * 365 * 24 * 3600;
	if(year > 1972)
	{
		unix_time += (int)(year - 1972)/4 * 24 * 3600;
	}
	else if(year == 1972 && month > 2)
	{
		unix_time += 24 * 3600;
	}
	
	//算月
	int i = 0;
	for(; i < month - 1; i++)
	{
		unix_time += month_day[i] * 24 * 3600;
	}
	
	//算日
	unix_time += day * 24 * 3600;
	
	//算时
	unix_time += hour * 3600;
	
	//算分
	unix_time += minute * 60;
	
	//算秒
	unix_time += second;
	
	//修正
	//减去8个小时
	unix_time -= 8 * 3600;
	
	return unix_time;
}


//功能：获得照片的拍摄时间
//参数：pic_path：照片路径
//返回：成功unix时间戳，失败-1
unsigned long long EXIF_GetCreateTime(char *pic_path)
{
	EXIF_VALUE exif_value;
	memset(&exif_value, 0, sizeof(EXIF_VALUE));
	
	//获得IFD表
	EXIF_IFD *exif_ifd;
	if(EXIF_AllocIfd0(pic_path, &exif_ifd) < 0)
	{
		return -1;
	}
	
	//获得时间
	unsigned long long unix_time;
	if(EXIF_AllocValue("DateTimeOriginal", exif_ifd, &exif_value) == 0)
	{
		unix_time = EXIF_GetUnixTime((char *)exif_value.value);
		EXIF_DestroyValue(exif_value);
		EXIF_DestroyIfd0(&exif_ifd);
		//printf("DateTimeOriginal unix_time = %lld\n", unix_time);
		return unix_time;
	}
	else if(EXIF_AllocValue("DateTime", exif_ifd, &exif_value) == 0)
	{
		unix_time = EXIF_GetUnixTime((char *)exif_value.value);
		EXIF_DestroyValue(exif_value);
		EXIF_DestroyIfd0(&exif_ifd);
		//printf("DateTime unix_time = %lld\n", unix_time);
		return unix_time;
	}
	else if(EXIF_AllocValue("DateTimeDigitized", exif_ifd, &exif_value) == 0)
	{
		unix_time = EXIF_GetUnixTime((char *)exif_value.value);
		EXIF_DestroyValue(exif_value);
		EXIF_DestroyIfd0(&exif_ifd);
		//printf("DateTimeDigitized unix_time = %lld\n", unix_time);
		return unix_time;
	}
	
	//释放
	EXIF_DestroyValue(exif_value);
	EXIF_DestroyIfd0(&exif_ifd);
	return -1;
}









/*---------------------------------获取缩略图--------------------------------------*/
int EXIF_GetExifThumbnail(const char *pSrcPicPath, const char *pDstPicPath)
{
	int loc_endian = EXIF_GetLocalEndian();
	
	int ret = 0;
	
	char *pBuffer = NULL;
	FILE *pDstFp = NULL;
	FILE *pSrcFp = NULL;
	EXIF_VALUE exifValue;
	memset(&exifValue, 0, sizeof(EXIF_VALUE));
	
	EXIF_IFD *pExifIfd = NULL;
	
	//获得IFD表
	if( (ret = EXIF_AllocIfd0((char *)pSrcPicPath, &pExifIfd)) < 0)
	{
		fprintf(stderr, "can't alloc IFD0 in %s:%d\n", __FILE__, __LINE__);
		goto end;
	}
	
	
	
	//如果没有IFD1，就没有缩略图
	if(pExifIfd->ifd1 == NULL)
	{
		fprintf(stderr, "can't find IFD1 in %s\n", pSrcPicPath);
		ret = -1;
		goto end;
	}
	
	//找到缩略图的压缩方式
	int compression = 0;
	if( (ret = EXIF_AllocValue("Compression", pExifIfd, &exifValue)) == 0)
	{
		compression = *(unsigned int *)exifValue.value;
		//printf("compression = %x\n", compression);
		EXIF_DestroyValue(exifValue);
		
		if(compression != 0x6)
		{
			fprintf(stderr, "compression format err in %s:%d\n", __FILE__, __LINE__);
			ret = -1;
			goto end;	
		}
	}
	else
	{
		fprintf(stderr, "can't find compression in %s:%d\n", __FILE__, __LINE__);
		ret = -1;
		goto end;
	}
	
	
	
	//找到缩略图的偏移和大小
	unsigned int JpegIFByteCount = 0;
	if( (ret = EXIF_AllocValue("JpegIFByteCount", pExifIfd, &exifValue)) == 0)
	{
		JpegIFByteCount = *(unsigned int *)exifValue.value;
		//printf("JpegIFByteCount = %x\n", JpegIFByteCount);
		EXIF_DestroyValue(exifValue);
	}
	else
	{
		fprintf(stderr, "can't find JpegIFByteCount in %s:%d\n", __FILE__, __LINE__);
		ret = -1;
		goto end;
	}
	
	unsigned int JpegIFOffset = 0;
	if( (ret = EXIF_AllocValue("JpegIFOffset", pExifIfd, &exifValue)) == 0)
	{
		JpegIFOffset = *(unsigned int *)exifValue.value;
		//printf("JpegIFOffset = %x\n", JpegIFOffset);
		EXIF_DestroyValue(exifValue);
	}
	else
	{
		fprintf(stderr, "can't find JpegIFOffset in %s:%d\n", __FILE__, __LINE__);
		ret = -1;
		goto end;
	}
	
	
	
	//打开源和目标文件
	if( (pSrcFp = fopen(pSrcPicPath, "rb")) == NULL)
	{
		fprintf(stderr, "fopen error in %s:%d\n", __FILE__, __LINE__);
		ret = -1;
		goto end;
	}
	if( (pDstFp = fopen(pDstPicPath, "wb")) == NULL)
	{
		fprintf(stderr, "fopen error in %s:%d\n", __FILE__, __LINE__);
		ret = -1;
		goto end;
	}
	
	//偏移
	int n = 0;
	if( (n = fseek(pSrcFp, JpegIFOffset + pExifIfd->head_fseek, SEEK_SET)) == -1)
	{
		fprintf(stderr, "fseek error in %s:%d\n", __FILE__, __LINE__);
		ret = -1;
		goto end;
	}
	
	//读和写入数据
	pBuffer = (char *)malloc(JpegIFByteCount);
	memset(pBuffer, 0, JpegIFByteCount);
	
	n = fread(pBuffer, 1, JpegIFByteCount, pSrcFp);
	if(n < JpegIFByteCount)
	{
		fprintf(stderr, "fread error in %s:%d\n", __FILE__, __LINE__);
		ret = -1;
		goto end;
	}
	n = fwrite(pBuffer, 1, JpegIFByteCount, pDstFp);
	if(n < JpegIFByteCount)
	{
		fprintf(stderr, "fwrite error in %s:%d\n", __FILE__, __LINE__);
		ret = -1;
		goto end;
	}
	
	//获得拍摄方向
	unsigned int Orientation = 0;
	if( (ret = EXIF_AllocValue("Orientation", pExifIfd, &exifValue)) == 0)
	{
		Orientation = *(unsigned int *)exifValue.value;
		//printf("Orientation = %x\n", Orientation);
		EXIF_DestroyValue(exifValue);
	}
	else
	{
		fprintf(stderr, "can't find Orientation in %s:%d\n", __FILE__, __LINE__);
		ret = -1;
		goto end;
	}
	
	
end:
	if(pDstFp)
		fclose(pDstFp);
	if(pSrcFp)
		fclose(pSrcFp);
	if(pBuffer)
		free(pBuffer);
	if(pExifIfd)
		EXIF_DestroyIfd0(&pExifIfd);
	
	return ret;
}








