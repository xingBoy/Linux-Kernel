/*
 * Code for AM335X EVM.
 *
 * Copyright (C) 2011 Texas Instruments, Inc. - http://www.ti.com/
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation version 2.
 *
 * This program is distributed "as is" WITHOUT ANY WARRANTY of any
 * kind, whether express or implied; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/i2c.h>
#include <linux/module.h>
#include <linux/i2c/at24.h>
#include <linux/phy.h>
#include <linux/gpio.h>
#include <linux/spi/spi.h>
#include <linux/spi/flash.h>
#include <linux/gpio_keys.h>
#include <linux/input.h>
#include <linux/input/matrix_keypad.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/nand.h>
#include <linux/mtd/partitions.h>
#include <linux/platform_device.h>
#include <linux/clk.h>
#include <linux/err.h>
#include <linux/export.h>
#include <linux/wl12xx.h>
#include <linux/ethtool.h>
#include <linux/mfd/tps65910.h>
#include <linux/mfd/tps65217.h>
#include <linux/pwm_backlight.h>
#include <linux/input/ti_tsc.h>
#include <linux/platform_data/ti_adc.h>
#include <linux/mfd/ti_tscadc.h>
#include <linux/reboot.h>
#include <linux/pwm/pwm.h>
#include <linux/rtc/rtc-omap.h>
#include <linux/opp.h>
#include <linux/skbuff.h>
#include <linux/ti_wilink_st.h>

/* LCD controller is similar to DA850 */
#include <video/da8xx-fb.h>

#include <mach/hardware.h>
#include <mach/board-am335xevm.h>

#include <asm/mach-types.h>
#include <asm/mach/arch.h>
#include <asm/mach/map.h>
#include <asm/hardware/asp.h>

#include <plat/omap_device.h>
#include <plat/omap-pm.h>
#include <plat/irqs.h>
#include <plat/board.h>
#include <plat/common.h>
#include <plat/lcdc.h>
#include <plat/usb.h>
#include <plat/mmc.h>
#include <plat/emif.h>
#include <plat/nand.h>
#include <plat/dma-33xx.h>

#include <media/soc_camera.h>
#include <media/mt9t112.h>

#include "board-flash.h"
#include "cpuidle33xx.h"
#include "mux.h"
#include "devices.h"
#include "hsmmc.h"

#include "control.h"
#include <linux/proc_fs.h>


/* Convert GPIO signal to GPIO pin number */
#define GPIO_TO_PIN(bank, gpio) (32 * (bank) + (gpio))

/* BBB PHY IDs */
#define BBB_PHY_ID		0x7c0f1
#define BBB_PHY_MASK		0xfffffffe

/* AM335X EVM Phy ID and Debug Registers */
#define AM335X_EVM_PHY_ID		0x4dd074
#define AM335X_EVM_PHY_MASK		0xfffffffe
#define AR8051_PHY_DEBUG_ADDR_REG	0x1d
#define AR8051_PHY_DEBUG_DATA_REG	0x1e
#define AR8051_DEBUG_RGMII_CLK_DLY_REG	0x5
#define AR8051_RGMII_TX_CLK_DLY		BIT(8)


#define AM33XX_CTRL_REGADDR(reg)				\
		AM33XX_L4_WK_IO_ADDRESS(AM33XX_SCM_BASE + (reg))

/* bit 3: 0 - enable, 1 - disable for pull enable */
#define AM33XX_PULL_DISA		(1 << 3)
#define AM33XX_PULL_ENBL		(0 << 3)

int selected_pad;
int pad_mux_value;

#define SCREEN_TYPE_R 0
#define SCREEN_TYPE_C 1
int screen_type;
int screen_size;

#include "common.h"

#include <linux/lis3lv02d.h>

static struct omap2_hsmmc_info am335x_mmc[] = {
	{
		.mmc            = 1,
		.caps           = MMC_CAP_4_BIT_DATA,
	    .gpio_cd        = GPIO_TO_PIN(2, 1),
        /*
		.gpio_wp        = GPIO_TO_PIN(2, 0),
		*/
		.ocr_mask       = MMC_VDD_32_33 | MMC_VDD_33_34, /* 3V3 */
	},
	{
		.mmc            = 0,	/* will be set at runtime */
	},
	{
		.mmc            = 0,	/* will be set at runtime */
	},
	{}      /* Terminator */
};


#ifdef CONFIG_OMAP_MUX
static struct omap_board_mux board_mux[] __initdata = {
	/*
	 * Setting SYSBOOT[5] should set xdma_event_intr0 pin to mode 3 thereby
	 * allowing clkout1 to be available on xdma_event_intr0.
	 * However, on some boards (like EVM-SK), SYSBOOT[5] isn't properly
	 * latched.
	 * To be extra cautious, setup the pin-mux manually.
	 * If any modules/usecase requries it in different mode, then subsequent
	 * module init call will change the mux accordingly.
	 */
	AM33XX_MUX(XDMA_EVENT_INTR0, OMAP_MUX_MODE3 | AM33XX_PIN_OUTPUT),
	AM33XX_MUX(I2C0_SDA, OMAP_MUX_MODE0 | AM33XX_SLEWCTRL_SLOW |
			AM33XX_INPUT_EN | AM33XX_PIN_OUTPUT),
	AM33XX_MUX(I2C0_SCL, OMAP_MUX_MODE0 | AM33XX_SLEWCTRL_SLOW |
			AM33XX_INPUT_EN | AM33XX_PIN_OUTPUT),
	{ .reg_offset = OMAP_MUX_TERMINATOR },
};
#else
#define	board_mux	NULL
#endif

/* module pin mux structure */
struct pinmux_config {
	const char *string_name; /* signal name format */
	int val; /* Options for the mux register value */
};

struct evm_dev_cfg {
	void (*device_init)(int evm_id, int profile);

/*
* If the device is required on both baseboard & daughter board (ex i2c),
* specify DEV_ON_BASEBOARD
*/
#define DEV_ON_BASEBOARD	0
#define DEV_ON_DGHTR_BRD	1
	u32 device_on;

	u32 profile;	/* Profiles (0-7) in which the module is present */
};

/* AM335X - CPLD Register Offsets */
#define	CPLD_DEVICE_HDR	0x00 /* CPLD Header */
#define	CPLD_DEVICE_ID	0x04 /* CPLD identification */
#define	CPLD_DEVICE_REV	0x0C /* Revision of the CPLD code */
#define	CPLD_CFG_REG	0x10 /* Configuration Register */

static u32 am335x_evm_id;
static struct omap_board_config_kernel am335x_evm_config[] __initdata = {
};
static bool daughter_brd_detected;
static int am33xx_evmid = -EINVAL;

/*
* am335x_evm_set_id - set up board evmid
* @evmid - evm id which needs to be configured
*
* This function is called to configure board evm id.
*/
void am335x_evm_set_id(unsigned int evmid)
{
	am33xx_evmid = evmid;
	return;
}

/*
* am335x_evm_get_id - returns Board Type (EVM/BB/EVM-SK ...)
*
* Note:
*	returns -EINVAL if Board detection hasn't happened yet.
*/
int am335x_evm_get_id(void)
{
	return am33xx_evmid;
}
EXPORT_SYMBOL(am335x_evm_get_id);

/* Pin mux for nand flash module */
static struct pinmux_config nand_pin_mux[] = {
	{"gpmc_ad0.gpmc_ad0",	  OMAP_MUX_MODE0 | AM33XX_PIN_INPUT_PULLUP},
	{"gpmc_ad1.gpmc_ad1",	  OMAP_MUX_MODE0 | AM33XX_PIN_INPUT_PULLUP},
	{"gpmc_ad2.gpmc_ad2",	  OMAP_MUX_MODE0 | AM33XX_PIN_INPUT_PULLUP},
	{"gpmc_ad3.gpmc_ad3",	  OMAP_MUX_MODE0 | AM33XX_PIN_INPUT_PULLUP},
	{"gpmc_ad4.gpmc_ad4",	  OMAP_MUX_MODE0 | AM33XX_PIN_INPUT_PULLUP},
	{"gpmc_ad5.gpmc_ad5",	  OMAP_MUX_MODE0 | AM33XX_PIN_INPUT_PULLUP},
	{"gpmc_ad6.gpmc_ad6",	  OMAP_MUX_MODE0 | AM33XX_PIN_INPUT_PULLUP},
	{"gpmc_ad7.gpmc_ad7",	  OMAP_MUX_MODE0 | AM33XX_PIN_INPUT_PULLUP},
	{"gpmc_wait0.gpmc_wait0", OMAP_MUX_MODE0 | AM33XX_PIN_INPUT_PULLUP},
	{"gpmc_wpn.gpmc_wpn",	  OMAP_MUX_MODE0 | AM33XX_PIN_INPUT_PULLUP},
	{"gpmc_csn0.gpmc_csn0",	  OMAP_MUX_MODE0 | AM33XX_PULL_DISA},
	{"gpmc_advn_ale.gpmc_advn_ale",  OMAP_MUX_MODE0 | AM33XX_PULL_DISA},
	{"gpmc_oen_ren.gpmc_oen_ren",	 OMAP_MUX_MODE0 | AM33XX_PULL_DISA},
	{"gpmc_wen.gpmc_wen",     OMAP_MUX_MODE0 | AM33XX_PULL_DISA},
	{"gpmc_ben0_cle.gpmc_ben0_cle",	 OMAP_MUX_MODE0 | AM33XX_PULL_DISA},
	{NULL, 0},
};

/* Module pin mux for mii1 */
static struct pinmux_config mii1_pin_mux[] = {
      {"mii1_rxerr.mii1_rxerr", OMAP_MUX_MODE0 | AM33XX_PIN_INPUT_PULLDOWN},
      {"mii1_txen.mii1_txen", OMAP_MUX_MODE0 | AM33XX_PIN_OUTPUT},
      {"mii1_rxdv.mii1_rxdv", OMAP_MUX_MODE0 | AM33XX_PIN_INPUT_PULLDOWN},
      {"mii1_txd3.mii1_txd3", OMAP_MUX_MODE0 | AM33XX_PIN_OUTPUT},
      {"mii1_txd2.mii1_txd2", OMAP_MUX_MODE0 | AM33XX_PIN_OUTPUT},
      {"mii1_txd1.mii1_txd1", OMAP_MUX_MODE0 | AM33XX_PIN_OUTPUT},
      {"mii1_txd0.mii1_txd0", OMAP_MUX_MODE0 | AM33XX_PIN_OUTPUT},
      {"mii1_txclk.mii1_txclk", OMAP_MUX_MODE0 | AM33XX_PIN_INPUT_PULLDOWN},
      {"mii1_rxclk.mii1_rxclk", OMAP_MUX_MODE0 | AM33XX_PIN_INPUT_PULLDOWN},
      {"mii1_rxd3.mii1_rxd3", OMAP_MUX_MODE0 | AM33XX_PIN_INPUT_PULLDOWN},
      {"mii1_rxd2.mii1_rxd2", OMAP_MUX_MODE0 | AM33XX_PIN_INPUT_PULLDOWN},
      {"mii1_rxd1.mii1_rxd1", OMAP_MUX_MODE0 | AM33XX_PIN_INPUT_PULLDOWN},
      {"mii1_rxd0.mii1_rxd0", OMAP_MUX_MODE0 | AM33XX_PIN_INPUT_PULLDOWN},
      {"mdio_data.mdio_data", OMAP_MUX_MODE0 | AM33XX_PIN_INPUT_PULLUP},
      {"mdio_clk.mdio_clk", OMAP_MUX_MODE0 | AM33XX_PIN_OUTPUT_PULLUP},
      {NULL, 0},
};

static struct pinmux_config mii2_pin_mux[] = {
    {"gpmc_wpn.mii2_rxerr",OMAP_MUX_MODE1 |AM33XX_PIN_INPUT_PULLDOWN},
    {"gpmc_a0.mii2_txen", OMAP_MUX_MODE1 | AM33XX_PIN_OUTPUT},
	{"gpmc_a1.mii2_rxdv", OMAP_MUX_MODE1| AM33XX_PIN_INPUT_PULLDOWN},
    {"gpmc_a2.mii2_txd3", OMAP_MUX_MODE1 | AM33XX_PIN_OUTPUT},
    {"gpmc_a3.mii2_txd2", OMAP_MUX_MODE1| AM33XX_PIN_OUTPUT},
    {"gpmc_a4.mii2_txd1", OMAP_MUX_MODE1 | AM33XX_PIN_OUTPUT},
    {"gpmc_a5.mii2_txd0", OMAP_MUX_MODE1 | AM33XX_PIN_OUTPUT},
    {"gpmc_a6.mii2_txclk",OMAP_MUX_MODE1 | AM33XX_PIN_INPUT_PULLDOWN},
    /*
	{"gpmc_a6.mii2_txclk", OMAP_MUX_MODE1 | AM33XX_PIN_OUTPUT},
	 */
    {"gpmc_a7.mii2_rxclk", OMAP_MUX_MODE1 | AM33XX_PIN_INPUT_PULLDOWN},
    {"gpmc_a8.mii2_rxd3", OMAP_MUX_MODE1 |   AM33XX_PIN_INPUT },
    {"gpmc_a9.mii2_rxd2", OMAP_MUX_MODE1 |   AM33XX_PIN_INPUT },
    {"gpmc_a10.mii2_rxd1", OMAP_MUX_MODE1 |  AM33XX_PIN_INPUT },
    {"gpmc_a11.mii2_rxd0", OMAP_MUX_MODE1 |  AM33XX_PIN_INPUT },
    {"gpmc_wpn.mii2_rxerr", OMAP_MUX_MODE1 | AM33XX_PIN_INPUT_PULLDOWN},
    {"gpmc_ben1.mii2_col",OMAP_MUX_MODE1 | AM33XX_PIN_OUTPUT},
    {"mdio_data.mdio_data", OMAP_MUX_MODE0 | AM33XX_PIN_INPUT_PULLUP},
    {"mdio_clk.mdio_clk", OMAP_MUX_MODE0 | AM33XX_PIN_OUTPUT_PULLUP},
    {NULL, 0},
};

/* Module pin mux for mmc0 */
static struct pinmux_config mmc0_common_pin_mux[] = {
	{"mmc0_dat3.mmc0_dat3",	OMAP_MUX_MODE0 | AM33XX_PIN_INPUT_PULLUP},
	{"mmc0_dat2.mmc0_dat2",	OMAP_MUX_MODE0 | AM33XX_PIN_INPUT_PULLUP},
	{"mmc0_dat1.mmc0_dat1",	OMAP_MUX_MODE0 | AM33XX_PIN_INPUT_PULLUP},
	{"mmc0_dat0.mmc0_dat0",	OMAP_MUX_MODE0 | AM33XX_PIN_INPUT_PULLUP},
	{"mmc0_clk.mmc0_clk",	OMAP_MUX_MODE0 | AM33XX_PIN_INPUT_PULLUP},
	{"mmc0_cmd.mmc0_cmd",	OMAP_MUX_MODE0 | AM33XX_PIN_INPUT_PULLUP},
	{"gpmc_clk.gpio2_1", OMAP_MUX_MODE7 | AM33XX_PIN_INPUT_PULLUP},
	{NULL, 0},
};

static struct pinmux_config uart0_pin_mux[] = {
    {"uart0_rxd.uart0_rxd", OMAP_MUX_MODE0 | AM33XX_PIN_INPUT_PULLUP},
    {"uart0_txd.uart0_txd", OMAP_MUX_MODE0 | AM33XX_PULL_ENBL},
    {NULL, 0},
};

static struct pinmux_config uart1_pin_mux[] = {
    {"uart1_rxd.uart1_rxd", OMAP_MUX_MODE0 | AM33XX_PIN_INPUT_PULLUP},
    {"uart1_txd.uart1_txd", OMAP_MUX_MODE0 | AM33XX_PULL_ENBL},
    {NULL, 0},
};

static struct pinmux_config uart2_pin_mux[] = {
	{"spi0_sclk.uart2_rxd", OMAP_MUX_MODE1 | AM33XX_SLEWCTRL_SLOW |AM33XX_PIN_INPUT_PULLUP},
	{"spi0_d0.uart2_txd", OMAP_MUX_MODE1 | AM33XX_PULL_UP |AM33XX_PULL_DISA |AM33XX_SLEWCTRL_SLOW},
	{NULL, 0},
};

static struct pinmux_config uart5_pin_mux[] = {
     {"lcd_data9.uart5_rxd", OMAP_MUX_MODE4 | AM33XX_PIN_INPUT_PULLUP},
     {"lcd_data8.uart5_txd", OMAP_MUX_MODE4 | AM33XX_PULL_ENBL},
     {NULL, 0},
};


static struct pinmux_config i2c1_pin_mux[] = {
	{"spi0_d1.i2c1_sda",    OMAP_MUX_MODE2 | AM33XX_SLEWCTRL_SLOW |
					AM33XX_PULL_ENBL | AM33XX_INPUT_EN},
	{"spi0_cs0.i2c1_scl",   OMAP_MUX_MODE2 | AM33XX_SLEWCTRL_SLOW |
					AM33XX_PULL_ENBL | AM33XX_INPUT_EN},
	{NULL, 0},
};

static struct pinmux_config d_can0_pin_mux[] = {
    {"uart1_ctsn.d_can0_tx", OMAP_MUX_MODE2 | AM33XX_PULL_ENBL},
    {"uart1_rtsn.d_can0_rx", OMAP_MUX_MODE2 | AM33XX_PIN_INPUT_PULLUP},
    {NULL, 0},
};

static struct pinmux_config d_can1_pin_mux[] = {
	{"uart0_ctsn.d_can1_tx", OMAP_MUX_MODE2 | AM33XX_PULL_ENBL},
	{"uart0_rtsn.d_can1_rx", OMAP_MUX_MODE2 | AM33XX_PIN_INPUT_PULLUP},
	{NULL, 0},
};

static struct pinmux_config cmi_at752_gpio_pin_mux[] = {
    {"mcasp0_ahclkx.gpio3_21",  OMAP_MUX_MODE7 | AM33XX_PIN_OUTPUT},
    {"mcasp0_aclkx.gpio3_14",   OMAP_MUX_MODE7 | AM33XX_PIN_OUTPUT},
    {"mcasp0_fsx.gpio3_15",     OMAP_MUX_MODE7 | AM33XX_PIN_OUTPUT},
    {"mcasp0_axr0.gpio3_16",    OMAP_MUX_MODE7 | AM33XX_PIN_OUTPUT},
    {"mcasp0_ahclkr.gpio3_17",  OMAP_MUX_MODE7 | AM33XX_PIN_OUTPUT},
    {"mcasp0_aclkr.gpio3_18",   OMAP_MUX_MODE7 | AM33XX_PIN_OUTPUT},
    {"mcasp0_fsr.gpio3_19",    OMAP_MUX_MODE7 | AM33XX_PIN_OUTPUT},
    {"mcasp0_axr1.gpio3_20",    OMAP_MUX_MODE7 | AM33XX_PIN_OUTPUT},

    {"rmii1_refclk.gpio0_29",    OMAP_MUX_MODE7 | AM33XX_PIN_OUTPUT},
    {"gpmc_ad12.gpio1_12",    OMAP_MUX_MODE7 | AM33XX_PIN_INPUT},

    {NULL, 0},
};

static struct pinmux_config ecap1_pin_mux[] = {
	{"spi0_cs1.ecap1_in_pwm1_out",
		OMAP_MUX_MODE2 | AM33XX_PIN_OUTPUT},
	{NULL, 0},
};

/*
* @pin_mux - single module pin-mux structure which defines pin-mux
*			details for all its pins.
*/
static void setup_pin_mux(struct pinmux_config *pin_mux)
{
	int i;

	for (i = 0; pin_mux->string_name != NULL; pin_mux++)
		omap_mux_init_signal(pin_mux->string_name, pin_mux->val);
}

/*
* @evm_id - evm id which needs to be configured
* @dev_cfg - single evm structure which includes
*				all module inits, pin-mux defines
* @profile - if present, else PROFILE_NONE
* @dghtr_brd_flg - Whether Daughter board is present or not
*/
static void _configure_device(int evm_id, struct evm_dev_cfg *dev_cfg,
	int profile)
{
	int i;

	am335x_evm_set_id(evm_id);

	/*
	* Only General Purpose & Industrial Auto Motro Control
	* EVM has profiles. So check if this evm has profile.
	* If not, ignore the profile comparison
	*/

	/*
	* If the device is on baseboard, directly configure it. Else (device on
	* Daughter board), check if the daughter card is detected.
	*/
	if (profile == PROFILE_NONE) {
		for (i = 0; dev_cfg->device_init != NULL; dev_cfg++) {
			if (dev_cfg->device_on == DEV_ON_BASEBOARD)
				dev_cfg->device_init(evm_id, profile);
			else if (daughter_brd_detected == true)
				dev_cfg->device_init(evm_id, profile);
		}
	} else {
		for (i = 0; dev_cfg->device_init != NULL; dev_cfg++) {
			if (dev_cfg->profile & profile) {
				if (dev_cfg->device_on == DEV_ON_BASEBOARD)
					dev_cfg->device_init(evm_id, profile);
				else if (daughter_brd_detected == true)
					dev_cfg->device_init(evm_id, profile);
			}
		}
	}
}

/* pinmux for usb0 drvvbus */
static struct pinmux_config usb0_pin_mux[] = {
	{"usb0_drvvbus.usb0_drvvbus", OMAP_MUX_MODE0 | AM33XX_PIN_OUTPUT},
	{NULL, 0},
};

/* pinmux for usb1 drvvbus */
static struct pinmux_config usb1_pin_mux[] = {
	{"usb1_drvvbus.usb1_drvvbus", OMAP_MUX_MODE0 | AM33XX_PIN_OUTPUT},
	//{"usb1_drvvbus.gpio3_13", OMAP_MUX_MODE7 | AM33XX_PIN_OUTPUT | AM33XX_PIN_OUTPUT_PULLUP},
	{NULL, 0},
};

#define BEAGLEBONE_CAMERA_ORIENTATION GPIO_TO_PIN(0, 30)

static void mii1_init(int evm_id, int profile)
{
	setup_pin_mux(mii1_pin_mux);
	return;
}

static void mii2_init(int evm_id, int profile)
{
	setup_pin_mux(mii2_pin_mux);
	return;
}

static int am33xx_evm_tx_clk_dly_phy_fixup(struct phy_device *phydev)
{
    // phy is LAN8710, not AR8051, so comment it
    // phy_write(phydev, AR8051_PHY_DEBUG_ADDR_REG,
    // AR8051_DEBUG_RGMII_CLK_DLY_REG);
    // phy_write(phydev, AR8051_PHY_DEBUG_DATA_REG, AR8051_RGMII_TX_CLK_DLY);

	return 0;
}

#define LAN8710A_PHY_ID       0x7c0f1
char sbc_7109_phy1[10] = {"0:00"};
char sbc_7109_phy2[10] = {"0:01"};

static void cmi_at752_cpsw_init(int evm_id, int profile)
{
	am33xx_cpsw_init(AM33XX_CPSW_MODE_MII, sbc_7109_phy1, sbc_7109_phy2);
	phy_register_fixup_for_uid(LAN8710A_PHY_ID , AM335X_EVM_PHY_MASK,
			                     am33xx_evm_tx_clk_dly_phy_fixup);
    printk("cmi_at101 cpsw init .....................\n");
	return;
}

static void usb0_init(int evm_id, int profile)
{
	setup_pin_mux(usb0_pin_mux);
	return;
}

static void usb1_init(int evm_id, int profile)
{
	setup_pin_mux(usb1_pin_mux);
	return;
}

/* setup uart0 for sbc-7109 */
static void uart0_init(int evm_id, int profile)
{
	setup_pin_mux(uart0_pin_mux);
	return;
}

static void uart1_init(int evm_id, int profile)
{
	setup_pin_mux(uart1_pin_mux);
	return;
}

static void uart2_init(int evm_id, int profile)
{
	setup_pin_mux(uart2_pin_mux);
	return;
}

static void uart5_init(int evm_id, int profile)
{
	setup_pin_mux(uart5_pin_mux);
	return;
}

static void d_can_init(int evm_id, int profile)
{
	setup_pin_mux(d_can0_pin_mux);
	am33xx_d_can_init(0);

	setup_pin_mux(d_can1_pin_mux);
	am33xx_d_can_init(1);
}

static struct i2c_board_info am335x_i2c1_boardinfo2[] = {
    {
		I2C_BOARD_INFO("ds1307", 0x68),
    },
};

static void i2c1_init(int evm_id, int profile)
{
	setup_pin_mux(i2c1_pin_mux);
	omap_register_i2c_bus(2, 100, am335x_i2c1_boardinfo2, ARRAY_SIZE(am335x_i2c1_boardinfo2));
	return;
}

static void cmi_at752_gpio(int evm_id, int profile)
{
    setup_pin_mux(cmi_at752_gpio_pin_mux);
}

static struct adc_data am335x_adc_data = {
	.adc_channels = 8,
};

static struct mfd_tscadc_board adc = {
	.adc_init = &am335x_adc_data,
};

static void mfd_adc_init(int evm_id, int profile)
{
	int err;

	err = am33xx_register_mfd_tscadc(&adc);
	if (err)
		pr_err("failed to register touchscreen device\n");
}

static struct pwmss_platform_data  pwm_pdata[3] = {
	{
		.version = PWM_VERSION_1,
	},
	{
		.version = PWM_VERSION_1,
	},
	{
		.version = PWM_VERSION_1,
	},
};

static void buzzer_init(int evm_id, int profile)
{
    setup_pin_mux(ecap1_pin_mux);
    am33xx_register_ecap(1, &pwm_pdata[1]);
}

/* setup haptics */
#define HAPTICS_MAX_FREQ 250
static struct mtd_partition am335x_nand_partitions[] = {
/* All the partition sizes are listed in terms of NAND block size */
	{
		.name           = "SPL",
		.offset         = 0,			/* Offset = 0x0 */
		.size           = SZ_128K,
	},
	{
		.name           = "SPL.backup1",
		.offset         = MTDPART_OFS_APPEND,	/* Offset = 0x20000 */
		.size           = SZ_128K,
	},
	{
		.name           = "SPL.backup2",
		.offset         = MTDPART_OFS_APPEND,	/* Offset = 0x40000 */
		.size           = SZ_128K,
	},
	{
		.name           = "SPL.backup3",
		.offset         = MTDPART_OFS_APPEND,	/* Offset = 0x60000 */
		.size           = SZ_128K,
	},
	{
		.name           = "U-Boot",
		.offset         = MTDPART_OFS_APPEND,   /* Offset = 0x80000 */
		.size           = 15 * SZ_128K,
	},
	{
		.name           = "U-Boot Env",
		.offset         = MTDPART_OFS_APPEND,   /* Offset = 0x260000 */
		.size           = 1 * SZ_128K,
	},
	{
		.name           = "Kernel",
		.offset         = MTDPART_OFS_APPEND,   /* Offset = 0x280000 */
		.size           = 40 * SZ_128K,
	},
	{
		.name           = "File System",
		.offset         = MTDPART_OFS_APPEND,   /* Offset = 0x780000 */
		.size           = MTDPART_SIZ_FULL,
	},
};

/* NAND partition information */
#if 0
static struct mtd_partition am335x_nand_partitions[] = {
/* All the partition sizes are listed in terms of NAND block size */
	{
		.name           = "SPL",
		.offset         = 0,			/* Offset = 0x0 */
		.size           = SZ_128K,
	},
	{
		.name           = "SPL.backup1",
		.offset         = MTDPART_OFS_APPEND,	/* Offset = 0x20000 */
		.size           = SZ_128K,
	},
	{
		.name           = "SPL.backup2",
		.offset         = MTDPART_OFS_APPEND,	/* Offset = 0x40000 */
		.size           = SZ_128K,
	},
	{
		.name           = "SPL.backup3",
		.offset         = MTDPART_OFS_APPEND,	/* Offset = 0x60000 */
		.size           = SZ_128K,
	},
	{
		.name           = "U-Boot",
		.offset         = MTDPART_OFS_APPEND,   /* Offset = 0x80000 */
		.size           = 15 * SZ_128K,
	},
	{
		.name           = "U-Boot Env",
		.offset         = MTDPART_OFS_APPEND,   /* Offset = 0x260000 */
		.size           = 1 * SZ_128K,
	},
	{
		.name           = "Kernel",
		.offset         = MTDPART_OFS_APPEND,   /* Offset = 0x280000 */
		.size           = 40 * SZ_128K,
	},
	{
		.name           = "File System",
		.offset         = MTDPART_OFS_APPEND,   /* Offset = 0x780000 */
		.size           = MTDPART_SIZ_FULL,
	},
};
#endif

static struct gpmc_timings am335x_nand_timings = {
    /*
	.sync_clk = 0,

	.cs_on = 0,
	.cs_rd_off = 44,
	.cs_wr_off = 44,

	.adv_on = 6,
	.adv_rd_off = 34,
	.adv_wr_off = 44,
	.we_off = 40,
	.oe_off = 54,

	.access = 64,
	.rd_cycle = 82,
	.wr_cycle = 82,

	.wr_access = 40,
	.wr_data_mux_bus = 0,
    */
	.sync_clk = 0,

	.cs_on = 0,
	.cs_rd_off = 44,
	.cs_wr_off = 44,

	.adv_on = 6,
	.adv_rd_off = 34,
	.adv_wr_off = 44,

	.we_off = 40,
	.oe_off = 54,

	.access = 64,
	.rd_cycle = 82,
	.wr_cycle = 82,

	.wr_access = 40,
	.wr_data_mux_bus = 0,
};

static void evm_nand_init(int evm_id, int profile)
{
	struct omap_nand_platform_data *pdata;
	struct gpmc_devices_info gpmc_device[2] = {
		{ NULL, 0 },
		{ NULL, 0 },
	};

	setup_pin_mux(nand_pin_mux);

	/**
	 * zengjf 2015-8-27 modify to 512Mbyte nand device
	 */
	pdata = omap_nand_init(am335x_nand_partitions,
		ARRAY_SIZE(am335x_nand_partitions), 0, 0,
		//NULL);
		&am335x_nand_timings);
	if (!pdata)
		return;

    pdata->ecc_opt =OMAP_ECC_BCH8_CODE_HW;
    //pdata->ecc_opt =OMAP_ECC_HAMMING_CODE_DEFAULT;

	pdata->elm_used = true;
	gpmc_device[0].pdata = pdata;
	gpmc_device[0].flag = GPMC_DEVICE_NAND;

	omap_init_gpmc(gpmc_device, sizeof(gpmc_device));
	omap_init_elm();
}

/* TPS65217 voltage regulator support */


struct tps65217_bl_pdata bone_lcd3_bl_pdata[] = {
	{
		.isel = TPS65217_BL_ISET1,
		.fdim = TPS65217_BL_FDIM_200HZ,
	},
};

static struct regulator_init_data am335x_dummy = {
	    .constraints.always_on  = true,
};

static struct regulator_consumer_supply am335x_vdd1_supply[] = {
	    REGULATOR_SUPPLY("vdd_mpu", NULL),
};
static struct regulator_init_data am335x_vdd1 = {
    .constraints = {
    .min_uV         = 600000,
    .max_uV         = 1500000,
	.valid_modes_mask   = REGULATOR_MODE_NORMAL,
	.valid_ops_mask     = REGULATOR_CHANGE_VOLTAGE,
	.always_on      = 1,
	 },
	.num_consumer_supplies  = ARRAY_SIZE(am335x_vdd1_supply),
	.consumer_supplies  = am335x_vdd1_supply,
	.ignore_check_consumers = 1,
};
static struct regulator_consumer_supply am335x_vdd2_supply[] = {
	    REGULATOR_SUPPLY("vdd_core", NULL),
};
static struct regulator_init_data am335x_vdd2 = {
	.constraints = {
	.min_uV         = 600000,
	.max_uV         = 1500000,
	.valid_modes_mask   = REGULATOR_MODE_NORMAL,
	.valid_ops_mask     = REGULATOR_CHANGE_VOLTAGE,
	.always_on      = 1,
	 },
	.num_consumer_supplies  = ARRAY_SIZE(am335x_vdd2_supply),
	.consumer_supplies  = am335x_vdd2_supply,
	.ignore_check_consumers = 1,
};
#define PMIC_IRQ_GPIO       GPIO_TO_PIN(0, 19)
 struct tps65910_board am335x_tps65910_info = {
	.irq_base   = TWL4030_IRQ_BASE,
    .gpio_base  = OMAP_MAX_GPIO_LINES,
	/*
	 * danny modify
    .irq        = OMAP_GPIO_IRQ(PMIC_IRQ_GPIO),
	*/
    .tps65910_pmic_init_data[TPS65910_REG_VRTC] = &am335x_dummy,
    .tps65910_pmic_init_data[TPS65910_REG_VIO]  = &am335x_dummy,
    .tps65910_pmic_init_data[TPS65910_REG_VDD1] = &am335x_vdd1,
    .tps65910_pmic_init_data[TPS65910_REG_VDD2] = &am335x_vdd2,
    .tps65910_pmic_init_data[TPS65910_REG_VDD3] = &am335x_dummy,
    .tps65910_pmic_init_data[TPS65910_REG_VDIG1]    = &am335x_dummy,
    .tps65910_pmic_init_data[TPS65910_REG_VDIG2]    = &am335x_dummy,
    .tps65910_pmic_init_data[TPS65910_REG_VPLL] = &am335x_dummy,
    .tps65910_pmic_init_data[TPS65910_REG_VDAC] = &am335x_dummy,
    .tps65910_pmic_init_data[TPS65910_REG_VAUX1]    = &am335x_dummy,
    .tps65910_pmic_init_data[TPS65910_REG_VAUX2]    = &am335x_dummy,
    .tps65910_pmic_init_data[TPS65910_REG_VAUX33]   = &am335x_dummy,
    .tps65910_pmic_init_data[TPS65910_REG_VMMC] = &am335x_dummy,
};

#define GPIO_TO_PIN(bank, gpio) (32 * (bank) + (gpio))

#ifdef CONFIG_TI_ST
/* TI-ST for WL12xx BT */

/* Bluetooth Enable PAD for EVM Rev 1.1 and up */
#define AM33XX_CONTROL_PADCONF_MCASP0_AHCLKX_OFFSET		0x09AC

/* Bluetooth Enable PAD for EVM Rev 1.0 */
#define AM33XX_CONTROL_PADCONF_GPMC_CSN2_OFFSET			0x0884

int plat_kim_suspend(struct platform_device *pdev, pm_message_t state)
{
	/* TODO: wait for HCI-LL sleep */
	return 0;
}

int plat_kim_resume(struct platform_device *pdev)
{
	return 0;
}

int plat_kim_chip_enable(struct kim_data_s *kim_data)
{
	printk(KERN_DEBUG "%s\n", __func__);

	/* Configure BT_EN pin so that suspend/resume works correctly on rev 1.1 */
	selected_pad = AM33XX_CONTROL_PADCONF_MCASP0_AHCLKX_OFFSET;
	/* Configure BT_EN pin so that suspend/resume works correctly on rev 1.0 */
	/*selected_pad = AM33XX_CONTROL_PADCONF_GPMC_CSN2_OFFSET;*/

	gpio_direction_output(kim_data->nshutdown, 0);
	msleep(1);
	gpio_direction_output(kim_data->nshutdown, 1);

	/* Enable pullup on the enable pin for keeping BT active during suspend */
	pad_mux_value = readl(AM33XX_CTRL_REGADDR(selected_pad));
	pad_mux_value &= (~AM33XX_PULL_DISA);
	writel(pad_mux_value, AM33XX_CTRL_REGADDR(selected_pad));

	return 0;
}

int plat_kim_chip_disable(struct kim_data_s *kim_data)
{
	printk(KERN_DEBUG "%s\n", __func__);

	gpio_direction_output(kim_data->nshutdown, 0);

	/* Disable pullup on the enable pin to allow BT shut down during suspend */
	pad_mux_value = readl(AM33XX_CTRL_REGADDR(selected_pad));
	pad_mux_value |= AM33XX_PULL_DISA;
	writel(pad_mux_value, AM33XX_CTRL_REGADDR(selected_pad));

	return 0;
}

struct ti_st_plat_data wilink_pdata = {
	.nshutdown_gpio = GPIO_TO_PIN(3, 21),
	.dev_name = "/dev/ttyO1",
	.flow_cntrl = 1,
	.baud_rate = 3000000,
	.suspend = plat_kim_suspend,
	.resume = plat_kim_resume,
	.chip_enable = plat_kim_chip_enable,
	.chip_disable = plat_kim_chip_disable,
};

static struct platform_device wl12xx_device = {
	.name		= "kim",
	.id		= -1,
	.dev.platform_data = &wilink_pdata,
};

static struct platform_device btwilink_device = {
	.name = "btwilink",
	.id = -1,
};

#ifdef CONFIG_MACH_AM335XEVM_WILINK8
static struct platform_device nfcwilink_device = {
        .name = "nfcwilink",
        .id = -1,
};
#endif

static inline void __init am335xevm_init_btwilink(void)
{
	pr_info("am335xevm: bt init\n");

	platform_device_register(&wl12xx_device);
	platform_device_register(&btwilink_device);
#ifdef CONFIG_MACH_AM335XEVM_WILINK8
	platform_device_register(&nfcwilink_device);
#endif
}
#endif

static void mmc0_init(int evm_id, int profile)
{
	switch (evm_id) {
	case BEAGLE_BONE_A3:
	case BEAGLE_BONE_OLD:
	case EVM_SK:
	case BEAGLE_BONE_BLACK:
		setup_pin_mux(mmc0_common_pin_mux);
		//setup_pin_mux(mmc0_cd_only_pin_mux);
		break;
	default:
		setup_pin_mux(mmc0_common_pin_mux);
		//setup_pin_mux(mmc0_cd_only_pin_mux);
		//setup_pin_mux(mmc0_wp_only_pin_mux);
		break;
	}

	omap2_hsmmc_init(am335x_mmc);
	return;
}

static struct i2c_board_info i2c0_boardinfo[] = {
	{
		I2C_BOARD_INFO("tps65910", TPS65910_I2C_ID1),
		.platform_data  = &am335x_tps65910_info,
	},
	{
		I2C_BOARD_INFO("24c02", 0x50),
	},

};

#define procfs_name "boardname"

int proc_read(char *buffer,char **buffer_location,off_t offset, int buffer_length, int *eof, void *data)
{
	int ret;

	if (offset > 0) {
	/* we have finished to read, return 0 */
	ret = 0;
	} else {
	/* fill the buffer, return the buffer size */
#if defined(CONFIG_OK335XD)
	ret = sprintf(buffer, "OK335xD");
#elif defined(CONFIG_OK335XS)
	ret = sprintf(buffer, "OK335xS");
#endif
	}

	return ret;
}

int proc_init(void)
{
	struct proc_dir_entry *Our_Proc_File;

	Our_Proc_File = create_proc_entry(procfs_name, 0644, NULL);

	if (Our_Proc_File == NULL) {
		remove_proc_entry(procfs_name, NULL);
		printk(KERN_ALERT "Error: Could not initialize /proc/%s\n",
		procfs_name);
		return -ENOMEM;
	}

	Our_Proc_File->read_proc = proc_read;
	//Our_Proc_File->owner = THIS_MODULE;
	Our_Proc_File->mode = S_IFREG | S_IRUGO;
	Our_Proc_File->uid = 0;
	Our_Proc_File->gid = 0;
	Our_Proc_File->size = 37;

	return 0; /* everything is ok */
}

/* cmi_at752 */
static struct evm_dev_cfg cmi_at752_dev_cfg[] = {
	{mmc0_init,	            DEV_ON_BASEBOARD, PROFILE_ALL},
	{evm_nand_init,         DEV_ON_BASEBOARD, PROFILE_ALL},
	{uart0_init,            DEV_ON_BASEBOARD, PROFILE_ALL},
	{uart1_init,            DEV_ON_BASEBOARD, PROFILE_ALL},
	{uart2_init,            DEV_ON_BASEBOARD, PROFILE_ALL},
	{uart5_init,            DEV_ON_BASEBOARD, PROFILE_ALL},
    {mii1_init,	            DEV_ON_BASEBOARD, PROFILE_ALL},
    {mii2_init,	            DEV_ON_BASEBOARD, PROFILE_ALL},
    {cmi_at752_cpsw_init,	DEV_ON_BASEBOARD, PROFILE_ALL},
	{usb0_init,	            DEV_ON_BASEBOARD, PROFILE_ALL},
	{usb1_init,	            DEV_ON_BASEBOARD, PROFILE_ALL},
    //{d_can_init,            DEV_ON_BASEBOARD, PROFILE_ALL},
    {i2c1_init,             DEV_ON_BASEBOARD, PROFILE_ALL},
    {cmi_at752_gpio,        DEV_ON_BASEBOARD, PROFILE_ALL},
    {mfd_adc_init,          DEV_ON_BASEBOARD, PROFILE_ALL},
    {buzzer_init,          DEV_ON_BASEBOARD, PROFILE_ALL},

    {NULL, 0, 0},
};

#define AM33XX_VDD_CORE_OPP50_UV		1100000
#define AM33XX_OPP120_FREQ		600000000
#define AM33XX_OPPTURBO_FREQ		720000000

#define AM33XX_ES2_0_VDD_CORE_OPP50_UV	950000
#define AM33XX_ES2_0_OPP120_FREQ	720000000
#define AM33XX_ES2_0_OPPTURBO_FREQ	800000000
#define AM33XX_ES2_0_OPPNITRO_FREQ	1000000000

#define AM33XX_ES2_1_VDD_CORE_OPP50_UV	950000
#define AM33XX_ES2_1_OPP120_FREQ	720000000
#define AM33XX_ES2_1_OPPTURBO_FREQ	800000000
#define AM33XX_ES2_1_OPPNITRO_FREQ	1000000000

#ifdef CONFIG_MACH_AM335XEVM_VIBRATOR
int am335xevm_vibrator_init(void);
#endif

/* sbc-7109 */
#define TLK110_PHY_ID       0x2000a211


static void setup_cmi_at752(void)
{
	pr_info("The board is cmi_at752.\n");

	/*which doesn't have Write Protect pin LAN8710A_PHY_ID */
	am335x_mmc[0].gpio_wp = -EINVAL;

	_configure_device(EVM_SK, cmi_at752_dev_cfg, PROFILE_NONE);
}

static struct omap_musb_board_data musb_board_data = {
	.interface_type	= MUSB_INTERFACE_ULPI,
	/*
	 * mode[0:3] = USB0PORT's mode
	 * mode[4:7] = USB1PORT's mode
	 * AM335X beta EVM has USB0 in OTG mode and USB1 in host mode.
	.mode           = (MUSB_HOST << 4) | MUSB_OTG,
	 */

	/**
         * zengjf 2015-8-26 modify USB0 to host only
         */
	//.mode           = (MUSB_HOST << 4) | MUSB_OTG,
	.mode           = (MUSB_HOST << 4) | MUSB_HOST ,
	.power		= 500,
	.instances	= 1,
};


static void __init am335x_evm_i2c0_init(void)
{
	/* Initially assume General Purpose EVM Config */
	am335x_evm_id = EVM_SK;
	omap_register_i2c_bus(1, 100, i2c0_boardinfo,ARRAY_SIZE(i2c0_boardinfo));
}

void __iomem *am33xx_emif_base;

void __iomem * __init am33xx_get_mem_ctlr(void)
{

	am33xx_emif_base = ioremap(AM33XX_EMIF0_BASE, SZ_32K);

	if (!am33xx_emif_base)
		pr_warning("%s: Unable to map DDR2 controller",	__func__);

	return am33xx_emif_base;
}

void __iomem *am33xx_get_ram_base(void)
{
	return am33xx_emif_base;
}

void __iomem *am33xx_gpio0_base;

void __iomem *am33xx_get_gpio0_base(void)
{
	am33xx_gpio0_base = ioremap(AM33XX_GPIO0_BASE, SZ_4K);

	return am33xx_gpio0_base;
}

static struct resource am33xx_cpuidle_resources[] = {
	{
		.start		= AM33XX_EMIF0_BASE,
		.end		= AM33XX_EMIF0_BASE + SZ_32K - 1,
		.flags		= IORESOURCE_MEM,
	},
};

/* AM33XX devices support DDR2 power down */
static struct am33xx_cpuidle_config am33xx_cpuidle_pdata = {
	.ddr2_pdown	= 1,
};

static struct platform_device am33xx_cpuidle_device = {
	.name			= "cpuidle-am33xx",
	.num_resources		= ARRAY_SIZE(am33xx_cpuidle_resources),
	.resource		= am33xx_cpuidle_resources,
	.dev = {
		.platform_data	= &am33xx_cpuidle_pdata,
	},
};

static void __init am33xx_cpuidle_init(void)
{
	int ret;

	am33xx_cpuidle_pdata.emif_base = am33xx_get_mem_ctlr();

	ret = platform_device_register(&am33xx_cpuidle_device);

	if (ret)
		pr_warning("AM33XX cpuidle registration failed\n");
}

static void __init am335x_evm_init(void)
{
	am33xx_cpuidle_init();
	am33xx_mux_init(board_mux);
	omap_serial_init();
	am335x_evm_i2c0_init();
	omap_sdrc_init(NULL, NULL);
	usb_musb_init(&musb_board_data);

	omap_board_config = am335x_evm_config;
	omap_board_config_size = ARRAY_SIZE(am335x_evm_config);

	daughter_brd_detected = false;
	setup_cmi_at752();

	/*create  /proc/boardname to export info to userspace*/
	proc_init();

	/* Create an alias for icss clock */
	if (clk_add_alias("pruss", NULL, "pruss_uart_gclk", NULL))
		pr_warn("failed to create an alias: icss_uart_gclk --> pruss\n");
	/* Create an alias for gfx/sgx clock */
	if (clk_add_alias("sgx_ck", NULL, "gfx_fclk", NULL))
		pr_warn("failed to create an alias: gfx_fclk --> sgx_ck\n");
}

static void __init am335x_evm_map_io(void)
{
	omap2_set_globals_am33xx();
	omapam33xx_map_common_io();
}

MACHINE_START(APLEX_CMI_AT752, "aplex_CMI_AT752")
	/* Maintainer: Texas Instruments */
	.atag_offset	= 0x100,
	.map_io		= am335x_evm_map_io,
	.init_early	= am33xx_init_early,
	.init_irq	= ti81xx_init_irq,
	.handle_irq     = omap3_intc_handle_irq,
	.timer		= &omap3_am33xx_timer,
	.init_machine	= am335x_evm_init,
MACHINE_END
