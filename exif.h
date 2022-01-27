#ifndef __EXIF_H
#define __EXIF_H


#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

//标签结构体
typedef struct __EXIF_TAG
{
	u16 tag_number;			//标签的标识
	u16 value_format;		//数据的格式(1:unsigned byte, 2:ascii strings, 3:unsigned short, 4:unsigned long, 5:unsigned rational, 6:signed byte
							//				7:undefined, 8:signed short, 9:signed long, 10:signed rational, 11:single float, 12:double float)
	u32 value_amount;		//数据的数量
	u32 value_or_offset;	//数据或数据的偏移，相对于EXIF_IFD.head_fseek的偏移
	
	struct __EXIF_TAG *next;
} EXIF_Tag;

//IFD结构体
typedef struct __EXIF_IFD
{
	FILE *pfd;									//图片的文件指针
	
	u16 FFE1_size;								//FFE1的大小
	
	u8 endian;									//大小端
	
	u32 head_fseek;								//IFD在文件中开始的位置
	u32 ifd_offset;								//IFD内容相对于head_fseek的偏移，只有IFD0有
	
	u16 tag_amount;								//标签的数量
	struct __EXIF_TAG *tag_next;				//标签链表
	struct __EXIF_TAG *last_tag;				//指向最后一个tag的指针
	
	u32 subifd_offset;							//子IFD的偏移量，相对于head_fseek的偏移量，只有IFD0有
	struct __EXIF_IFD *subifd;					//指向子ifd的指针
	
	u32 interoperabilityifd_offset;				//互操性IFD的偏移量，相对于head_fseek的偏移量，只有子IFD有
	struct __EXIF_IFD *interoperabilityifd;		//指向子ifd的指针
	
	u32 makernoteifd_offset;					//子IFD的偏移量，相对于head_fseek的偏移量，只有子IFD有
	struct __EXIF_IFD *makernoteifd;			//指向子ifd的指针
	
	u32 ifd1_offset;							//IFD1的偏移量，相对于head_fseek的偏移量，只有IFD0有
	struct __EXIF_IFD *ifd1;					//指向IFD1的指针
} EXIF_IFD;


typedef struct __EXIF_VALUE
{
	void *value;		//数据
	u32 value_size;		//每一个数据的大小
	u32 value_amount;	//数据的数量
} EXIF_VALUE;



//功能：显示所有的exif数据
//参数：exif_ifd：EXIF_IFD结构指针
//返回：成功0，失败-1
char EXIF_ShowAllIfdTag(char *pic_path);


//功能：获得照片的拍摄时间
//参数：pic_path：照片路径
//返回：成功unix时间戳，失败-1
unsigned long long EXIF_GetCreateTime(char *pic_path);


int EXIF_GetExifThumbnail(const char *pSrcPicPath, const char *pDstPicPath);


#endif /*__EXIF_H*/


