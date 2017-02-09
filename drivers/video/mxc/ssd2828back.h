#ifndef SSD2828_H
#define SSD2828_H

#include <asm/io.h>
#include <asm/types.h>
#include <linux/gpio.h>
#include <linux/delay.h>
// #include "8040H.h"

#define TEST_MIPI
#define TS8040H
//-------------------------//
//注:RGB和LVDS接口无法测量电流所以MIN_WARRING为0，原因ina219量程100ms以内
//#define TEST_RGB
//#define TEST_LVDS

#ifdef TS8040H
#define LED_H  "8040H.h"//"7045A.h"//"8040H.h" //"8055A.h"
static ushort MAX_WARRING = 0xFFFF;  //电流最大值
static ushort MIN_WARRING = 0x0001;  //电流最小值

//////////////////////////以上不用改///////////////////////////////////////////////////
	
#define S5PV210_LCD_WIDTH 720//720//480    分辨率大宽度 
#define S5PV210_LCD_HEIGHT 1280//1280//800  分辨率大高度
#define S5PV210_BMP_WIDTH 768  //显示内容宽度  ,一定要比分辨率大
#define S5PV210_BMP_HEIGHT 1600 //显示内容高度,一定要比分辨率大
	
#define S5PV210_LCD_CLKDIR 0x1 //1// = Direct clock,  1 = Divided by CLKVAL_F
#define S5PV210_LCD_CLKVAL_F 0x1//1// S5PV210_LCD_CLKDIR=10有效, 范围0x00-0xFF, 大幅度调整 CLK并成反比
#define S5PV210_LCD_CLK_M 0x4//4///0-0xF, 小幅度调整 CLK并成反比5
	
  /*
#define S5PV210_LCD_VBPD  60  //0x0-0xFF  06
#define S5PV210_LCD_VFPD  10  //0x0-0xFF  37
#define S5PV210_LCD_VSPW  4  //0x0-0xFF 04
#define S5PV210_LCD_HBPD  380  //0x0-0xFF 12//50 260 220ok
#define S5PV210_LCD_HFPD  20  //0x0-0xFF 12//0f  210
#define S5PV210_LCD_HSPW  4  //0x0-0xFF 04
 */
/*
#define S5PV210_LCD_VBPD  60  //0x0-0xFF  06
#define S5PV210_LCD_VFPD  10  //0x0-0xFF  37
#define S5PV210_LCD_VSPW  4  //0x0-0xFF 04
#define S5PV210_LCD_HBPD  380  //0x0-0xFF 12//50 260 220ok
#define S5PV210_LCD_HFPD  20  //0x0-0xFF 12//0f  210
#define S5PV210_LCD_HSPW  4  //0x0-0xFF 
*/
#define S5PV210_LCD_VBPD  60  //0x0-0xFF  06
#define S5PV210_LCD_VFPD  10  //0x0-0xFF  37
#define S5PV210_LCD_VSPW  4  //0x0-0xFF 04
#define S5PV210_LCD_HBPD  380  //0x0-0xFF 12//50 260 220ok
#define S5PV210_LCD_HFPD  20  //0x0-0xFF 12//0f  210
#define S5PV210_LCD_HSPW  4  //0x0-0xFF 
 
#define S5PV210_LCD_IVCLK 1   //just 0 or 1
#define S5PV210_LCD_IHSYNC 1 //just 0 or 1
#define S5PV210_LCD_IVSYNC 0 //just 0 or 1  decision
#define S5PV210_LCD_IVDEN 0  //just 0 or 1 

#endif//结束以下改

////////////以下不用改//////////////////////////////////////////////////////////
#ifndef RED

#define GRAY             (0x007F7F7F)
#define RED               (0xFF<<16)
#define GREEN             (0xFF<<8)
#define BLUE              (0xFF)
#define YELLOW              (0xFFFF00)//红+绿=黄
#define BLACK             (0x00000000)
#define WHITE             (0xffffffff)
#define FEN            (0xFF00FF) //红+蓝=粉
#define QING              (0x00FFFF) //绿+兰=青

#endif
#define Inp32(_addr)		readl(_addr)
#define Outp32(addr, data)	(*(volatile u32 *)(addr) = (data))
// #define mdelay(_a)		udelay(_a*1000)

void spi_send_data(unsigned int data);
unsigned short Read_register(void);
void Write_com(unsigned int cmd);
void Write_register(unsigned int data);
void writec16(unsigned int cmd);
void writed16(unsigned int data);
void WriteRegister(unsigned int cmd, unsigned int data);
void LCDWriteParam(int data, ...);

/*
#define S5PV210_GPE1_SET_VAL(p, v)  {Outp32(0xE0200104, (readl(0xE0200104)&(~(0x1<<(1*p))))|(v<<(1*p)));\
    Outp32(0xE0200100, (readl(0xE0200100)&(~(0xf<<(4*p))))|(1<<(4*p)));\
    Outp32(0xE0200108, (readl(0xE0200108)&(~(0x3<<(2*p))))|(2<<(2*p)));\
    Outp32(0xE020010C, (readl(0xE020010C)&(~(0x3<<(2*p))))|(3<<(2*p)));}
	*/


void initLCD();

#endif



