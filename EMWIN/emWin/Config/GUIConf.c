#include "GUI.h"
#include "sdram.h"
#include "malloc.h"

//����EMWIN�ڴ��С
#define GUI_NUMBYTES  (8*1024*1024)
#define GUI_BLOCKSIZE 0XA0  //���С

//GUI_X_Config
//��ʼ����ʱ�����,��������emwin��ʹ�õ��ڴ�
void GUI_X_Config(void) {

	U32 *aMemory = mymalloc(SRAMEX,GUI_NUMBYTES); //���ⲿSRAM�з���GUI_NUMBYTES�ֽڵ��ڴ�
	GUI_ALLOC_AssignMemory((void*)aMemory, GUI_NUMBYTES); //Ϊ�洢����ϵͳ����һ���洢��
	GUI_ALLOC_SetAvBlockSize(GUI_BLOCKSIZE); //���ô洢���ƽ���ߴ�,����Խ��,���õĴ洢������Խ��
	GUI_SetDefaultFont(GUI_FONT_6X8); //����Ĭ������
}

