#include "fattester.h"	 
#include "sdio_sdcard.h"
#include "exfuns.h"
#include "malloc.h"		  
#include "ff.h"
#include "string.h"
    
//Ϊ����ע�Ṥ����	 
//path:����·��������"0:"��"1:"
//mt:0��������ע�ᣨ�Ժ�ע�ᣩ��1������ע��
//����ֵ:ִ�н��
u8 mf_mount(u8* path)
{		   
	return f_mount(fs,(const TCHAR*)path,1); 
}
//��·���µ��ļ�
//path:·��+�ļ���
//mode:��ģʽ
//����ֵ:ִ�н��
u32 mf_open(char*path,u8 mode)
{
	if(!f_open(file,(const TCHAR*)path,mode))//���ļ���
	{
		f_mkdir(path);
		f_open(file,(const TCHAR*)path,mode);
	}
	return mf_size();
} 
//�ر��ļ�
//����ֵ:ִ�н��
u8 mf_close(void)
{
	f_close(file);
	return 0;
}

//д������
//dat:���ݻ�����
//len:д�볤��
//����ֵ:ִ�н��
u8 mf_write(char *dat,u16 len)
{			    
	u8 res;	   					   
	res=f_write(file,dat,len,&bw);
	return res;
}

//��Ŀ¼
 //path:·��
//����ֵ:ִ�н��
u8 mf_opendir(char* path)
{
	return f_opendir(&dir,(const TCHAR*)path);	
}

u8 mf_read(char *buf,u16 len)
{
	return f_read(file,buf,len,&br);
}
//�ر�Ŀ¼ 
//����ֵ:ִ�н��
u8 mf_closedir(void)
{
	return f_closedir(&dir);	
}
//���ȡ�ļ���
//����ֵ:ִ�н��
u8 mf_readdir(void)
{
	u8 res;    
	res=f_readdir(&dir,&fileinfo);	//��ȡһ���ļ�����Ϣ
	if(res!=FR_OK)return res;		//������ 
	return 0;
}			 

 //�����ļ�
 //path:·��
 //����ֵ:ִ�н��
u8 mf_scan_files(u8 * path)
{
	FRESULT res;	 
    res = f_opendir(&dir,(const TCHAR*)path); //��һ��Ŀ¼
    if (res == FR_OK) 
	{	
		while(1)
		{
	        res = f_readdir(&dir, &fileinfo);                   //��ȡĿ¼�µ�һ���ļ�
	        if (res != FR_OK || fileinfo.fname[0] == 0) break;  //������/��ĩβ��,�˳�
	        //if (fileinfo.fname[0] == '.') continue;             //�����ϼ�Ŀ¼
		} 
    }	   
    return res;	  
}

u8 mf_getnextfilename(char *buf)
{
	if(f_readdir(&dir, &fileinfo) == FR_OK)                   //��ȡĿ¼�µ�һ���ļ�
	{
		if(fileinfo.fname[0] != 0)
		{
			strcpy(buf,fileinfo.fname);
			return 1;
		}
	}
	return 0;
}

void mf_opendirex(DIR *dir,char *path)
{
	f_opendir(dir,path);
}
	  
u8 mf_getnextfilenameex(char *buf,DIR *dir)
{
	if(f_readdir(dir, &fileinfo) == FR_OK)                   //��ȡĿ¼�µ�һ���ļ�
	{
		if(fileinfo.fname[0] != 0)
		{
			strcpy(buf,fileinfo.fname);
			return 1;
		}
	}
	return 0;
}
//�ļ���дָ��ƫ��
//offset:����׵�ַ��ƫ����
//����ֵ:ִ�н��.
u8 mf_lseek(u32 offset)
{
	return f_lseek(file,offset);
}
//��ȡ�ļ���ǰ��дָ���λ��.
//����ֵ:λ��
u32 mf_tell(void)
{
	return f_tell(file);
}
//��ȡ�ļ���С
//����ֵ:�ļ���С
u32 mf_size(void)
{
	return f_size(file);
} 
//����Ŀ¼
//pname:Ŀ¼·��+����
//����ֵ:ִ�н��
u8 mf_mkdir(char *pname)
{
	return f_mkdir((const TCHAR *)pname);
}
//��ʽ��
//path:����·��������"0:"��"1:"
//mode:ģʽ
//au:�ش�С
//����ֵ:ִ�н��
u8 mf_fmkfs(u8* path,u8 mode,u16 au)
{
	return f_mkfs((const TCHAR*)path,mode,au);//��ʽ��,drv:�̷�;mode:ģʽ;au:�ش�С
} 
//ɾ���ļ�/Ŀ¼
//pname:�ļ�/Ŀ¼·��+����
//����ֵ:ִ�н��
u8 mf_unlink(u8 *pname)
{
	return  f_unlink((const TCHAR *)pname);
}

//�޸��ļ�/Ŀ¼����(���Ŀ¼��ͬ,�������ƶ��ļ�Ŷ!)
//oldname:֮ǰ������
//newname:������
//����ֵ:ִ�н��
u8 mf_rename(u8 *oldname,u8* newname)
{
	return  f_rename((const TCHAR *)oldname,(const TCHAR *)newname);
}
//��ȡ�̷����������֣�
//path:����·��������"0:"��"1:"  
void mf_getlabel(u8 *path)
{
	u8 buf[20];
	u32 sn=0;
	f_getlabel ((const TCHAR *)path,(TCHAR *)buf,(DWORD*)&sn);
}
//�����̷����������֣����11���ַ�������֧�����ֺʹ�д��ĸ����Լ����ֵ�
//path:���̺�+���֣�����"0:ALIENTEK"��"1:OPENEDV"  
void mf_setlabel(u8 *path)
{
	f_setlabel ((const TCHAR *)path);
} 

//���ļ������ȡһ���ַ���
//size:Ҫ��ȡ�ĳ���
void mf_gets(u16 size)
{
 	TCHAR* rbuf;
	rbuf=f_gets((TCHAR*)fatbuf,size,file);
	if(*rbuf==0)return  ;//û�����ݶ���
}
//��Ҫ_USE_STRFUNC>=1
//дһ���ַ����ļ�
//c:Ҫд����ַ�
//����ֵ:ִ�н��
u8 mf_putc(u8 c)
{
	return f_putc((TCHAR)c,file);
}
//д�ַ������ļ�
//c:Ҫд����ַ���
//����ֵ:д����ַ�������
u8 mf_puts(u8*c)
{
	return f_puts((TCHAR*)c,file);
}











