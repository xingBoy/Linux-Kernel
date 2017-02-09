
#include "ssd2828.h"

#include LED_H

//#define BUILD_UBOOT
#ifdef BUILD_UBOOT
#define printk printf
#else
#define LCM_DEBUG printk
#endif 

#define Bridge_IC_ID      0x2828 


/*
MX6DL_PAD_EIM_CS0__GPIO_2_23,
MX6DL_PAD_EIM_CS1__GPIO_2_24,
MX6DL_PAD_EIM_LBA__GPIO_2_27,
MX6DL_PAD_EIM_OE__GPIO_2_25,
MX6DL_PAD_EIM_RW__GPIO_2_26,
MX6DL_PAD_KEY_ROW1__GPIO_4_9,

EIM_CS0__GPIO_2_23,
EIM_CS1__GPIO_2_24,
EIM_LBA__GPIO_2_27,
EIM_OE__GPIO_2_25,
EIM_RW__GPIO_2_26,
*/
#define SSD2828_SDC_LBA  IMX_GPIO_NR(2, 27)
#define SSD2828_SCK_CS0  IMX_GPIO_NR(2, 23)
#define SSD2828_SDI_OE   IMX_GPIO_NR(2, 25)
#define SSD2828_SDO_CS1  IMX_GPIO_NR(2, 24)
#define SSD2828_CSX0_RW  IMX_GPIO_NR(2, 26)
#define SSD2828_RESET    IMX_GPIO_NR(4, 9)

#define SET_LSCE_LOW   (gpio_set_value(SSD2828_CSX0_RW, 0))
#define SET_LSCE_HIGH  (gpio_set_value(SSD2828_CSX0_RW, 1))
#define SET_LSCK_LOW   (gpio_set_value(SSD2828_SCK_CS0, 0))
#define SET_LSCK_HIGH  (gpio_set_value(SSD2828_SCK_CS0, 1))
#define SET_LSDA_LOW   (gpio_set_value(SSD2828_SDI_OE, 0))
#define SET_LSDA_HIGH  (gpio_set_value(SSD2828_SDI_OE, 1))
#define RXD_INPUT()	   (gpio_direction_input(SSD2828_SDO_CS1))
#define RXD_GET()	   (gpio_get_value(SSD2828_SDO_CS1))

#define HX_WR_COM       (0x70)
#define HX_WR_REGISTER  (0x72)
#define HX_RD_REGISTER  (0x73)


// ---------------------------------------------------------------------------
//  Local Functions
// ---------------------------------------------------------------------------
 void spi_send_data(unsigned int data)
{
    //printk("spi_send_data \n");
	unsigned int i;
    SET_LSCE_HIGH;
	SET_LSCK_HIGH;
    SET_LSDA_LOW;
    udelay(10);
    SET_LSCE_LOW;
    udelay(10);
    for (i = 0; i < 24; ++i)
    {
        if (data & (0x800000)) {//   1 << 23
            SET_LSDA_HIGH;
        } else {
            SET_LSDA_LOW;
        }
        udelay(5);
        SET_LSCK_LOW;
        udelay(9);
        SET_LSCK_HIGH;
        //udelay(10);
        data <<= 1;
    }
    SET_LSDA_LOW;
    SET_LSCE_HIGH;
}


 unsigned short Read_register(void)
{
    //printk("Read_register Enter\n");
    unsigned char i,j,front_data;
    unsigned short value = 0;
    front_data=HX_RD_REGISTER;
    SET_LSCE_HIGH;
    SET_LSCK_HIGH;
    SET_LSDA_LOW;
    udelay(10);
    SET_LSCE_LOW;
    udelay(10); 
    for(i=0;i<8;i++) // 8  Data
    {
		if ( front_data& 0x80){
           SET_LSDA_HIGH;
        }
        else{
           SET_LSDA_LOW;
        }
		front_data<<= 1; 
        udelay(9);
        SET_LSCK_LOW;
        udelay(9); 
        SET_LSCK_HIGH;
        udelay(9); 
    }
    mdelay(1); 
    for(j=0;j<16;j++) // 16 Data
    {
		SET_LSCK_HIGH;         
        udelay(9); 
        SET_LSCK_LOW;
		value<<= 1;    
		if(RXD_GET() == 1)
			value |= 0x01;
        //printk("Read_register value=%x,GET_HX_SDI=%x\n", value, RXD_GET());
        udelay(9);

		    
		}
       	SET_LSCE_HIGH;
       	//printk("Read_register exit\n");
       	return value;      
    }

 void Write_com(unsigned int cmd)
{
    unsigned int out = ((HX_WR_COM<<16) | (cmd & 0xFFFF));
    spi_send_data(out);
}
 void Write_register(unsigned int data)
{
    unsigned int out = ((HX_WR_REGISTER<<16) |(data & 0xFFFF));
    spi_send_data(out);
}

 void writec16(unsigned int cmd)
{
	Write_com(cmd);
}



void writed16(unsigned int data)
{
    //int tmp = ((data & 0x00FF) << 8)| ((data & 0xFF00) >> 8);
    Write_register(data);
}

void WriteRegister(unsigned int cmd, unsigned int data)
{
    Write_com(cmd);
    Write_register(data);
}

void LCDWriteParam(int data, ...)
{

	int temp[60] = {0};
    unsigned char i,num = 0;
	int j = data;
	va_list argptr;  
    va_start(argptr, data);
	temp[num++] = j;
	//printk("+++++0x%02x\n", temp[num-1]&0xFF);
    while (j != -1)  
    {
        j = va_arg(argptr, int);
		if (j != -1)
		{
			temp[num++] = j;
			//printk("+++++0x%02x\n", j);
		}
		else
			break;
    }  
    va_end(argptr);  
	//printk("==============%d\n", num);
	if (num <=0 || num > 60)
		return;
	writec16(0xBC);
	writed16(num);
	writec16(0xBF);
	for (i = 0; i < num;)
	{
		if ((i+2) <= num) //双数
		{
			writed16(((temp[i+1]&0xFF) << 8) | (temp[i]&0xFF));
			//printk("*******0x%04x\n", ((temp[i+1]&0xFF) << 8) | (temp[i]&0xFF));
		}
		else
		{
			writed16((temp[i]&0xFF));
			//printk("######0x%04x\n", temp[i]&0xFF);
		}
		i+=2;
	}
}



// ---------------------------------------------------------------------------

//  LCM Driver Implementations

// ---------------------------------------------------------------------------

 void config_gpio(void)

{
    printk("config_gpio enter\n");
    //lcm_util.set_gpio_mode(LSCE_GPIO_PIN, GPIO_DISP_LSCE_PIN_M_GPIO);
    // S5PV210_GPE0_SET_VAL(4, 1);
    // S5PV210_GPE0_SET_VAL(5, 0);
	// S5PV210_GPE0_SET_VAL(7, 0);
	//S5PV210_GPE0_SET_VAL(6, 0);

    printk("config_gpio exit\n");

} 

 
/*
 unsigned int lcm_compare_id(void)

{
    unsigned short id,j,value;
    spi_send_data(0x00b0);    
    spi_send_data(0x00fa);
    
    for(j=0;j<16;j++) // 16 Data
    {
      SET_LSCK_HIGH;
      udelay(10); 
      SET_LSCK_LOW;
      id<<= 1;
      id |= GET_HX_SDI;
      printk("Read_register value=%x,GET_HX_SDI=%x\n",value,GET_HX_SDI);
      udelay(10);   
    }
    SET_LSCE_HIGH;
    printk("lcm_compare_id id = 0x%x  exit\n", id);
        
    return (Bridge_IC_ID == id)?id:0; 
}
*/
extern int ssd2828_get_id(void);
unsigned int ssd_read_register(unsigned int reg);
extern void lcm_init(void)
{ 
	int ret;
    printk("lcm_init enter\n");

	/*
	#define SSD2828_SDC_LBA  IMX_GPIO_NR(2, 27)
	#define SSD2828_SCK_CS0  IMX_GPIO_NR(2, 23)
	#define SSD2828_SDI_OE   IMX_GPIO_NR(2, 25)
	#define SSD2828_SDO_CS1  IMX_GPIO_NR(2, 24)
	#define SSD2828_CSX0_RW  IMX_GPIO_NR(2, 26)
	*/
	ret = gpio_request(SSD2828_SDC_LBA, "SSD2828_SDC_LBA");
	if ( ret ) {
	  printk("get SSD2828_SDC_LBA gpio FAILED!\n");
	  return ret;
	}
	gpio_direction_output(SSD2828_SDC_LBA, 0);

	ret = gpio_request(SSD2828_SCK_CS0, "SSD2828_SCK_CS0");
	if ( ret ) {
	  printk("get SSD2828_SCK_CS0 gpio FAILED!\n");
	  return ret;
	}
	gpio_direction_output(SSD2828_SCK_CS0, 0);

	ret = gpio_request(SSD2828_SDI_OE, "SSD2828_SDI_OE");
	if ( ret ) {
	  printk("get SSD2828_SDI_OE gpio FAILED!\n");
	  return ret;
	}
	gpio_direction_output(SSD2828_SDI_OE, 0);

	ret = gpio_request(SSD2828_SDO_CS1, "SSD2828_SDO_CS1");
	if ( ret ) {
	  printk("get SSD2828_SDO_CS1 gpio FAILED!\n");
	  return ret;
	}
	//gpio_direction_output(SSD2828_SDO_CS1, 0);
	RXD_INPUT(); // as input pin
		
	ret = gpio_request(SSD2828_CSX0_RW, "SSD2828_CSX0_RW");
	if ( ret ) {
	  printk("get SSD2828_CSX0_RW gpio FAILED!\n");
	  return ret;
	}
	gpio_direction_output(SSD2828_CSX0_RW, 1);

	ret = gpio_request(SSD2828_RESET, "SSD2828_RESET");
	if ( ret ) {
	  printk("get SSD2828_CSX0_RW gpio FAILED!\n");
	  return ret;
	}
	gpio_direction_output(SSD2828_RESET, 1);
    msleep(1);
    gpio_set_value(SSD2828_RESET, 0);
    msleep(10);
    gpio_set_value(SSD2828_RESET, 1);

	/*
	//LCD电源开关
	Outp32(0xE0200280,readl(0xE0200280)|0x1000000); //GPJ2_6 配置为输出
	Outp32(0xE0200284,readl(0xE0200284) |= (1<<6)); //GPJ2_6 配置为输出脚高
	
	//GPH0_4 大电流检测脚
	Outp32(0xE0200C00,readl(0xE0200C00)& 0xFFF0FFFF); // GPH0_4配置为输入脚
	Outp32(0xE0200C08,readl(0xE0200C08)& 0xFFBF); // GPH0_4配置为高

	#ifdef TEST_MIPI
	Outp32(0xE0200280,readl(0xE0200280)|0x100);
    Outp32(0xE0200280+4,(Inp32(0xE0200280+4) |=(1<<2))); //GPJ2_2 High 灯
    
	//Outp32(0xE02000E0,readl(0xE02000E0)& 0xF0FFFFFF); // GPE0_6配置为输入脚
	//Outp32(0xE02000E8,readl(0xE02000E8)& 0xFFFF); // GPE0_6配置为高
		
	S5PV210_GPE1_SET_VAL(2, 1);//背光
    S5PV210_GPE1_SET_VAL(0,0);//LCD复位
	S5PV210_GPH1_SET_VAL(6, 0);//MIPI开关
	S5PV210_GPH1_SET_VAL(2, 1); //RGB开关
	//LVDS开关
	Outp32(0xE02002A0,readl(0xE02002A0)|0x10000000); //GPJ3_7 配置为输出
	Outp32(0xE02002A4,readl(0xE02002A4) |= (1<<7)); //GPJ3_7 配置为输出脚高 
	*/


     unsigned short id;
     config_gpio(); 
     SET_LSCK_HIGH;
     mdelay(25);
     SET_LSCK_LOW;
     mdelay(25);
     SET_LSCK_HIGH;

	 /*
    S5PV210_GPE0_SET_VAL(3,1);//SSD2828复位
    mdelay(10);
    S5PV210_GPE0_SET_VAL(3,0);

    mdelay(20);
    S5PV210_GPE0_SET_VAL(3,1);
    mdelay(50);

    S5PV210_GPE1_SET_VAL(0,1);

	*/

	
	//Write_com(0x00b0);
	//id = Read_register();
    //printk("Kernel--SSD2828 id is: %x\n",id);
	printk("Kernel--SSD2828 id is: %x\n",ssd2828_get_id());

	 // use for check pin
	 // int i = 0;
	 // for(i = 0; i < 1000; i++ ) {
	 //    //#define SSD2828_SDC_LBA  IMX_GPIO_NR(2, 27)
	 //    //#define SSD2828_SCK_CS0  IMX_GPIO_NR(2, 23)
	 //    //#define SSD2828_SDI_OE   IMX_GPIO_NR(2, 25)
	 //    //#define SSD2828_SDO_CS1  IMX_GPIO_NR(2, 24)
	 //    //#define SSD2828_CSX0_RW  IMX_GPIO_NR(2, 26)
	 //     gpio_set_value(SSD2828_CSX0_RW, i%2);
	 //     mdelay(10);
	 // }
	
	/*
	#ifdef TEST_RGB

	Outp32(0xE0200280,readl(0xE0200280)|0x1000);
    Outp32(0xE0200280+4,(Inp32(0xE0200280+4) |=(1<<3))); //GPJ2_3 High 灯
	//#define S5PV210_GPH1_SET_VAL(p, v) {Outp32(0xE0200C24, (readl(0xE0200C24)&(~(0x1<<(1*p))))|(v<<(1*p)));\
	//		Outp32(0xE0200C20, (readl(0xE0200C20)&(~(0x1<<(4*p))))|(1<<(4*p)));}
	//GPH1_2 RGB信号开关
	//Outp32(0xE0200C20,readl(0xE0200C20)|0x100); //GPH1_2 配置为输出
	//Outp32(0xE0200C24,readl(0xE0200C24) &= ~(1<<2)); //GPH1_2 配置为输出脚低
	S5PV210_GPH1_SET_VAL(2, 0);//RBG开关0-开1-关
	S5PV210_GPH1_SET_VAL(6, 1);//MIPI开关0-开1-关
	
	//GPJ3_1 RBG 复位
	Outp32(0xE02002A0,readl(0xE02002A0)|0x10); //GPJ3_1 配置为输出
	Outp32(0xE02002A4,readl(0xE02002A4) |= (1<<1)); //GPJ3_1 配置为输出脚高	
	
	//GPJ4_3 RBG电源开关
	Outp32(0xE02002C0,readl(0xE02002C0)|0x1000); //GPJ4_3 配置为输出
	Outp32(0xE02002C4,readl(0xE02002C4) |= (1<<3)); //GPJ4_3 配置为输出脚高	
	//GPH0_5 RBG电源开关
	Outp32(0xE0200C00,readl(0xE0200C00) | 0x100000); // GPH0_5配置为输出脚
	Outp32(0xE0200C04,readl(0xE0200C04) |= (1<<5)); // GPH0_5配置为高
	//LVDS开关
	Outp32(0xE02002A0,readl(0xE02002A0)|0x10000000); //GPJ3_7 配置为输出
	Outp32(0xE02002A4,readl(0xE02002A4) |= (1<<7)); //GPJ3_7 配置为输出脚高	

	#endif

	#ifdef TEST_LVDS
	Outp32(0xE0200280,readl(0xE0200280)|0x10000);
	Outp32(0xE0200280+4,(Inp32(0xE0200280+4) |=(1<<4))); //GPJ2_4 High 灯

	//LVDS开关
	Outp32(0xE02002A0,readl(0xE02002A0)|0x10000000); //GPJ3_7 配置为输出
	Outp32(0xE02002A4,readl(0xE02002A4) &= ~(1<<7)); //GPJ3_7 配置为输出脚低

	//GPJ4_3 RBG电源开关  1-开0-关
	Outp32(0xE02002C0,readl(0xE02002C0)|0x1000); //GPJ4_3 配置为输出
	Outp32(0xE02002C4,readl(0xE02002C4) |= (1<<3)); //GPJ4_3 配置为输出脚高 
	//GPH0_5 RBG电源开关1-开0-关
	Outp32(0xE0200C00,readl(0xE0200C00) | 0x100000); // GPH0_5配置为输出脚
	Outp32(0xE0200C04,readl(0xE0200C04) |= (1<<5)); // GPH0_5配置为高

	S5PV210_GPH1_SET_VAL(2, 1);//RBG开关0-开1-关
	S5PV210_GPH1_SET_VAL(6, 1);//MIPI开关0-开1-关

	#endif
	*/
    initLCD();   
} 

