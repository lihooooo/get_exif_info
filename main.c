#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>


#include "exif.h"


int main(int argc, char *argv[])
{
	//EXIF_ShowAllIfdTag(argv[1]);
	EXIF_GetExifThumbnail(argv[1], argv[2]);
	
	
	return 0;
} 
