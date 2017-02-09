/*
 * Copyright (C) 2011-2012 Freescale Semiconductor, Inc. All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <linux/types.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/err.h>
#include <linux/clk.h>
#include <linux/console.h>
#include <linux/io.h>
#include <linux/bitops.h>
#include <linux/spinlock.h>
#include <linux/mxcfb.h>
#include <linux/backlight.h>
#include <video/mipi_display.h>

#include <mach/hardware.h>
#include <mach/clock.h>
#include <mach/mipi_dsi.h>
#include <linux/gpio.h>

#include "../../video/mxc/mipi_dsi.h"

#define MIPI_DSI_MAX_RET_PACK_SIZE	(0x4)

#define HX8389BL_MAX_BRIGHT		(255)
#define HX8389BL_DEF_BRIGHT		        (255)

#define HX8389_MAX_DPHY_CLK		(800)
#define HX8389_ONE_DATA_LANE		(0x01)
#define HX8389_TWO_DATA_LANE		(0x02)


#define HX8389B_540x960   1
#define HX8389C_480x1120 2

#define HX8389_VERSION HX8389C_480x1120  //select your display panel version.

#define HX8389_CMD_SETEXTC			      (0xB9)
#define HX8389_CMD_SETEXTC_LEN		      (0x04)
#define HX8389_CMD_SETEXTC_PARAM_1      (0x8983ff)

#define HX8389_CMD_GETHXID				(0x04)
#define HX8389_CMD_GETHXID_LEN			(0x4)
#define HX8389_ID						(0x69)
#define HX8389_ID_MASK					(0xFF)

#if HX8389_VERSION==HX8389B_540x960
#define HX8389_CMD_GETHXID					(0xF4)
#define HX8389_CMD_GETHXID_LEN					(0x04)
#define HX8389_ID						(0x69)
#define HX8389_ID_MASK						(0xFF)

#define HX8389_CMD_SETDISP					(0xB2)
#define HX8389_CMD_SETDISP_LEN					(0x08)
#define HX8389_CMD_SETDISP_1_HALT				(0x00)
#define HX8389_CMD_SETDISP_2_RES_MODE				(0x00)
#define HX8389_CMD_SETDISP_3_BP					(0x78)
#define HX8389_CMD_SETDISP_4_FP					(0x04)
#define HX8389_CMD_SETDISP_5_SAP				(0x03)
#define HX8389_CMD_SETDISP_6_GENON				(0x3F)
#define HX8389_CMD_SETDISP_7_GENOFF				(0x80)


#define HX8389_CMD_SETCYC					(0xB4)
#define HX8389_CMD_SETCYC_LEN					(0x20)
#define HX8389_CMD_SETCYC_PARAM_1				(0x000C80)
#define HX8389_CMD_SETCYC_PARAM_2				(0x32001032)
#define HX8389_CMD_SETCYC_PARAM_3				(0x1032CA13)
#define HX8389_CMD_SETCYC_PARAM_4				(0x50011700)
#define HX8389_CMD_SETCYC_PARAM_5				(0x3C302301)
#define HX8389_CMD_SETCYC_PARAM_6				(0x0AFFFF14)
#define HX8389_CMD_SETCYC_PARAM_7				(0x3C305001)
#define HX8389_CMD_SETCYC_PARAM_8				(0x0AFFFF14)

#define HX8389_CMD_SETGIP					(0xD5)
#define HX8389_CMD_SETGIP_LEN					(0x31)
#define HX8389_CMD_SETGIP_PARAM_1				(0x000000)
#define HX8389_CMD_SETGIP_PARAM_2				(0x00000100)
#define HX8389_CMD_SETGIP_PARAM_3				(0x88200000)
#define HX8389_CMD_SETGIP_PARAM_4				(0x10321088)
#define HX8389_CMD_SETGIP_PARAM_5				(0x88889988)
#define HX8389_CMD_SETGIP_PARAM_6				(0x88888888)
#define HX8389_CMD_SETGIP_PARAM_7				(0x88888888)
#define HX8389_CMD_SETGIP_PARAM_8				(0x88888888)
#define HX8389_CMD_SETGIP_PARAM_9				(0x99010123)
#define HX8389_CMD_SETGIP_PARAM_10			(0x88888888)
#define HX8389_CMD_SETGIP_PARAM_11			(0x88888888)
#define HX8389_CMD_SETGIP_PARAM_12			(0x88888888)
#define HX8389_CMD_SETGIP_PARAM_13			(0x88)


#define HX8389_CMD_SETPOWER					(0xB1)
#define HX8389_CMD_SETPOWER_LEN				(0x14)
#define HX8389_CMD_SETPOWER_PARAM_1			(0x070000)
#define HX8389_CMD_SETPOWER_PARAM_2			(0x11109292)
#define HX8389_CMD_SETPOWER_PARAM_3			(0x3E36F3F3)
#define HX8389_CMD_SETPOWER_PARAM_4			(0x00422A2A)
#define HX8389_CMD_SETPOWER_PARAM_5			(0x8020F73A)

#define HX8389_CMD_SETVCOM					(0xB6)
#define HX8389_CMD_SETVCOM_LEN					(0x5)
#define HX8389_CMD_SETVCOM_PARAM_1				(0x009E00)
#define HX8389_CMD_SETVCOM_PARAM_2				(0x9E)

#define HX8389_CMD_SETDGC						(0xC1)
#define HX8389_CMD_SETDGC_LEN					(0x80)
#define HX8389_CMD_SETDGC_PARAM_1				(0x070001)
#define HX8389_CMD_SETDGC_PARAM_2				(0x3F342A1C)
#define HX8389_CMD_SETDGC_PARAM_3				(0x635B5249)
#define HX8389_CMD_SETDGC_PARAM_4				(0x7F78716A)
#define HX8389_CMD_SETDGC_PARAM_5				(0x97918B85)
#define HX8389_CMD_SETDGC_PARAM_6				(0xAFA8A39D)
#define HX8389_CMD_SETDGC_PARAM_7				(0xC9C6BEB5)
#define HX8389_CMD_SETDGC_PARAM_8				(0xE6E1D7CD)
#define HX8389_CMD_SETDGC_PARAM_9				(0x34FFF7F1)
#define HX8389_CMD_SETDGC_PARAM_10			(0x54D94FCB)
#define HX8389_CMD_SETDGC_PARAM_11			(0xC04A3CC8)
#define HX8389_CMD_SETDGC_PARAM_12			(0x2A1C0700)
#define HX8389_CMD_SETDGC_PARAM_13			(0x52493F34)
#define HX8389_CMD_SETDGC_PARAM_14			(0x716A635B)
#define HX8389_CMD_SETDGC_PARAM_15			(0x8B857F78)
#define HX8389_CMD_SETDGC_PARAM_16			(0xA39D9791)
#define HX8389_CMD_SETDGC_PARAM_17			(0xBEB5AFA8)
#define HX8389_CMD_SETDGC_PARAM_18			(0xD7CDC9C6)
#define HX8389_CMD_SETDGC_PARAM_19			(0xF7F1E6E1)
#define HX8389_CMD_SETDGC_PARAM_20			(0x4FCB34FF)
#define HX8389_CMD_SETDGC_PARAM_21			(0x3CC854D9)
#define HX8389_CMD_SETDGC_PARAM_22			(0x0700C04A)


#define HX8389_CMD_SETPANEL					(0xCC)
#define HX8389_CMD_SETPANEL_LEN				(0x02)
#define HX8389_CMD_SETPANEL_PARAM_1			(0x02)

#define HX8389_CMD_SETGAMMA					(0xE0)
#define HX8389_CMD_SETGAMMA_LEN					(0x23)
#define HX8389_CMD_SETGAMMA_PARAM_1				(0x181400)
#define HX8389_CMD_SETGAMMA_PARAM_2				(0x253F302B)
#define HX8389_CMD_SETGAMMA_PARAM_3				(0x100E0844)
#define HX8389_CMD_SETGAMMA_PARAM_4				(0x15131613)
#define HX8389_CMD_SETGAMMA_PARAM_5				(0x14001B11)
#define HX8389_CMD_SETGAMMA_PARAM_6				(0x3F302B18)
#define HX8389_CMD_SETGAMMA_PARAM_7				(0x0E084425)
#define HX8389_CMD_SETGAMMA_PARAM_8				(0x13161310)
#define HX8389_CMD_SETGAMMA_PARAM_9				(0x1B1115)

#define HX8389_CMD_SETMIPI					(0xBA)
#define HX8389_CMD_SETMIPI_LEN					(0x08)
#define HX8389_CMD_SETMIPI_PARAM_1				(0x009341)
#define HX8389_CMD_SETMIPI_PARAM_2				(0x1800A416)


#define HX8389_CMD_SETPIXEL_FMT					(0x3A)
#define HX8389_CMD_SETPIXEL_FMT_24BPP				(0x77)
#define HX8389_CMD_SETPIXEL_FMT_18BPP				(0x66)
#define HX8389_CMD_SETPIXEL_FMT_16BPP				(0x55)

#define HX8389_CMD_SETCLUMN_ADDR				(0x2A)
#define HX8389_CMD_SETCLUMN_ADDR_LEN				(5)
#define HX8389_CMD_SETCLUMN_ADDR_PARAM_1			(0xdf0000)
#define HX8389_CMD_SETCLUMN_ADDR_PARAM_2			(0x01)

#define HX8389_CMD_SETPAGE_ADDR					(0x2B)
#define HX8389_CMD_SETPAGE_ADDR_LEN				(5)
#define HX8389_CMD_SETPAGE_ADDR_PARAM_1				(0x1f0000)
#define HX8389_CMD_SETPAGE_ADDR_PARAM_2				(0x03)

#define HX8389_CMD_WRT_DISP_BRIGHT				(0x51)
#define HX8389_CMD_WRT_DISP_BRIGHT_PARAM_1			(0xFF)

#define HX8389_CMD_WRT_CABC_MIN_BRIGHT				(0x5E)
#define HX8389_CMD_WRT_CABC_MIN_BRIGHT_PARAM_1			(0x20)

#define HX8389_CMD_WRT_CABC_CTRL				(0x55)
#define HX8389_CMD_WRT_CABC_CTRL_PARAM_1			(0x1)

#define HX8389_CMD_WRT_CTRL_DISP				(0x53)
#define HX8389_CMD_WRT_CTRL_DISP_PARAM_1			(0x24)
#else
#define HX8389_CMD_SETPOWER					(0xB1)
#define HX8389_CMD_SETPOWER_LEN				(17)
#define HX8389_CMD_SETPOWER_PARAM_1			(0x14147f)//(0x14147c)
#define HX8389_CMD_SETPOWER_PARAM_2			(0xD0905511)
#define HX8389_CMD_SETPOWER_PARAM_3			(0x20809ad8)//(0x208096dc)
#define HX8389_CMD_SETPOWER_PARAM_4			(0xAAAAF820)
#define HX8389_CMD_SETPOWER_PARAM_5			(0xA3)

#define HX8389_CMD_SET_C0				        (0xC0)
#define HX8389_CMD_SET_C0_LEN                            (5)
#define HX8389_CMD_SET_C0_PARAM_1                  (0x001730)
#define HX8389_CMD_SET_C0_PARAM_2                  (0x03)

#define HX8389_CMD_SETDISP					(0xB2)
#define HX8389_CMD_SETDISP_LEN	                        (11)
#define HX8389_CMD_SETDISP_PARAM_1                (0x056488) 
#define HX8389_CMD_SETDISP_PARAM_2                (0x1128FF03)//(0x11283003)
#define HX8389_CMD_SETDISP_PARAM_3                (0x095D64)

#define HX8389_CMD_SETCYC					(0xB4)
#define HX8389_CMD_SETCYC_LEN				(12)
#define HX8389_CMD_SETCYC_PARAM_1			(0x4c504c)
#define HX8389_CMD_SETCYC_PARAM_2			(0x06504c65)//(0x06504c50)
#define HX8389_CMD_SETCYC_PARAM_3			(0xe0660766)

#define HX8389_CMD_SET_D3				        (0xD3)
#define HX8389_CMD_SET_D3_LEN                           (36)
#define HX8389_CMD_SET_D3_PARAM_1			(0x000c00)
#define HX8389_CMD_SET_D3_PARAM_2			(0x1e1e0000)
#define HX8389_CMD_SET_D3_PARAM_3			(0x00001032)
#define HX8389_CMD_SET_D3_PARAM_4			(0x03ca0300)
#define HX8389_CMD_SET_D3_PARAM_5			(0x000000ca)
#define HX8389_CMD_SET_D3_PARAM_6			(0x01112700)
#define HX8389_CMD_SET_D3_PARAM_7			(0x00002301)
#define HX8389_CMD_SET_D3_PARAM_8			(0x00080500)
#define HX8389_CMD_SET_D3_PARAM_9			(0x01000a00)

#define HX8389_CMD_SETGIP					        (0xD5)
#define HX8389_CMD_SETGIP_LEN				        (39)
#define HX8389_CMD_SETGIP_PARAM_1				(0x181818)
#define HX8389_CMD_SETGIP_PARAM_2				(0x03000118)
#define HX8389_CMD_SETGIP_PARAM_3				(0x18202102)
#define HX8389_CMD_SETGIP_PARAM_4				(0x18191918)
#define HX8389_CMD_SETGIP_PARAM_5				(0x18181818)
#define HX8389_CMD_SETGIP_PARAM_6				(0x18181818)
#define HX8389_CMD_SETGIP_PARAM_7				(0x18181818)
#define HX8389_CMD_SETGIP_PARAM_8				(0x18181818)
#define HX8389_CMD_SETGIP_PARAM_9				(0x18181818)
#define HX8389_CMD_SETGIP_PARAM_10			(0x181818)

#define HX8389_CMD_SET_D6				             (0xD6)
#define HX8389_CMD_SET_D6_LEN                                (39)
#define HX8389_CMD_SET_D6_PARAM_1                      (0x181818)
#define HX8389_CMD_SET_D6_PARAM_2                      (0x00030218)
#define HX8389_CMD_SET_D6_PARAM_3                      (0x19212001)
#define HX8389_CMD_SET_D6_PARAM_4                      (0x18181819)
#define HX8389_CMD_SET_D6_PARAM_5                      (0x18181818)
#define HX8389_CMD_SET_D6_PARAM_6                      (0x18181818)
#define HX8389_CMD_SET_D6_PARAM_7                      (0x18181818)
#define HX8389_CMD_SET_D6_PARAM_8                      (0x18181818)
#define HX8389_CMD_SET_D6_PARAM_9                      (0x18181818)
#define HX8389_CMD_SET_D6_PARAM_10                    (0x181818)

#define HX8389_CMD_SETGAMMA					(0xE0)
#define HX8389_CMD_SETGAMMA_LEN				(43)
#define HX8389_CMD_SETGAMMA_PARAM_1			(0x342F17)
#define HX8389_CMD_SETGAMMA_PARAM_2			(0x3C3F3B3A)
#define HX8389_CMD_SETGAMMA_PARAM_3			(0x0D0C0850)
#define HX8389_CMD_SETGAMMA_PARAM_4			(0x140F1218)
#define HX8389_CMD_SETGAMMA_PARAM_5			(0x12140610)
#define HX8389_CMD_SETGAMMA_PARAM_6			(0x1217172F)
#define HX8389_CMD_SETGAMMA_PARAM_7			(0x3F3B3A34)
#define HX8389_CMD_SETGAMMA_PARAM_8			(0x0C08503C)
#define HX8389_CMD_SETGAMMA_PARAM_9			(0x120F180D)
#define HX8389_CMD_SETGAMMA_PARAM_10			(0x06141214)
#define HX8389_CMD_SETGAMMA_PARAM_11			(0x171210)

#define HX8389_CMD_SET_D2					        (0xD2)
#define HX8389_CMD_SET_D2_LEN				        (2)
#define HX8389_CMD_SET_D2_PARAM_1				(0x77)


#define HX8389_CMD_SETPANEL					(0xCC)
#define HX8389_CMD_SETPANEL_LEN				(2)
#define HX8389_CMD_SETPANEL_PARAM_1			(0x02)

#define HX8389_CMD_SETVCOM					        (0xB6)
#define HX8389_CMD_SETVCOM_LEN			        (3)
#define HX8389_CMD_SETVCOM_PARAM_1			(0x5959)


#endif

#define CHECK_RETCODE(ret)					\
do {								\
	if (ret < 0) {						\
		dev_err(&mipi_dsi->pdev->dev,			\
			"%s ERR: ret:%d, line:%d.\n",		\
			__func__, ret, __LINE__);		\
		return ret;					\
	}							\
} while (0)

#if HX8389_VERSION==HX8389B_540x960
static struct fb_videomode truly_lcd_modedb[] = {
	{
	 "TRULY-WVGA", 60, 540, 960, 33333,
	 32, 32,
	 10, 13,
	 10, 4,
	 FB_SYNC_OE_LOW_ACT,
	 FB_VMODE_NONINTERLACED,
	 0,
	},
};
/*

static struct fb_videomode truly_lcd_modedb[] = {
    {
     //"TRULY-WVGA", 60, 720, 1280, 37880,
     "TRULY-WVGA", 60, 720, 1280, 17007,
     24, 12,
     2, 2,
     6, 2,
     FB_SYNC_OE_LOW_ACT,
     FB_VMODE_NONINTERLACED,
     0,
    },
};

static struct fb_videomode truly_lcd_modedb[] = {
    {
     //"TRULY-WVGA", 60, 720, 1280, 37880,
     "TRULY-WVGA", 40, 768, 1024, 19007,
     24, 12,
     2, 2,
     6, 2,
     FB_SYNC_OE_LOW_ACT,
     FB_VMODE_NONINTERLACED,
     0,
    },
};
*/
#else
static struct fb_videomode truly_lcd_modedb[] = {
	{
	 "TRULY-WVGA", 60, 480, 1120, 31250,
	 24, 24,
	 58, 63,
	 6, 4,
	 FB_SYNC_OE_LOW_ACT,
	 FB_VMODE_NONINTERLACED,
	 0,
	},
};
/*
static struct fb_videomode truly_lcd_modedb[] = {
    {
     //"TRULY-WVGA", 60, 720, 1280, 37880,
     "TRULY-WVGA", 60, 720, 1280, 17007,
     24, 12,
     2, 2,
     6, 2,
     FB_SYNC_OE_LOW_ACT,
     FB_VMODE_NONINTERLACED,
     0,
    },
};
*/
#endif

static struct mipi_lcd_config lcd_config = {
	.virtual_ch		= 0x0,
	.data_lane_num  = HX8389_TWO_DATA_LANE,
	.max_phy_clk    = HX8389_MAX_DPHY_CLK,
	.dpi_fmt		= MIPI_RGB888,
};
void mipid_hx8369_get_lcd_videomode(struct fb_videomode **mode, int *size,
		struct mipi_lcd_config **data)
{
	
	//if (cpu_is_mx6dl())
	 	//truly_lcd_modedb[0].pixclock = 25000; /* 27M clock*/
	 	//truly_lcd_modedb[0].pixclock = 37037; /* 27M clock*/

	*mode = &truly_lcd_modedb[0];
	*size = ARRAY_SIZE(truly_lcd_modedb);
	*data = &lcd_config;
}

int mipid_hx8369_lcd_setup(struct mipi_dsi_info *mipi_dsi)
{
	u32 buf[DSI_CMD_BUF_MAXSIZE];
	int err;

	
	printk("mipid_hx8369_lcd_setup MIPI DSI LCD setup.\n");

	//Set EXTC
	dev_dbg(&mipi_dsi->pdev->dev, "MIPI DSI LCD setup.\n");
	buf[0] = HX8389_CMD_SETEXTC | (HX8389_CMD_SETEXTC_PARAM_1 << 8);
	err = mipi_dsi_pkt_write(mipi_dsi, MIPI_DSI_DCS_LONG_WRITE,buf, HX8389_CMD_SETEXTC_LEN);
	CHECK_RETCODE(err);

#if 	HX8389_VERSION==HX8389B_540x960
	//Set MIPI-lanes
	/* Set MIPI: data lane number =2*/
	buf[0] = HX8389_CMD_SETMIPI | (HX8389_CMD_SETMIPI_PARAM_1 << 8);
	buf[1] = HX8389_CMD_SETMIPI_PARAM_2;
	err = mipi_dsi_pkt_write(mipi_dsi, MIPI_DSI_DCS_LONG_WRITE, buf,HX8389_CMD_SETMIPI_LEN);
	CHECK_RETCODE(err);

	//Unkonw command
	buf[0] =0x08C6;
	err = mipi_dsi_pkt_write(mipi_dsi, MIPI_DSI_DCS_LONG_WRITE, buf,0x02);
	
	//Set Power
	buf[0] = HX8389_CMD_SETPOWER | (HX8389_CMD_SETPOWER_PARAM_1 << 8);
	buf[1] = HX8389_CMD_SETPOWER_PARAM_2;
	buf[2] = HX8389_CMD_SETPOWER_PARAM_3;
	buf[3] = HX8389_CMD_SETPOWER_PARAM_4;
	buf[4] = HX8389_CMD_SETPOWER_PARAM_5;
	err = mipi_dsi_pkt_write(mipi_dsi, MIPI_DSI_DCS_LONG_WRITE, buf,HX8389_CMD_SETPOWER_LEN);
	CHECK_RETCODE(err);
	
	//Set Display
	/* set LCD resolution as 480RGBx960, DPI interface,
	 */
	buf[0] = HX8389_CMD_SETDISP | (HX8389_CMD_SETDISP_1_HALT << 8) |
			(HX8389_CMD_SETDISP_2_RES_MODE << 16) |
			(HX8389_CMD_SETDISP_3_BP << 24);
	buf[1] = HX8389_CMD_SETDISP_4_FP | (HX8389_CMD_SETDISP_5_SAP << 8) |
			 (HX8389_CMD_SETDISP_6_GENON << 16) |
			 (HX8389_CMD_SETDISP_7_GENOFF << 24);
	err = mipi_dsi_pkt_write(mipi_dsi, MIPI_DSI_DCS_LONG_WRITE,buf, HX8389_CMD_SETDISP_LEN);
	CHECK_RETCODE(err);

	//Set CYC
	/* Set display waveform cycle */
	buf[0] = HX8389_CMD_SETCYC | (HX8389_CMD_SETCYC_PARAM_1 << 8);
	buf[1] = HX8389_CMD_SETCYC_PARAM_2;
	buf[2] = HX8389_CMD_SETCYC_PARAM_3;
	buf[3] = HX8389_CMD_SETCYC_PARAM_4;
	buf[4] = HX8389_CMD_SETCYC_PARAM_5;
	buf[5] = HX8389_CMD_SETCYC_PARAM_6;
	buf[6] = HX8389_CMD_SETCYC_PARAM_7;
	buf[7] = HX8389_CMD_SETCYC_PARAM_8;
	err = mipi_dsi_pkt_write(mipi_dsi, MIPI_DSI_DCS_LONG_WRITE,buf, HX8389_CMD_SETCYC_LEN);
	CHECK_RETCODE(err);

	//Set GIP
	/* Set GIP timing output control */
	buf[0] = HX8389_CMD_SETGIP | (HX8389_CMD_SETGIP_PARAM_1 << 8);
	buf[1] = HX8389_CMD_SETGIP_PARAM_2;
	buf[2] = HX8389_CMD_SETGIP_PARAM_3;
	buf[3] = HX8389_CMD_SETGIP_PARAM_4;
	buf[4] = HX8389_CMD_SETGIP_PARAM_5;
	buf[5] = HX8389_CMD_SETGIP_PARAM_6;
	buf[6] = HX8389_CMD_SETGIP_PARAM_7;
	buf[7] = HX8389_CMD_SETGIP_PARAM_8;
	buf[8] = HX8389_CMD_SETGIP_PARAM_9;
	buf[9] = HX8389_CMD_SETGIP_PARAM_10;
	buf[10] = HX8389_CMD_SETGIP_PARAM_11;
	buf[11] = HX8389_CMD_SETGIP_PARAM_12;
	buf[12] = HX8389_CMD_SETGIP_PARAM_13;
	err = mipi_dsi_pkt_write(mipi_dsi, MIPI_DSI_DCS_LONG_WRITE, buf,HX8389_CMD_SETGIP_LEN);
	CHECK_RETCODE(err);

	//Set VCOM
	buf[0] = HX8389_CMD_SETVCOM | (HX8389_CMD_SETVCOM_PARAM_1 << 8);
	buf[1] = HX8389_CMD_SETVCOM_PARAM_2;
	err = mipi_dsi_pkt_write(mipi_dsi, MIPI_DSI_DCS_LONG_WRITE, buf,HX8389_CMD_SETVCOM_LEN);
	CHECK_RETCODE(err);

	//Set GAMMA
	/* Set gamma curve related setting */
	buf[0] = HX8389_CMD_SETGAMMA | (HX8389_CMD_SETGAMMA_PARAM_1 << 8);
	buf[1] = HX8389_CMD_SETGAMMA_PARAM_2;
	buf[2] = HX8389_CMD_SETGAMMA_PARAM_3;
	buf[3] = HX8389_CMD_SETGAMMA_PARAM_4;
	buf[4] = HX8389_CMD_SETGAMMA_PARAM_5;
	buf[5] = HX8389_CMD_SETGAMMA_PARAM_6;
	buf[7] = HX8389_CMD_SETGAMMA_PARAM_7;
	buf[7] = HX8389_CMD_SETGAMMA_PARAM_8;
	buf[8] = HX8389_CMD_SETGAMMA_PARAM_9;
	err = mipi_dsi_pkt_write(mipi_dsi, MIPI_DSI_DCS_LONG_WRITE, buf,HX8389_CMD_SETGAMMA_LEN);
	CHECK_RETCODE(err);

	//Set Panel
	/* Set Panel: BGR/RGB or Inversion. */
	buf[0] = HX8389_CMD_SETPANEL | (HX8389_CMD_SETPANEL_PARAM_1 << 8);
	err = mipi_dsi_pkt_write(mipi_dsi,MIPI_DSI_DCS_LONG_WRITE, buf, HX8389_CMD_SETPANEL_LEN);
	CHECK_RETCODE(err);
#else
	//Set Power
	buf[0] = HX8389_CMD_SETPOWER|(HX8389_CMD_SETPOWER_PARAM_1<<8);
	buf[1] = HX8389_CMD_SETPOWER_PARAM_2;
	buf[2] = HX8389_CMD_SETPOWER_PARAM_3;
	buf[3] = HX8389_CMD_SETPOWER_PARAM_4;
	err = mipi_dsi_pkt_write(mipi_dsi, MIPI_DSI_DCS_LONG_WRITE,buf, HX8389_CMD_SETPOWER_LEN);
	CHECK_RETCODE(err);

	buf[0] = HX8389_CMD_SET_C0 |(HX8389_CMD_SET_C0_PARAM_1 <<8);
	buf[1] = HX8389_CMD_SET_C0_PARAM_2;
	err = mipi_dsi_pkt_write(mipi_dsi, MIPI_DSI_DCS_LONG_WRITE,buf, HX8389_CMD_SET_C0_LEN);
	CHECK_RETCODE(err);

	//SET Display
	buf[0] = HX8389_CMD_SETDISP |(HX8389_CMD_SETDISP_PARAM_1<<8);
	buf[1] = HX8389_CMD_SETDISP_PARAM_2;
	buf[2] = HX8389_CMD_SETDISP_PARAM_3;
	err = mipi_dsi_pkt_write(mipi_dsi, MIPI_DSI_DCS_LONG_WRITE,buf, HX8389_CMD_SETDISP_LEN);
	CHECK_RETCODE(err);

	//SETCYC
	buf[0] = HX8389_CMD_SETCYC|(HX8389_CMD_SETCYC_PARAM_1<<8);
	buf[1] = HX8389_CMD_SETCYC_PARAM_2;
	buf[2] = HX8389_CMD_SETCYC_PARAM_3;
	err = mipi_dsi_pkt_write(mipi_dsi, MIPI_DSI_DCS_LONG_WRITE,buf, HX8389_CMD_SETCYC_LEN);
	CHECK_RETCODE(err);

	//SET D3
	buf[0] = HX8389_CMD_SET_D3|(HX8389_CMD_SET_D3_PARAM_1<<8);
	buf[1] = HX8389_CMD_SET_D3_PARAM_2;
	buf[2] = HX8389_CMD_SET_D3_PARAM_3;
	buf[3] = HX8389_CMD_SET_D3_PARAM_4;
	buf[4] = HX8389_CMD_SET_D3_PARAM_5;
	buf[5] = HX8389_CMD_SET_D3_PARAM_6;
	buf[6] = HX8389_CMD_SET_D3_PARAM_7;
	buf[7] = HX8389_CMD_SET_D3_PARAM_8;
	buf[8] = HX8389_CMD_SET_D3_PARAM_9;
	err = mipi_dsi_pkt_write(mipi_dsi, MIPI_DSI_DCS_LONG_WRITE,buf, HX8389_CMD_SET_D3_LEN);
	CHECK_RETCODE(err);

	//SET GIP
	buf[0] = HX8389_CMD_SETGIP|(HX8389_CMD_SETGIP_PARAM_1<<8);
	buf[1] = HX8389_CMD_SETGIP_PARAM_2;
	buf[2] = HX8389_CMD_SETGIP_PARAM_3;
	buf[3] = HX8389_CMD_SETGIP_PARAM_4;
	buf[4] = HX8389_CMD_SETGIP_PARAM_5;
	buf[5] = HX8389_CMD_SETGIP_PARAM_6;
	buf[6] = HX8389_CMD_SETGIP_PARAM_7;
	buf[7] = HX8389_CMD_SETGIP_PARAM_8;
	buf[8] = HX8389_CMD_SETGIP_PARAM_9;
	buf[9] =HX8389_CMD_SETGIP_PARAM_10;
	err = mipi_dsi_pkt_write(mipi_dsi, MIPI_DSI_DCS_LONG_WRITE,buf, HX8389_CMD_SETGIP_LEN);
	CHECK_RETCODE(err);

	//SET D6
	buf[0] = HX8389_CMD_SET_D6|(HX8389_CMD_SET_D6_PARAM_1<<8);
	buf[1] = HX8389_CMD_SET_D6_PARAM_2;
	buf[2] = HX8389_CMD_SET_D6_PARAM_3;
	buf[3] = HX8389_CMD_SET_D6_PARAM_4;
	buf[4] = HX8389_CMD_SET_D6_PARAM_5;
	buf[5] = HX8389_CMD_SET_D6_PARAM_6;
	buf[6] = HX8389_CMD_SET_D6_PARAM_7;
	buf[7] = HX8389_CMD_SET_D6_PARAM_8;
	buf[8] = HX8389_CMD_SET_D6_PARAM_9;
	buf[9] =HX8389_CMD_SET_D6_PARAM_10;
	err = mipi_dsi_pkt_write(mipi_dsi, MIPI_DSI_DCS_LONG_WRITE,buf, HX8389_CMD_SET_D6_LEN);
	CHECK_RETCODE(err);

	//SET GAMMA
	buf[0] = HX8389_CMD_SETGAMMA|(HX8389_CMD_SETGAMMA_PARAM_1<<8);
	buf[1] = HX8389_CMD_SETGAMMA_PARAM_2;
	buf[2] = HX8389_CMD_SETGAMMA_PARAM_3;
	buf[3] = HX8389_CMD_SETGAMMA_PARAM_4;
	buf[4] = HX8389_CMD_SETGAMMA_PARAM_5;
	buf[5] = HX8389_CMD_SETGAMMA_PARAM_6;
	buf[6] = HX8389_CMD_SETGAMMA_PARAM_7;
	buf[7] = HX8389_CMD_SETGAMMA_PARAM_8;
	buf[8] = HX8389_CMD_SETGAMMA_PARAM_9;
	buf[9] = HX8389_CMD_SETGAMMA_PARAM_10;
	buf[10]=HX8389_CMD_SETGAMMA_PARAM_11;
	err = mipi_dsi_pkt_write(mipi_dsi, MIPI_DSI_DCS_LONG_WRITE,buf, HX8389_CMD_SETGAMMA_LEN);
	CHECK_RETCODE(err);

	//SET D2
	buf[0] = HX8389_CMD_SET_D2|(HX8389_CMD_SET_D2_PARAM_1<<8);
	err = mipi_dsi_pkt_write(mipi_dsi, MIPI_DSI_DCS_LONG_WRITE,buf, HX8389_CMD_SET_D2_LEN);
	CHECK_RETCODE(err);

	//SET PANEL
	//buf[0] = HX8389_CMD_SETPANEL|(HX8389_CMD_SETPANEL_PARAM_1<<8);
	//err = mipi_dsi_pkt_write(mipi_dsi, MIPI_DSI_DCS_LONG_WRITE,buf, HX8389_CMD_SETPANEL_LEN);
	//CHECK_RETCODE(err);

	//SET VCOM
	buf[0] = HX8389_CMD_SETVCOM|(HX8389_CMD_SETVCOM_PARAM_1<<8);
	err = mipi_dsi_pkt_write(mipi_dsi, MIPI_DSI_DCS_LONG_WRITE,buf, HX8389_CMD_SETVCOM_LEN);
	CHECK_RETCODE(err);
	msleep(10);
	
#endif

	//Set Sleep Out
	/* exit sleep mode and set display on */
	buf[0] = MIPI_DCS_EXIT_SLEEP_MODE;
	err = mipi_dsi_pkt_write(mipi_dsi, MIPI_DSI_DCS_SHORT_WRITE,buf, 0);
	CHECK_RETCODE(err);
	/* To allow time for the supply voltages
	 * and clock circuits to stabilize.
	 */
	msleep(120);

	//Set display On
	buf[0] = MIPI_DCS_SET_DISPLAY_ON;
	err = mipi_dsi_pkt_write(mipi_dsi, MIPI_DSI_DCS_SHORT_WRITE,buf, 0);
	CHECK_RETCODE(err);
	msleep(60);
	
	return 0;
}
