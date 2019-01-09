#include "fontupd.h"
#include "ff.h"	  
#include "tftlcd.h"  
#include "string.h"
#include "malloc.h"

														
//用来保存字库基本信息，地址，大小等
_font_info ftinfo;

u8 *UNIGBKBUF;
u8 *GBKHZ12BUF;
u8 *GBKHZ16BUF;
u8 *GBKHZ24BUF;
u8 *GBKHZ32BUF;

//用来保存字库基本信息，地址，大小等
_font_info ftinfo;

//字库存放在磁盘中的路径
char*const GBK_PATH[5]=
{
"/SYSTEM/FONT/UNIGBK.BIN",	//UNIGBK.BIN的存放位置
"/SYSTEM/FONT/GBK12.FON",	//GBK12的存放位置
//"/SYSTEM/FONT/GBK16.FON",	//GBK16的存放位置
"/SYSTEM/FONT/HZ16.DZK",	//GBK16的存放位置
"/SYSTEM/FONT/HZ24.DZK",	//GBK24的存放位置
"/SYSTEM/FONT/HZ32.DZK",	//GBK32的存放位置
}; 

//u8*const GBK_PATH[5]=
//{
//"/SYSTEM/FONT/UNIGBK.BIN",	//UNIGBK.BIN的存放位置
//"/SYSTEM/FONT/GBK12.FON",	//GBK12的存放位置
//"/SYSTEM/EMWINFONT/XBF16.xbf",	//GBK16的存放位置
//"/SYSTEM/FONT/GBK24.FON",	//GBK24的存放位置
//"/SYSTEM/FONT/GBK32.FON",	//GBK32的存放位置
//}; 

//更新时的提示信息
char*const UPDATE_REMIND_TBL[5]=
{
"Updating UNIGBK.BIN",	//提示正在更新UNIGBK.bin
"Updating GBK12.FON",	//提示正在更新GBK12
"Updating GBK16.FON",	//提示正在更新GBK16
"Updating GBK24.FON",	//提示正在更新GBK24
"Updating GBK32.FON",	//提示正在更新GBK32
}; 

//显示当前字体更新进度
//x,y:坐标
//size:字体大小
//fsize:整个文件大小
//pos:当前文件指针位置
u32 fupd_prog(u16 x,u16 y,u8 size,u32 fsize,u32 pos)
{
	float prog;
	u8 t=0XFF;
	prog=(float)pos/fsize;
	prog*=100;
	if(t!=prog)
	{
		t=prog;
		if(t>100)t=100;
	}
	return 0;					    
} 
//更新某一个
//x,y:坐标
//size:字体大小
//fxpath:路径
//fx:更新的内容 0,ungbk;1,gbk12;2,gbk16;3,gbk24;4,gbk32;
//返回值:0,成功;其他,失败.
u8 updata_fontx(u8 *fxpath,u8 fx)
{							    
	FIL * fftemp;

 	u8 res;	
	u16 bread;
	u8 rval=0;	     
	fftemp=(FIL*)mymalloc(SRAMIN,sizeof(FIL));	//分配内存	

 	res=f_open(fftemp,(const TCHAR*)fxpath,FA_READ); 
 	if(res)rval=2;//打开文件失败  
 	if(rval==0)	 
	{
		switch(fx)
		{
			case 0:						//更新UNIGBK.BIN
				UNIGBKBUF = mymalloc(SRAMEX,fftemp->obj.objsize);
				ftinfo.ugbkaddr=(u32)UNIGBKBUF;	//信息头之后，紧跟UNIGBK转换码表
				ftinfo.ugbksize=fftemp->obj.objsize;					//UNIGBK大小
				f_read(fftemp,UNIGBKBUF,fftemp->obj.objsize,(UINT *)&bread);		//读取数据	
				break;
			case 1:
				GBKHZ12BUF = mymalloc(SRAMEX,fftemp->obj.objsize);
				ftinfo.f12addr=(u32)GBKHZ12BUF;	//UNIGBK之后，紧跟GBK12字库
				ftinfo.gbk12size=fftemp->obj.objsize;					//GBK12字库大小
				f_read(fftemp,GBKHZ12BUF,fftemp->obj.objsize,(UINT *)&bread);		//读取数据	
				break;
			case 2:
				GBKHZ16BUF = mymalloc(SRAMEX,fftemp->obj.objsize);
				ftinfo.f16addr=(u32)GBKHZ16BUF;	//GBK12之后，紧跟GBK16字库
				ftinfo.gbk16size=fftemp->obj.objsize;					//GBK16字库大小
				f_read(fftemp,GBKHZ16BUF,fftemp->obj.objsize,(UINT *)&bread);		//读取数据	
				break;
			case 3:
				GBKHZ24BUF = mymalloc(SRAMEX,fftemp->obj.objsize);
				ftinfo.f24addr=(u32)GBKHZ24BUF;	//GBK16之后，紧跟GBK24字库
				ftinfo.gbk24size=fftemp->obj.objsize;					//GBK24字库大小
				f_read(fftemp,GBKHZ24BUF,fftemp->obj.objsize,(UINT *)&bread);		//读取数据	
				break;
			case 4:
				GBKHZ32BUF = mymalloc(SRAMEX,fftemp->obj.objsize);
				ftinfo.f32addr=(u32)GBKHZ32BUF;	//GBK24之后，紧跟GBK32字库
				ftinfo.gbk32size=fftemp->obj.objsize;					//GBK32字库大小
				f_read(fftemp,GBKHZ32BUF,fftemp->obj.objsize,(UINT *)&bread);		//读取数据	
				break;
		} 
		f_close(fftemp);		
	}			 
	myfree(SRAMIN,fftemp);	//释放内存
	return res;
} 
//更新字体文件,UNIGBK,GBK12,GBK16,GBK24,GBK32一起更新
//x,y:提示信息的显示地址
//size:字体大小
//src:字库来源磁盘."0:",SD卡;"1:",FLASH盘,"2:",U盘.
//提示信息字体大小										  
//返回值:0,更新成功;
//		 其他,错误代码.	  
//u8 update_font(u8* src)
//{	
//	u8 *pname;
//	u32 *buf;
//	u8 res=0;		   
// 	u16 i;
//	FIL *fftemp;
//	u8 rval=0; 
//	char info[100];
//	res=0XFF;		
//	ftinfo.fontok=0XFF;
//	pname=mymalloc(SRAMIN,100);	//申请100字节内存  
//	buf=mymalloc(SRAMIN,4096);	//申请4K字节内存  
//	fftemp=(FIL*)mymalloc(SRAMIN,sizeof(FIL));	//分配内存	
//	if(buf==NULL||pname==NULL||fftemp==NULL)
//	{
//		myfree(SRAMIN,fftemp);
//		myfree(SRAMIN,pname);
//		myfree(SRAMIN,buf);
//		return 5;		//内存申请失败
//	}
//	for(i=0;i<5;i++)	//先查找文件UNIGBK,GBK12,GBK16,GBK24,GBK32是否正常 
//	{ 
//		strcpy((char*)pname,(char*)src);				//copy src内容到pname
//		strcat((char*)pname,(char*)GBK_PATH[i]);		//追加具体文件路径 
//		res=f_open(fftemp,(const TCHAR*)pname,FA_READ);	//尝试打开
//		if(res)
//		{
//			rval|=1<<7;	//标记打开文件失败  
//			break;		//出错了,直接退出
//		}
//	} 
//	myfree(SRAMIN,fftemp);	//释放内存
//	if(rval==0)				//字库文件都存在.
//	{  
//		for(i=0;i<5;i++)	//依次更新UNIGBK,GBK12,GBK16,GBK24,GBK32
//		{	
//			
//			strcpy((char*)pname,(char*)src);				//copy src内容到pname
//			strcat((char*)pname,(char*)GBK_PATH[i]); 		//追加具体文件路径 
//			strcpy(info,"LOADING FONT: ");
//			strcat(info,(char*)pname);
//			LCD_ShowString(100,120+i*36,360,24,16,(u8*)info);
//			res=updata_fontx(pname,i);	//更新字库
//			
//			if(res)
//			{
//				myfree(SRAMIN,buf);
//				myfree(SRAMIN,pname);
//				return 1+i;
//			} 
//		} 
//		//全部更新好了
//		ftinfo.fontok=0XAA;
//	}
//	myfree(SRAMIN,pname);//释放内存 
//	myfree(SRAMIN,buf);
//	return rval;//无错误.			 
//} 

u8 update_font(u8* src)
{	
	u8 *pname;
	u32 *buf;
	u8 res=0;		   
 	u16 i;
	FIL *fftemp;
	u8 rval=0; 
	char info[100];
	res=0XFF;		
	ftinfo.fontok=0XFF;
	pname=mymalloc(SRAMIN,100);	//申请100字节内存  
	buf=mymalloc(SRAMIN,4096);	//申请4K字节内存  
	fftemp=(FIL*)mymalloc(SRAMIN,sizeof(FIL));	//分配内存	
	if(buf==NULL||pname==NULL||fftemp==NULL)
	{
		myfree(SRAMIN,fftemp);
		myfree(SRAMIN,pname);
		myfree(SRAMIN,buf);
		return 5;		//内存申请失败
	}
	for(i=0;i<5;i++)	//先查找文件UNIGBK,GBK12,GBK16,GBK24,GBK32是否正常 
	{ 
		strcpy((char*)pname,(char*)src);				//copy src内容到pname
		strcat((char*)pname,(char*)GBK_PATH[i]);		//追加具体文件路径 
		res=f_open(fftemp,(const TCHAR*)pname,FA_READ);	//尝试打开
		if(res)
		{
			rval|=1<<7;	//标记打开文件失败  
			break;		//出错了,直接退出
		}
	} 
	myfree(SRAMIN,fftemp);	//释放内存
	if(rval==0)				//字库文件都存在.
	{  
		strcpy((char*)pname,(char*)src);				//copy src内容到pname
		strcat((char*)pname,(char*)GBK_PATH[2]); 		//追加具体文件路径 
		strcpy(info,"LOADING FONT: ");
		strcat(info,(char*)pname);
		LCD_ShowString(100,120+i*36,360,24,16,(u8*)info);
		res=updata_fontx(pname,2);	//更新字库
		
		if(res)
		{
			myfree(SRAMIN,buf);
			myfree(SRAMIN,pname);
			return 1+i;
		} 
		//全部更新好了
		ftinfo.fontok=0XAA;
	}
	myfree(SRAMIN,pname);//释放内存 
	myfree(SRAMIN,buf);
	return rval;//无错误.			 
} 
//初始化字体
//返回值:0,字库完好.
//		 其他,字库丢失
u8 font_init(void)
{		
	return update_font((u8*)"0:/"); 
}

//void Font_Read(u8* pBuffer,u32 ReadAddr,u16 NumByteToRead)   
//{ 
//	u16 i;
//	u8 *p = (u8*)ReadAddr;
//	for(i=0;i<NumByteToRead;i++)
//		*(pBuffer+i) = *(p+i);
//}  

void Font_Read(u8* pBuffer,u32 ReadAddr,u16 NumByteToRead,u8 size)   
{ 
	u16 i;
	FIL	fontFile;
	u8 index;
	u8 *p; 
	unsigned int j;

	switch(size)
	{
		case 12:
			index = 1;
			break;
		case 16:
			p = (u8*)(ReadAddr + GBKHZ16BUF);
			for(i=0;i<NumByteToRead;i++)
				*(pBuffer+i) = *(p+i);
			return;
		case 24:
			index = 3;
			break;
		case 32:
			index = 4;
			break;
	}
	f_open(&fontFile,GBK_PATH[index],FA_READ);
	f_lseek(&fontFile,ReadAddr);
	f_read(&fontFile,pBuffer,NumByteToRead,&j);
}  




















