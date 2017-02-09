/*
 * Copyright (C) 2012 ROCKCHIP, Inc.
 *
 * author: hhb@rock-chips.com
 * create date: 2013-01-17
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */
#include <linux/fb.h>
#include <linux/delay.h>
#include <mach/gpio.h>
#include <mach/iomux.h>

#define GPIO_HIGH 1
#define GPIO_LOW  0

/* define spi gpio*/
#define SSD2828_SDC_LBA  IMX_GPIO_NR(2, 27)
#define SSD2828_SCK_CS0  IMX_GPIO_NR(2, 23)
#define SSD2828_SDI_OE   IMX_GPIO_NR(2, 25)
#define SSD2828_SDO_CS1  IMX_GPIO_NR(2, 24)
#define SSD2828_CSX0_RW  IMX_GPIO_NR(2, 26)

#define TXD_PORT        IMX_GPIO_NR(2, 25)
#define CLK_PORT        IMX_GPIO_NR(2, 23)
#define CS_PORT         IMX_GPIO_NR(2, 26)
#define RXD_PORT        IMX_GPIO_NR(2, 24)

#define CS_OUT()        gpio_direction_output(CS_PORT, 0)
#define CS_SET()        gpio_set_value(CS_PORT, GPIO_HIGH)
#define CS_CLR()        gpio_set_value(CS_PORT, GPIO_LOW)
#define CLK_OUT()       gpio_direction_output(CLK_PORT, 0)
#define CLK_SET()       gpio_set_value(CLK_PORT, GPIO_HIGH)
#define CLK_CLR()       gpio_set_value(CLK_PORT, GPIO_LOW)
#define TXD_OUT()       gpio_direction_output(TXD_PORT, 0)
#define TXD_SET()       gpio_set_value(TXD_PORT, GPIO_HIGH)
#define TXD_CLR()       gpio_set_value(TXD_PORT, GPIO_LOW)
#define TXD_INPUT()		gpio_direction_input(TXD_PORT)
#define TXD_GET()  		gpio_get_value(TXD_PORT)
#define RXD_INPUT()		gpio_direction_input(RXD_PORT)
#define RXD_GET()  		gpio_get_value(RXD_PORT)

void ssd2838_init(void) {
	int ret = 0;
	ret = gpio_request(SSD2828_SDC_LBA, "SSD2828_SDC_LBA");
	if ( ret ) {
	  printk("get SSD2828_SDC_LBA gpio FAILED!\n");
	  return ;
	}
	gpio_direction_output(SSD2828_SDC_LBA, 0);

	ret = gpio_request(SSD2828_SCK_CS0, "SSD2828_SCK_CS0");
	if ( ret ) {
	  printk("get SSD2828_SCK_CS0 gpio FAILED!\n");
	  return ;
	}
	gpio_direction_output(SSD2828_SCK_CS0, 0);

	ret = gpio_request(SSD2828_SDI_OE, "SSD2828_SDI_OE");
	if ( ret ) {
	  printk("get SSD2828_SDI_OE gpio FAILED!\n");
	  return ;
	}
	gpio_direction_output(SSD2828_SDI_OE, 0);

	ret = gpio_request(SSD2828_SDO_CS1, "SSD2828_SDO_CS1");
	if ( ret ) {
	  printk("get SSD2828_SDO_CS1 gpio FAILED!\n");
	  return ;
	}
	gpio_direction_input(SSD2828_SDO_CS1);
		
	ret = gpio_request(SSD2828_CSX0_RW, "SSD2828_CSX0_RW");
	if ( ret ) {
	  printk("get SSD2828_CSX0_RW gpio FAILED!\n");
	  return ;
	}
	gpio_direction_output(SSD2828_CSX0_RW, 1);
}

/* spi write a data frame,type mean command or data 
	3 wire 24 bit SPI interface
*/

static void spi_send_data(unsigned int data)
{
    unsigned int i;

    CS_SET();
    udelay(1);
    CLK_SET();
    TXD_SET();

    CS_CLR();
    udelay(1);

    for (i = 0; i < 24; i++)
    {
        //udelay(1); 
        CLK_CLR();
        udelay(1);
        if (data & 0x00800000) {
            TXD_SET();
        } else {
            TXD_CLR();
        }
        udelay(1);
        CLK_SET();
        udelay(1);
        data <<= 1;
    }

    TXD_SET();
    CS_SET();
}

static void spi_recv_data(unsigned int* data)
{
    unsigned int i = 0, temp = 0x73;   //read data

    CS_SET();
    udelay(1);
    CLK_SET();
    TXD_SET();

    CS_CLR();
    udelay(1);

	for(i = 0; i < 8; i++) // 8 bits Data
    {
		udelay(1); 
		CLK_CLR();
		if (temp & 0x80)
		   TXD_SET();
		else
		   TXD_CLR();
		temp <<= 1;
		udelay(1); 
		CLK_SET();
		udelay(1); 
	}
	udelay(1);
	temp = 0;
	for(i = 0; i < 16; i++) // 16 bits Data
	{
		udelay(1); 
		CLK_CLR();
		udelay(1); 
		CLK_SET();
		udelay(1); 
		temp <<= 1;
		if(RXD_GET() == GPIO_HIGH)
		   temp |= 0x01;
		
	}
    TXD_SET();
    CS_SET();
    *data = temp;
}

#define DEVIE_ID (0x70 << 16)
void send_ctrl_cmd(unsigned int cmd)
{
    unsigned int out = (DEVIE_ID | cmd );
    spi_send_data(out);
}

static void send_data_cmd(unsigned int data)
{
    unsigned int out = (DEVIE_ID | (0x2 << 16) | data );
    spi_send_data(out);
}

unsigned int ssd_read_register(unsigned int reg) {
	unsigned int data = 0;
	send_ctrl_cmd(reg);
	spi_recv_data(&data);
	return data;
}

void ssd_set_register(unsigned int reg_and_value)
{
    send_ctrl_cmd(reg_and_value >> 16);
    send_data_cmd(reg_and_value & 0x0000ffff);
}

int ssd_set_registers(unsigned int reg_array[], int n) {

	int i = 0;
	for(i = 0; i < n; i++) {
		if(reg_array[i] < 0x00b00000) {      //the lowest address is 0xb0 of ssd2828
		    if(reg_array[i] < 20000)
		    	udelay(reg_array[i]);
		    else {
		    	mdelay(reg_array[i]/1000);
		    }
		} else {
			ssd_set_register(reg_array[i]);
		}
	}
	return 0;
}

int ssd_mipi_dsi_send_dcs_packet(unsigned char regs[], u32 n) {
	//unsigned int data = 0, i = 0;
	ssd_set_register(0x00B70363);
	ssd_set_register(0x00B80000);
	ssd_set_register(0x00Bc0001);
	
	ssd_set_register(0x00Bf0000 | regs[0]);
	msleep(1);

	return 0;
}


int _ssd2828_send_packet(unsigned char type, unsigned char regs[], u32 n) {

	
	return 0;
}

int ssd2828_send_packet(unsigned char type, unsigned char regs[], u32 n) {
	return _ssd2828_send_packet(type, regs, n);
}

int ssd_mipi_dsi_read_dcs_packet(unsigned char *data, u32 n) {
	//DCS READ 
	unsigned int i = 0;
	
	i = ssd_read_register(0xc6);
	printk("read mipi slave error:%04x\n", i);
	ssd_set_register(0x00B70382);
	ssd_set_register(0x00BB0008);
	ssd_set_register(0x00C1000A);
	ssd_set_register(0x00C00001);
	ssd_set_register(0x00Bc0001);
	ssd_set_register(0x00Bf0000 | *data);
	msleep(10);
	i = ssd_read_register(0xc6);
	printk("read mipi slave error:%04x\n", i);
	
	if(i & 1) {
		i = ssd_read_register(0xff);
		printk("read %02x:%04x\n", *data, i);
		i = ssd_read_register(0xff);
		printk("read %02x:%04x\n", *data, i);
		i = ssd_read_register(0xff);
		printk("read %02x:%04x\n", *data, i);
	
	} 
		
	return 0;
}


int ssd2828_get_id(void) {
	
	int id = -1;
	id = ssd_read_register(0xb0);
	
	return id;
}


