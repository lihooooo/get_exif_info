#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include "FreeImage.h"









/*-----------------------------------旋转图片-------------------------------------*/

extern "C" 
{
	int FreeImage_RotateImg(const char *pImgPath, int direction);
}


int FreeImage_RotateImg(const char *pImgPath, int direction)
{
	FIBITMAP *src_bitmap = NULL;
	FIBITMAP *target_bitmap = NULL;
	
	FreeImage_Initialise(TRUE);
	
	// 图片格式
    FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
	
	// 获取图片格式
    /* 此处需要特别注意，即使后缀名是.png并不代表图片格式就真是PNG格式，这里先通过FreeImage_GetFileType函数获取图片格式，
     然后再进行加载，否则，也许会出现加载失败的情况。
     */
    fif = FreeImage_GetFileType(pImgPath);
    if (fif == FIF_UNKNOWN)
        fif = FreeImage_GetFIFFromFilename(pImgPath);
	if ((fif != FIF_UNKNOWN) && FreeImage_FIFSupportsReading(fif))
	{
        src_bitmap = FreeImage_Load(fif, pImgPath, JPEG_DEFAULT);
		if (!src_bitmap)
		{
			fprintf(stderr, "Fail to Load Image!\n");
			return -1;
		}
    }
	
	//根据方向旋转图像
	switch(direction)
	{
		case 1: break;
		case 3: target_bitmap = FreeImage_RotateClassic(target_bitmap, 180.0);break;
		case 6: target_bitmap = FreeImage_RotateClassic(target_bitmap, 270.0);break;
		case 8: target_bitmap = FreeImage_RotateClassic(target_bitmap, 90.0);break;
		default: break;
	}
	if(target_bitmap == NULL)
	{
		// 从内存中删除载入图片，防止内存泄漏
		FreeImage_Unload(src_bitmap);
		
		// 撤销初始化
		FreeImage_DeInitialise();
		
		return -1;
	}
	
	// 保存更新后的图片
    FreeImage_Save(fif, target_bitmap, pImgPath, JPEG_DEFAULT);
    
    // 从内存中删除载入图片，防止内存泄漏
    FreeImage_Unload(src_bitmap);
	
    // 撤销初始化
    FreeImage_DeInitialise();
	
	return 0;
}



