#ifndef __TESTLED_H
#define __TESTLED_H

#include "ssd2828.h"
#include <common.h>
//#include "android_logo.h"
#include <i2c.h>

#define Delay(_a)		udelay(_a*1000)


#define CFG_I2C_SPEED 100000
#define CFG_I2C_SLAVE 0x40


unsigned short run_timer = 0;
unsigned short mod = 7;
unsigned char first_run = 1;
//32?辰?℅
unsigned short black_white_grads[32]=  {0x0000,0x0841,0x1082,0x18c3,0x2104,0x2945,0x3186,0x39c7,
                                                  0x4208,0x4a49,0x528a,0x5acb,0x630c,0x6b4d,0x738e,0x7bcf,
                                                  0x8430,0x8c71,0x94b2,0x9cf3,0xa534,0xad75,0xb5b6,0xbdf7,
                                                  0xc638,0xce79,0xd6ba,0xdefb,0xe73c,0xef7d,0xf7be,0xffff};	  //o迆～℅32?辰?℅那y?

void GRAY_32(void)
{
	//return;
	int i = 0, j = 0, k =0;
	u32* pBuffer = (u32*)(CFG_LCD_FBUFFER);	
	for (i = 0; i < S5PV210_LCD_HEIGHT; i++)
	{
		for (j = 0; j < S5PV210_LCD_WIDTH; j++)
		{
			*pBuffer = (k << 16) | (k << 8) | k;
			pBuffer++;
		}
		if (i %(S5PV210_LCD_HEIGHT/32) == 0){
				k+=8;
				k == 256?k=255:k;
			}
	}
}

//Kingsley readBMP
int getPictureNum()
{
	char filename[32];
	block_dev_desc_t *dev_desc=NULL;
	dev_desc = get_dev("mmc", 0);
	if (dev_desc == NULL) {
		printf ("** Invalid boot device **\n");
		return 0;
	}
	if (fat_register_device(dev_desc, 1) != 0) {
		printf ("** Invalid partition **\n");
		return 0;
	}
	int i =0, size;
	unsigned long offset = CFG_FASTBOOT_TRANSFER_BUFFER;
	unsigned long count = 0;
	for (i = 0; i < 20; ++i)
	{
		sprintf(filename, "/lcdstar%d.bmp", i);
		size = file_fat_read (filename, (unsigned char *) offset, count);
		if (size == -1)
			return i;
	}	
	return i;
}


int fill_picture(int index)
{
	long size;
	unsigned long offset;
	unsigned long count;
	char filename[32];
	block_dev_desc_t *dev_desc=NULL;
	//printf ("** readBMP **\n");
	dev_desc = get_dev("mmc", 0);
	if (dev_desc == NULL) {
		printf ("** Invalid boot device **\n");
		return -1;
	}
	if (fat_register_device(dev_desc, 1) != 0) {
		printf ("** Invalid partition **\n");
		return -1;
	}

	//sprintf(filename, "%s/%s", "/sdfuse", file);
	sprintf(filename, "/lcdstar%d.bmp", index - 6);
	//printf(filename);
	offset = CFG_FASTBOOT_TRANSFER_BUFFER;
	count = 0;
	size = file_fat_read (filename, (unsigned char *) offset, count);
    if (size != -1) {
		unsigned char * file = (unsigned char *) offset;
		//printf("bmp=%x\n", *file);
		file += 0xa;
		int seek = *(file) + (*(file+1) << 8) + (*(file+2) << 16) + (*(file+3) << 24);
		//printf("offset=%d\n", seek);
		file += 8;
		int width = *(file) + (*(file+1) << 8) + (*(file+2) << 16) + (*(file+3) << 24);
		printf("width=%d\n",width);
		file += 4;
		int height = *(file) + (*(file+1) << 8) + (*(file+2) << 16) + (*(file+3) << 24);
		printf("height=%d\n",height);
		int size = ((width * 3 + 3) / 4) * 4;  //㊣?D?那?4℅??迆米???那y
		//printf("size=%d\n",size);
		
		int pos = seek + size * (height - 1);   //∩車℅?o車辰?DD?a那?豕?那y
		//printf("pos=%d\n",pos);
		short blue,green,red,cnt;
		u32* pBuffer = (u32*)(CFG_LCD_FBUFFER);
		int i = 0;
		while(pos >= 0)
		{
			file = (unsigned char *) offset;
			file += pos;
			pBuffer = (u32*)(CFG_LCD_FBUFFER)+S5PV210_LCD_WIDTH*(i++);
			for(cnt = 0; cnt < width; cnt++)
			{
				blue = *(file++);
				green =*(file++);
				red = *(file++);
	            //printf("0x%02x%02x%02x%s",blue,green,red,",\n");
				*pBuffer = (red << 16) | (green << 8) | blue;
				++pBuffer;
			}
			pos -= size;
	    }
		printf ("** readBMP OK **\n");
		return run_timer;
    }
	return -1;
}

/*void fill_picture_s (unsigned long base)
{
  int i = 0, j = 0;
  u32* pBuffer = (u32*)(CFG_LCD_FBUFFER);
  for (i = 0; i < 140; i++)
	{
	    //pBuffer = (u32*)(CFG_LCD_FBUFFER)+S5PV210_LCD_HEIGHT*(352+i)+472;
	    //for (j = 0; j < 433; j++)
	    pBuffer = (u32*)(CFG_LCD_FBUFFER)+S5PV210_LCD_WIDTH*(20+i)+24;
		for (j = 0; j < 199; j++)
		{
		    //Kingsley
			*pBuffer = android_raw565[i][j];
			pBuffer++;
			//printf("0x%08x\n",pBuffer);
		}
	}
  
}*/

void fill_frame (long colour)
{
	int i = 0,j = 0;
	u32* pBuffer = (u32*)(CFG_LCD_FBUFFER);
		
		for (i = 0; i < S5PV210_LCD_HEIGHT; i++)
		{
			//pBuffer = (u32*)(CFG_LCD_FBUFFER)+S5PV210_LCD_WIDTH*(i);
			for (j = 0; j < S5PV210_LCD_WIDTH; j++)
			{
				//if (i == 0 || j == 0 || i == S5PV210_LCD_HEIGHT - 1 || j == S5PV210_LCD_WIDTH - 1)
				//	*pBuffer = 0x00ff00;
				//else
					*pBuffer = colour;
				pBuffer++;
			}
		}
}

static s3c_cable_changed_bat()  
{
	//Outp32(0xE0200284,readl(0xE0200284) &= ~(1<<5)); //GPJ2_5 ?????a那?3???米赤
	run_timer %= mod;
	switch(run_timer)
    {  
	    case 0:
			fill_frame (WHITE);
            break;
        case 1:
			BEGIN:
            fill_frame (RED);
			break;
        case 2:
            fill_frame (GREEN);
    	  //area(S5PV210_LCD_HEIGHT/4,(S5PV210_LCD_HEIGHT/4)*3,S5PV210_LCD_WIDTH/4,(S5PV210_LCD_WIDTH/4)*3,WHITE);
            break;
        case 3:
            fill_frame (BLUE);
            break;
        case 4:
    	    fill_frame (BLACK);
            break;
        case 5:
    	    GRAY_32();
    	    //fill_frame (YELLOW);
	        break;
        /*case 6:
    	    fill_frame ( WHITE);
	        break;
        case 7:
			fill_frame ( QING);			
			//GRAY_32();
            break;*/
        default:
			{ 
				printf("mod=%d, run_timer =%d\n", mod, run_timer);
				//fill_frame ( BLACK);
	            if (fill_picture (run_timer) > 0)
            	{
            		if (first_run)
        			{
        				printf("mod++\n");
						++mod;
						++run_timer;
						if (mod < 6)
							mod = 6;
						first_run = 0;
        			}
            	}
				else
				{
						printf("mod--\n");
						--mod;
					goto BEGIN;
				}
	            break;
        	}
    }
	Ina219_Probe();
	++run_timer;
	//return IRQ_HANDLED;    
}


ushort readValues(uint addr)
{
	unsigned char data[2];
	int a = i2c_probe(CFG_I2C_SLAVE);
	if (a == 0) {
		//printf("readValues i2c_probe ok \n");
		if (i2c_read(CFG_I2C_SLAVE, addr, 1, data, 2) == 0) {
			//printf("I2C read: 0x%x \n",(data[0] << 8) | data[1]);
			return (data[0] << 8) | data[1];
		}
	}
	else printf("i2c_probe I2C write:failed %d \n",a);

	return 0;
}

int wirteValues(uint addr, ushort data)
{
	//printf("I2C write: 0x%x \n",data);
	if (i2c_write(CFG_I2C_SLAVE, addr, 1, data, 2) != 0)
	{
		return 1;
	}
		return 0;

}
void readRegedit()
{
    ushort config_val =  readValues(0x0);

    printf("read config 0x%04x\n", config_val);	
    config_val =  readValues(0x1);
    printf("read Shunt Voltage 0x%04x\n", config_val);	
    config_val =  readValues(0x2);
    printf("read Bus Voltage 0x%04x\n", config_val);	
    config_val =  readValues(0x3);
    printf("read Power 0x%04x\n", config_val);	
    config_val =  readValues(0x4);
    printf("read Current 0x%04x\n", config_val);	
    config_val =  readValues(0x5);
    printf("read Calibration 0x%04x\n", config_val);	
    printf("-----------------------------------------------------\n", config_val);	
}

void Ina219_Probe()
{
	//readRegedit();
     
	ushort i = 0, val = 0, max = 0, min = 0, sum = 0;
	val =  readValues(0x4);
	min = max = val;
	for (i = 0; i < 6; i++)
	{
		val =  readValues(0x4);
		if (val < min)
			min = val;
		if (val > max)
			max = val;
		sum += val;
	}
	val =  (sum - max -min)/4;
    if (val >= MAX_WARRING || val <= MIN_WARRING)
    {
    	printf("+++++++++++++++++val=0x%04x+++++++++++++++++\n",val);
        //㊣“?‘ GPJ2_5㊣“?‘米?
       	Outp32(0xE0200284,readl(0xE0200284)|= (1<<5)); //GPJ2_5 ?????a那?3?????
       	//mdelay(500);
		//Outp32(0xE0200284,readl(0xE0200284) &= ~(1<<5));
    }
	else
	{
		//printf("---------------------val=0x%04x-----------------\n",val);
		Outp32(0xE0200284,readl(0xE0200284) &= ~(1<<5));
	}
}
void Ina219_Init()
{
	//return;
	printf("Ina219_Init \n");
	Outp32(0xE02000C0,0);
	Outp32(0xE02000C0,readl(0xE02000C0)|0x20000); //GPD1_4 ?????aI2C2_SDA
	Outp32(0xE02000C0,readl(0xE02000C0)|0x200000); //GPD1_5 ?????aI2C2_SCL

	//Outp32(0xE02000C0,readl(0xE02000C0)|0x110000); //GPD1_4 GPD1_5?????a那?3?
	Outp32(0xE02000C4,readl(0xE02000C4)|= (0x11<<4)); //GPD1_4 GPD1_5?????a那?3?????

	//???‘米?
	Outp32(0xE0200280,readl(0xE0200280)|0x100000); //GPJ2_5 ?????a那?3?
	Outp32(0xE0200284,readl(0xE0200284) &= ~(1<<5)); //GPJ2_5 ?????a那?3???米赤
	//﹞??迄??
	Outp32(0xE02000A0,readl(0xE02000A0)|0x2); //GPD0_0 ?????aPWM0
	
	I2C_SET_BUS(2);
	i2c_init(CFG_I2C_SPEED, CFG_I2C_SLAVE);
	//printf("[0xE02000C0]=%08lx\n", Inp32(0xE02000C0)&0xffffffff); //i2c2
	//readRegedit();
	mdelay(50);
    wirteValues(0x00, 0x119f); // Config 0x019f
    wirteValues(0x05, 0x5000); // Config 0x019f
    //readRegedit();
}


int autoMode = 1; //℅??‘?‘??那?1㊣赤那?℅??‘??DD
#define AUTOMODE (readl(0xE0200c44) == 8)
#define STEPMODE (readl(0xE0200c44) == 32)
void TestLED()
{
    printf("----------------------------enter TestLED-------------------------\n");
	mod = getPictureNum() + 6;
	first_run = 0;
    /*set_irq_type(IRQ_REFLASH_BAT, IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING);
     int ret = request_irq(IRQ_REFLASH_BAT, s3c_cable_changed_bat,IRQF_DISABLED,"IRQ_REFLASH_BAT",&count);                  //XEINT20
        if(ret)                  
        {
            printf("%s: could not allocate ts IRQ_ACIN !\n", __func__);
            return;
        }*/
     
    while(1)
    {
    	
    	if (autoMode == 1)
		{
			//printf("---Waiting for Press KEY! %d---\n", autoMode);
			if (STEPMODE){//米ㄓ2??㏒那?
		        autoMode = 0;
			}
			mdelay(500);
			s3c_cable_changed_bat();
			if (STEPMODE){//米ㄓ2??㏒那?
		        autoMode = 0;
			}
			mdelay(500);
		}
		else
		{
			//printf("+++++Waiting for Press KEY! %d++++\n", autoMode);
			while(!AUTOMODE && !STEPMODE){//米ㄓ2??㏒那?
		        mdelay(50);
			}
			if (AUTOMODE)
			{
				autoMode = 1;
				continue;
			}
	        s3c_cable_changed_bat();
			while(readl(0xE0200c44) != 0){}
			mdelay(100);
		}
    }

}

#endif

