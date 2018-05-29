/***
	***************************************************************************
	*	@file  	main.c
	*	@version V1.0
	*  @date    2018.5.2	
	*	@author  FANKE
	*	@brief   SD卡文件系统移植，FatFs版本 R0.13
   ***************************************************************************
   *  @description
	*
	*	实验平台：反客STM32F407核心板
	*	淘宝地址：https://shop212360197.taobao.com
	*	QQ交流群：536665479
	*
	*	功能说明：
	*
	*  1.移植FatFs，进行简单的测试
	* 	2.SD卡底层驱动移植于官方评估板例程
	*
	***************************************************************************
***/

#include "stm32f4xx.h"
#include "led.h"   
#include "delay.h"
#include "usart.h"
#include "ff.h"


FATFS 	SD_FatFs; 		// 文件系统对象
FRESULT 	MyFile_Res;    // 操作结果 
	
//	函数：FatFs_Check
//	功能：进行FatFs文件系统的挂载
//
void FatFs_Check(void)	//判断FatFs是否挂载成功，若没有创建FatFs则格式化SD卡
{
	BYTE work[FF_MAX_SS]; 
	
	MyFile_Res = f_mount(&SD_FatFs,"0:",1);	//挂载SD卡
	
	if (MyFile_Res == FR_OK)	//判断是否挂载成功
	{
		printf("\r\nSD文件系统挂载成功\r\n");
	}
	else		
	{
		printf("SD卡还未创建文件系统，即将格式化\r\n");
		
		MyFile_Res = f_mkfs("0:",FM_FAT32,0,work,sizeof work);		//格式化SD卡，FAT32，簇默认大小16K
		
		if (MyFile_Res == FR_OK)		//判断是否格式化成功
			printf("SD卡格式化成功！\r\n");
		else
			printf("格式化失败，请检查或更换SD卡！\r\n");
	}
}
//	函数：FatFs_GetVolume
//	功能：计算设备的容量，包括总容量和剩余容量
//
void FatFs_GetVolume(void)	// 计算设备容量
{
	FATFS *fs;		//定义结构体指针
	u32 SD_CardCapacity = 0;		//SD卡的总容量
	u32 SD_FreeCapacity = 0;		//SD卡空闲容量
	DWORD fre_clust, fre_sect, tot_sect; 	//空闲簇，空闲扇区数，总扇区数

	f_getfree("0:",&fre_clust,&fs);			//获取SD卡剩余的簇

	tot_sect = (fs->n_fatent-2) * fs->csize;	//总扇区数量 = 总的簇 * 每个簇包含的扇区数
	fre_sect = fre_clust * fs->csize;			//计算剩余的可用扇区数	   

	SD_CardCapacity = tot_sect * 512 / 1048576;	//SD卡总容量 = 总扇区数 * 每扇区的字节数 /1M
	SD_FreeCapacity = fre_sect * 512 / 1048576;	//计算剩余的容量，单位为M

	printf("-------------------获取设备容量信息-----------------\r\n");		
	printf("SD容量：%dMB\r\n",SD_CardCapacity);	
	printf("SD剩余：%dMB\r\n",SD_FreeCapacity);
}

//	函数：FatFs_FileTest
//	功能：进行文件写入和读取测试
//
u8  FatFs_FileTest(void)	//文件创建和写入测试
{
	u8 i = 0;
	u16 BufferSize = 0;	
	FIL	MyFile;			// 文件对象
	UINT 	MyFile_Num;		//	数据长度
	BYTE 	MyFile_WriteBuffer[] = "STM32F429 SD卡 文件系统测试";	//要写入的数据
	BYTE 	MyFile_ReadBuffer[1024];	//要读出的数据
	
	printf("-------------FatFs 文件创建和写入测试---------------\r\n");
	
	MyFile_Res = f_open(&MyFile,"0:FatFs Test.txt",FA_CREATE_ALWAYS | FA_WRITE);	//打开文件，若不存在则创建该文件
	if(MyFile_Res == FR_OK)
	{
		printf("文件打开/创建成功，准备写入数据...\r\n");
		
		MyFile_Res = f_write(&MyFile,MyFile_WriteBuffer,sizeof(MyFile_WriteBuffer),&MyFile_Num);	//向文件写入数据
		if (MyFile_Res == FR_OK)	
		{
			printf("写入成功，写入内容为：\r\n");
			printf("%s\r\n",MyFile_WriteBuffer);
		}
		else
		{
			printf("文件写入失败，请检查SD卡或重新格式化!\r\n");
			f_close(&MyFile);	  //关闭文件	
			return ERROR;			
		}
		f_close(&MyFile);	  //关闭文件			
	}
	else
	{
		printf("无法打开/创建文件，请检查SD卡或重新格式化!\r\n");
		f_close(&MyFile);	  //关闭文件	
		return ERROR;		
	}
	
	printf("-------------FatFs 文件读取测试---------------\r\n");	
	
	BufferSize = sizeof(MyFile_WriteBuffer)/sizeof(BYTE);									// 计算写入的数据长度
	MyFile_Res = f_open(&MyFile,"0:FatFs Test.txt",FA_OPEN_EXISTING | FA_READ);	//打开文件，若不存在则创建该文件
	MyFile_Res = f_read(&MyFile,MyFile_ReadBuffer,BufferSize,&MyFile_Num);			// 读取文件
	if(MyFile_Res == FR_OK)
	{
		printf("文件读取成功，正在校验数据...\r\n");
		
		for(i=0;i<BufferSize;i++)
		{
			if(MyFile_WriteBuffer[i] != MyFile_ReadBuffer[i])		// 校验数据
			{
				printf("校验失败，请检查SD卡或重新格式化!\r\n");
				f_close(&MyFile);	  //关闭文件	
				return ERROR;
			}
		}
		printf("校验成功，读出的数据为：\r\n");
		printf("%s\r\n",MyFile_ReadBuffer);
	}	
	else
	{
		printf("无法读取文件，请检查SD卡或重新格式化!\r\n");
		f_close(&MyFile);	  //关闭文件	
		return ERROR;		
	}	
	
	f_close(&MyFile);	  //关闭文件	
	return SUCCESS;
}
int main(void)
{
	LED_Init();			//LED初始化
	Delay_Init();		//延时函数初始化
	Usart_Config(); 	//串口初始化	
	
	FatFs_Check();			//判断FatFs是否挂载成功，若没有创建FatFs则格式化SD卡
	FatFs_GetVolume();	// 计算设备容量
	FatFs_FileTest();		//文件创建和写入测试	
	
	while (1)
	{		
		LED1_OFF; LED1_ON;	Delay_ms(150);  //点亮LED
		LED1_ON;  LED1_OFF;  Delay_ms(150);  //关闭LED
	}
}





