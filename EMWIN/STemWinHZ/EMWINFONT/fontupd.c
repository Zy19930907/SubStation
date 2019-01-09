#include "fontupd.h"
#include "ff.h"	  
#include "tftlcd.h"  
#include "string.h"
#include "malloc.h"

														
//���������ֿ������Ϣ����ַ����С��
_font_info ftinfo;

u8 *UNIGBKBUF;
u8 *GBKHZ12BUF;
u8 *GBKHZ16BUF;
u8 *GBKHZ24BUF;
u8 *GBKHZ32BUF;

//���������ֿ������Ϣ����ַ����С��
_font_info ftinfo;

//�ֿ����ڴ����е�·��
char*const GBK_PATH[5]=
{
"/SYSTEM/FONT/UNIGBK.BIN",	//UNIGBK.BIN�Ĵ��λ��
"/SYSTEM/FONT/GBK12.FON",	//GBK12�Ĵ��λ��
//"/SYSTEM/FONT/GBK16.FON",	//GBK16�Ĵ��λ��
"/SYSTEM/FONT/HZ16.DZK",	//GBK16�Ĵ��λ��
"/SYSTEM/FONT/HZ24.DZK",	//GBK24�Ĵ��λ��
"/SYSTEM/FONT/HZ32.DZK",	//GBK32�Ĵ��λ��
}; 

//u8*const GBK_PATH[5]=
//{
//"/SYSTEM/FONT/UNIGBK.BIN",	//UNIGBK.BIN�Ĵ��λ��
//"/SYSTEM/FONT/GBK12.FON",	//GBK12�Ĵ��λ��
//"/SYSTEM/EMWINFONT/XBF16.xbf",	//GBK16�Ĵ��λ��
//"/SYSTEM/FONT/GBK24.FON",	//GBK24�Ĵ��λ��
//"/SYSTEM/FONT/GBK32.FON",	//GBK32�Ĵ��λ��
//}; 

//����ʱ����ʾ��Ϣ
char*const UPDATE_REMIND_TBL[5]=
{
"Updating UNIGBK.BIN",	//��ʾ���ڸ���UNIGBK.bin
"Updating GBK12.FON",	//��ʾ���ڸ���GBK12
"Updating GBK16.FON",	//��ʾ���ڸ���GBK16
"Updating GBK24.FON",	//��ʾ���ڸ���GBK24
"Updating GBK32.FON",	//��ʾ���ڸ���GBK32
}; 

//��ʾ��ǰ������½���
//x,y:����
//size:�����С
//fsize:�����ļ���С
//pos:��ǰ�ļ�ָ��λ��
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
//����ĳһ��
//x,y:����
//size:�����С
//fxpath:·��
//fx:���µ����� 0,ungbk;1,gbk12;2,gbk16;3,gbk24;4,gbk32;
//����ֵ:0,�ɹ�;����,ʧ��.
u8 updata_fontx(u8 *fxpath,u8 fx)
{							    
	FIL * fftemp;

 	u8 res;	
	u16 bread;
	u8 rval=0;	     
	fftemp=(FIL*)mymalloc(SRAMIN,sizeof(FIL));	//�����ڴ�	

 	res=f_open(fftemp,(const TCHAR*)fxpath,FA_READ); 
 	if(res)rval=2;//���ļ�ʧ��  
 	if(rval==0)	 
	{
		switch(fx)
		{
			case 0:						//����UNIGBK.BIN
				UNIGBKBUF = mymalloc(SRAMEX,fftemp->obj.objsize);
				ftinfo.ugbkaddr=(u32)UNIGBKBUF;	//��Ϣͷ֮�󣬽���UNIGBKת�����
				ftinfo.ugbksize=fftemp->obj.objsize;					//UNIGBK��С
				f_read(fftemp,UNIGBKBUF,fftemp->obj.objsize,(UINT *)&bread);		//��ȡ����	
				break;
			case 1:
				GBKHZ12BUF = mymalloc(SRAMEX,fftemp->obj.objsize);
				ftinfo.f12addr=(u32)GBKHZ12BUF;	//UNIGBK֮�󣬽���GBK12�ֿ�
				ftinfo.gbk12size=fftemp->obj.objsize;					//GBK12�ֿ��С
				f_read(fftemp,GBKHZ12BUF,fftemp->obj.objsize,(UINT *)&bread);		//��ȡ����	
				break;
			case 2:
				GBKHZ16BUF = mymalloc(SRAMEX,fftemp->obj.objsize);
				ftinfo.f16addr=(u32)GBKHZ16BUF;	//GBK12֮�󣬽���GBK16�ֿ�
				ftinfo.gbk16size=fftemp->obj.objsize;					//GBK16�ֿ��С
				f_read(fftemp,GBKHZ16BUF,fftemp->obj.objsize,(UINT *)&bread);		//��ȡ����	
				break;
			case 3:
				GBKHZ24BUF = mymalloc(SRAMEX,fftemp->obj.objsize);
				ftinfo.f24addr=(u32)GBKHZ24BUF;	//GBK16֮�󣬽���GBK24�ֿ�
				ftinfo.gbk24size=fftemp->obj.objsize;					//GBK24�ֿ��С
				f_read(fftemp,GBKHZ24BUF,fftemp->obj.objsize,(UINT *)&bread);		//��ȡ����	
				break;
			case 4:
				GBKHZ32BUF = mymalloc(SRAMEX,fftemp->obj.objsize);
				ftinfo.f32addr=(u32)GBKHZ32BUF;	//GBK24֮�󣬽���GBK32�ֿ�
				ftinfo.gbk32size=fftemp->obj.objsize;					//GBK32�ֿ��С
				f_read(fftemp,GBKHZ32BUF,fftemp->obj.objsize,(UINT *)&bread);		//��ȡ����	
				break;
		} 
		f_close(fftemp);		
	}			 
	myfree(SRAMIN,fftemp);	//�ͷ��ڴ�
	return res;
} 
//���������ļ�,UNIGBK,GBK12,GBK16,GBK24,GBK32һ�����
//x,y:��ʾ��Ϣ����ʾ��ַ
//size:�����С
//src:�ֿ���Դ����."0:",SD��;"1:",FLASH��,"2:",U��.
//��ʾ��Ϣ�����С										  
//����ֵ:0,���³ɹ�;
//		 ����,�������.	  
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
//	pname=mymalloc(SRAMIN,100);	//����100�ֽ��ڴ�  
//	buf=mymalloc(SRAMIN,4096);	//����4K�ֽ��ڴ�  
//	fftemp=(FIL*)mymalloc(SRAMIN,sizeof(FIL));	//�����ڴ�	
//	if(buf==NULL||pname==NULL||fftemp==NULL)
//	{
//		myfree(SRAMIN,fftemp);
//		myfree(SRAMIN,pname);
//		myfree(SRAMIN,buf);
//		return 5;		//�ڴ�����ʧ��
//	}
//	for(i=0;i<5;i++)	//�Ȳ����ļ�UNIGBK,GBK12,GBK16,GBK24,GBK32�Ƿ����� 
//	{ 
//		strcpy((char*)pname,(char*)src);				//copy src���ݵ�pname
//		strcat((char*)pname,(char*)GBK_PATH[i]);		//׷�Ӿ����ļ�·�� 
//		res=f_open(fftemp,(const TCHAR*)pname,FA_READ);	//���Դ�
//		if(res)
//		{
//			rval|=1<<7;	//��Ǵ��ļ�ʧ��  
//			break;		//������,ֱ���˳�
//		}
//	} 
//	myfree(SRAMIN,fftemp);	//�ͷ��ڴ�
//	if(rval==0)				//�ֿ��ļ�������.
//	{  
//		for(i=0;i<5;i++)	//���θ���UNIGBK,GBK12,GBK16,GBK24,GBK32
//		{	
//			
//			strcpy((char*)pname,(char*)src);				//copy src���ݵ�pname
//			strcat((char*)pname,(char*)GBK_PATH[i]); 		//׷�Ӿ����ļ�·�� 
//			strcpy(info,"LOADING FONT: ");
//			strcat(info,(char*)pname);
//			LCD_ShowString(100,120+i*36,360,24,16,(u8*)info);
//			res=updata_fontx(pname,i);	//�����ֿ�
//			
//			if(res)
//			{
//				myfree(SRAMIN,buf);
//				myfree(SRAMIN,pname);
//				return 1+i;
//			} 
//		} 
//		//ȫ�����º���
//		ftinfo.fontok=0XAA;
//	}
//	myfree(SRAMIN,pname);//�ͷ��ڴ� 
//	myfree(SRAMIN,buf);
//	return rval;//�޴���.			 
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
	pname=mymalloc(SRAMIN,100);	//����100�ֽ��ڴ�  
	buf=mymalloc(SRAMIN,4096);	//����4K�ֽ��ڴ�  
	fftemp=(FIL*)mymalloc(SRAMIN,sizeof(FIL));	//�����ڴ�	
	if(buf==NULL||pname==NULL||fftemp==NULL)
	{
		myfree(SRAMIN,fftemp);
		myfree(SRAMIN,pname);
		myfree(SRAMIN,buf);
		return 5;		//�ڴ�����ʧ��
	}
	for(i=0;i<5;i++)	//�Ȳ����ļ�UNIGBK,GBK12,GBK16,GBK24,GBK32�Ƿ����� 
	{ 
		strcpy((char*)pname,(char*)src);				//copy src���ݵ�pname
		strcat((char*)pname,(char*)GBK_PATH[i]);		//׷�Ӿ����ļ�·�� 
		res=f_open(fftemp,(const TCHAR*)pname,FA_READ);	//���Դ�
		if(res)
		{
			rval|=1<<7;	//��Ǵ��ļ�ʧ��  
			break;		//������,ֱ���˳�
		}
	} 
	myfree(SRAMIN,fftemp);	//�ͷ��ڴ�
	if(rval==0)				//�ֿ��ļ�������.
	{  
		strcpy((char*)pname,(char*)src);				//copy src���ݵ�pname
		strcat((char*)pname,(char*)GBK_PATH[2]); 		//׷�Ӿ����ļ�·�� 
		strcpy(info,"LOADING FONT: ");
		strcat(info,(char*)pname);
		LCD_ShowString(100,120+i*36,360,24,16,(u8*)info);
		res=updata_fontx(pname,2);	//�����ֿ�
		
		if(res)
		{
			myfree(SRAMIN,buf);
			myfree(SRAMIN,pname);
			return 1+i;
		} 
		//ȫ�����º���
		ftinfo.fontok=0XAA;
	}
	myfree(SRAMIN,pname);//�ͷ��ڴ� 
	myfree(SRAMIN,buf);
	return rval;//�޴���.			 
} 
//��ʼ������
//����ֵ:0,�ֿ����.
//		 ����,�ֿⶪʧ
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




















