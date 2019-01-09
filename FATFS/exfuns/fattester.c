#include "fattester.h"	 
#include "sdio_sdcard.h"
#include "exfuns.h"
#include "malloc.h"		  
#include "ff.h"
#include "string.h"
    
//为磁盘注册工作区	 
//path:磁盘路径，比如"0:"、"1:"
//mt:0，不立即注册（稍后注册）；1，立即注册
//返回值:执行结果
u8 mf_mount(u8* path)
{		   
	return f_mount(fs,(const TCHAR*)path,1); 
}
//打开路径下的文件
//path:路径+文件名
//mode:打开模式
//返回值:执行结果
u32 mf_open(char*path,u8 mode)
{
	if(!f_open(file,(const TCHAR*)path,mode))//打开文件夹
	{
		f_mkdir(path);
		f_open(file,(const TCHAR*)path,mode);
	}
	return mf_size();
} 
//关闭文件
//返回值:执行结果
u8 mf_close(void)
{
	f_close(file);
	return 0;
}

//写入数据
//dat:数据缓存区
//len:写入长度
//返回值:执行结果
u8 mf_write(char *dat,u16 len)
{			    
	u8 res;	   					   
	res=f_write(file,dat,len,&bw);
	return res;
}

//打开目录
 //path:路径
//返回值:执行结果
u8 mf_opendir(char* path)
{
	return f_opendir(&dir,(const TCHAR*)path);	
}

u8 mf_read(char *buf,u16 len)
{
	return f_read(file,buf,len,&br);
}
//关闭目录 
//返回值:执行结果
u8 mf_closedir(void)
{
	return f_closedir(&dir);	
}
//打读取文件夹
//返回值:执行结果
u8 mf_readdir(void)
{
	u8 res;    
	res=f_readdir(&dir,&fileinfo);	//读取一个文件的信息
	if(res!=FR_OK)return res;		//出错了 
	return 0;
}			 

 //遍历文件
 //path:路径
 //返回值:执行结果
u8 mf_scan_files(u8 * path)
{
	FRESULT res;	 
    res = f_opendir(&dir,(const TCHAR*)path); //打开一个目录
    if (res == FR_OK) 
	{	
		while(1)
		{
	        res = f_readdir(&dir, &fileinfo);                   //读取目录下的一个文件
	        if (res != FR_OK || fileinfo.fname[0] == 0) break;  //错误了/到末尾了,退出
	        //if (fileinfo.fname[0] == '.') continue;             //忽略上级目录
		} 
    }	   
    return res;	  
}

u8 mf_getnextfilename(char *buf)
{
	if(f_readdir(&dir, &fileinfo) == FR_OK)                   //读取目录下的一个文件
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
	if(f_readdir(dir, &fileinfo) == FR_OK)                   //读取目录下的一个文件
	{
		if(fileinfo.fname[0] != 0)
		{
			strcpy(buf,fileinfo.fname);
			return 1;
		}
	}
	return 0;
}
//文件读写指针偏移
//offset:相对首地址的偏移量
//返回值:执行结果.
u8 mf_lseek(u32 offset)
{
	return f_lseek(file,offset);
}
//读取文件当前读写指针的位置.
//返回值:位置
u32 mf_tell(void)
{
	return f_tell(file);
}
//读取文件大小
//返回值:文件大小
u32 mf_size(void)
{
	return f_size(file);
} 
//创建目录
//pname:目录路径+名字
//返回值:执行结果
u8 mf_mkdir(char *pname)
{
	return f_mkdir((const TCHAR *)pname);
}
//格式化
//path:磁盘路径，比如"0:"、"1:"
//mode:模式
//au:簇大小
//返回值:执行结果
u8 mf_fmkfs(u8* path,u8 mode,u16 au)
{
	return f_mkfs((const TCHAR*)path,mode,au);//格式化,drv:盘符;mode:模式;au:簇大小
} 
//删除文件/目录
//pname:文件/目录路径+名字
//返回值:执行结果
u8 mf_unlink(u8 *pname)
{
	return  f_unlink((const TCHAR *)pname);
}

//修改文件/目录名字(如果目录不同,还可以移动文件哦!)
//oldname:之前的名字
//newname:新名字
//返回值:执行结果
u8 mf_rename(u8 *oldname,u8* newname)
{
	return  f_rename((const TCHAR *)oldname,(const TCHAR *)newname);
}
//获取盘符（磁盘名字）
//path:磁盘路径，比如"0:"、"1:"  
void mf_getlabel(u8 *path)
{
	u8 buf[20];
	u32 sn=0;
	f_getlabel ((const TCHAR *)path,(TCHAR *)buf,(DWORD*)&sn);
}
//设置盘符（磁盘名字），最长11个字符！！，支持数字和大写字母组合以及汉字等
//path:磁盘号+名字，比如"0:ALIENTEK"、"1:OPENEDV"  
void mf_setlabel(u8 *path)
{
	f_setlabel ((const TCHAR *)path);
} 

//从文件里面读取一段字符串
//size:要读取的长度
void mf_gets(u16 size)
{
 	TCHAR* rbuf;
	rbuf=f_gets((TCHAR*)fatbuf,size,file);
	if(*rbuf==0)return  ;//没有数据读到
}
//需要_USE_STRFUNC>=1
//写一个字符到文件
//c:要写入的字符
//返回值:执行结果
u8 mf_putc(u8 c)
{
	return f_putc((TCHAR)c,file);
}
//写字符串到文件
//c:要写入的字符串
//返回值:写入的字符串长度
u8 mf_puts(u8*c)
{
	return f_puts((TCHAR*)c,file);
}











