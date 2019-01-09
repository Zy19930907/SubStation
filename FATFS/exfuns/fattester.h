#ifndef __FATTESTER_H
#define __FATTESTER_H 			   
#include "sys.h"	   
#include "ff.h"

u8 mf_mount(u8* path);
u32 mf_open(char*path,u8 mode);
u8 mf_close(void);
u8 mf_read(char *buf,u16 len);
u8 mf_write(char *dat,u16 len);
u8 mf_opendir(char* path);
u8 mf_closedir(void);
u8 mf_readdir(void);
u8 mf_scan_files(u8 * path);
u32 mf_showfree(u8 *drv);
u8 mf_lseek(u32 offset);
u32 mf_tell(void);
u32 mf_size(void);
u8 mf_mkdir(char *pname);
u8 mf_fmkfs(u8* path,u8 mode,u16 au);
u8 mf_unlink(u8 *pname);
u8 mf_rename(u8 *oldname,u8* newname);
void mf_getlabel(u8 *path);
void mf_setlabel(u8 *path); 
void mf_gets(u16 size);
u8 mf_putc(u8 c);
u8 mf_puts(u8*c);
u8 mf_getnextfilename(char *buf);
u8 mf_getnextfilenameex(char *buf,DIR *dir);
void mf_opendirex(DIR *dir,char *path);

#endif





























