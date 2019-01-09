#include "siffontcreate.h"
#include "ff.h"
#include "malloc.h"

//字体定义
GUI_FONT SIF12_Font;
GUI_FONT SIF16_Font;
GUI_FONT SIF24_Font;
GUI_FONT SIF36_Font;

//字体文件
FIL SIF12FontFile;
FIL SIF16FontFile;
FIL SIF24FontFile;
FIL SIF36FontFile;

//字库文件缓冲区
u8 *SIF12FontBuff;
u8 *SIF16FontBuff;
u8 *SIF24FontBuff;
u8 *SIF36FontBuff;

//SD卡获取字库文件
//fxpath:字库路径
//buffer:字库文件缓冲区
//files:文件
//返回值:0，失败；其他，得到的字库文件
u8 *GetFont_Info(u8 *fxpath,FIL files)
{
    int result;
	u16 bread;
	u8 *FontBuffer;

	result=f_open(&files,(const TCHAR*)fxpath,FA_READ);	//打开字库文件
	FontBuffer=mymalloc(SRAMEX,files.obj.objsize); //从外部SDRAM申请内存

    if(FontBuffer==NULL)	
	{
		FontBuffer=NULL;	//内存申请失败
	}

	result = f_read(&files,FontBuffer,files.obj.objsize,(UINT *)&bread); //读取数据
	if(result != FR_OK) FontBuffer=NULL;

    return FontBuffer;
}

//创建SIF12字体，供EMWIN使用
void Create_SIF12(u8 *fxpath) 
{
    SIF12FontBuff=GetFont_Info(fxpath,SIF12FontFile);
    if(SIF12FontBuff!=NULL)
    {
        //创建SIF格式字体
        GUI_SIF_CreateFont(SIF12FontBuff,&SIF12_Font,GUI_SIF_TYPE_PROP_AA2);
    }
}
     
//创建SIF16字体，供EMWIN使用
void Create_SIF16(u8 *fxpath) 
{
    SIF16FontBuff=GetFont_Info(fxpath,SIF16FontFile);
    if(SIF16FontBuff!=NULL)
    {
        //创建SIF格式字体
        GUI_SIF_CreateFont(SIF16FontBuff,&SIF16_Font,GUI_SIF_TYPE_PROP_AA2);
    }
}

//创建SIF24字体，供EMWIN使用
void Create_SIF24(u8 *fxpath) 
{
    SIF24FontBuff=GetFont_Info(fxpath,SIF24FontFile);
    if(SIF24FontBuff!=NULL)
    {
        //创建SIF格式字体
        GUI_SIF_CreateFont(SIF24FontBuff,&SIF24_Font,GUI_SIF_TYPE_PROP_AA2);
    }
}

//创建SIF36字体，供EMWIN使用
void Create_SIF36(u8 *fxpath) 
{
    SIF36FontBuff=GetFont_Info(fxpath,SIF36FontFile);
    if(SIF36FontBuff!=NULL)
    {
        //创建SIF格式字体
        GUI_SIF_CreateFont(SIF36FontBuff,&SIF36_Font,GUI_SIF_TYPE_PROP_AA2);
    }
}
