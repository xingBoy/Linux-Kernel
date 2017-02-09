./drivers/misc/lis3lv02d/lis3lv02d.c:			input_set_capability(input_dev, EV_KEY, BTN_X);
./drivers/misc/lis3lv02d/lis3lv02d.c:			input_set_capability(input_dev, EV_KEY, BTN_Y);
./drivers/misc/lis3lv02d/lis3lv02d.c:			input_set_capability(input_dev, EV_KEY, BTN_Z);
./drivers/misc/ibmasm/remote.c:	mouse_dev->evbit[0]  = BIT_MASK(EV_KEY) | BIT_MASK(EV_ABS);
./drivers/misc/ibmasm/remote.c:	keybd_dev->evbit[0]  = BIT_MASK(EV_KEY);
./drivers/media/video/uvc/uvc_status.c:	__set_bit(EV_KEY, input->evbit);
./drivers/media/video/pwc/pwc-if.c:	pdev->button_dev->evbit[0] = BIT_MASK(EV_KEY);
./drivers/media/video/em28xx/em28xx-input.c:	input_dev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_REP);
./drivers/media/video/gspca/gspca.c:		input_dev->evbit[0] = BIT_MASK(EV_KEY);
./drivers/media/rc/imon.c:	idev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_REP) | BIT_MASK(EV_REL);
./drivers/media/rc/imon.c:		BIT_MASK(EV_KEY) | BIT_MASK(EV_ABS);
./drivers/media/rc/rc-main.c:	set_bit(EV_KEY, dev->input_dev->evbit);
./drivers/media/dvb/firewire/firedtv-rc.c:	idev->evbit[0] = BIT_MASK(EV_KEY);
./drivers/media/dvb/ttusb-dec/ttusb_dec.c:	input_dev->evbit[0] = BIT_MASK(EV_KEY);
./drivers/media/dvb/dvb-usb/dvb-usb-remote.c:			input_event(d->input_dev, EV_KEY, event, 1);
./drivers/media/dvb/dvb-usb/dvb-usb-remote.c:			input_event(d->input_dev, EV_KEY, d->last_event, 0);
./drivers/media/dvb/dvb-usb/dvb-usb-remote.c:				input_event(d->rc_input_dev, EV_KEY, d->last_event, 0);
./drivers/media/dvb/dvb-usb/dvb-usb-remote.c:			input_event(d->rc_input_dev, EV_KEY, event, 1);
./drivers/media/dvb/dvb-usb/dvb-usb-remote.c:				input_event(d->rc_input_dev, EV_KEY, d->last_event, 2);
./drivers/media/dvb/dvb-usb/dvb-usb-remote.c:	input_dev->evbit[0] = BIT_MASK(EV_KEY);
./drivers/media/dvb/ttpci/av7110_ir.c:			input_event(ir->input_dev, EV_KEY, ir->last_key, 0);
./drivers/media/dvb/ttpci/av7110_ir.c:			input_event(ir->input_dev, EV_KEY, keycode, 1);
./drivers/media/dvb/ttpci/av7110_ir.c:			input_event(ir->input_dev, EV_KEY, keycode, 2);
./drivers/media/dvb/ttpci/av7110_ir.c:		input_event(ir->input_dev, EV_KEY, keycode, 1);
./drivers/media/dvb/ttpci/av7110_ir.c:	set_bit(EV_KEY, ir->input_dev->evbit);
./drivers/input/joystick/spaceorb.c:	input_dev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_ABS);
./drivers/input/joystick/magellan.c:	input_dev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_ABS);
./drivers/input/joystick/adi.c:	input_dev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_ABS);
./drivers/input/joystick/grip_mp.c:	input_dev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_ABS);
./drivers/input/joystick/amijoy.c:		amijoy_dev[i]->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_ABS);
./drivers/input/joystick/analog.c:	input_dev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_ABS);
./drivers/input/joystick/a3d.c:		input_dev->evbit[0] |= BIT_MASK(EV_ABS) | BIT_MASK(EV_KEY) |
./drivers/input/joystick/a3d.c:		input_dev->evbit[0] |= BIT_MASK(EV_KEY) | BIT_MASK(EV_REL);
./drivers/input/joystick/db9.c:		input_dev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_ABS);
./drivers/input/joystick/tmdc.c:	input_dev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_ABS);
./drivers/input/joystick/warrior.c:	input_dev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_REL) |
./drivers/input/joystick/sidewinder.c:		input_dev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_ABS);
./drivers/input/joystick/grip.c:		input_dev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_ABS);
./drivers/input/joystick/guillemot.c:	input_dev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_ABS);
./drivers/input/joystick/cobra.c:		input_dev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_ABS);
./drivers/input/joystick/walkera0701.c:	w->input_dev->evbit[0] = BIT(EV_ABS) | BIT_MASK(EV_KEY);
./drivers/input/joystick/gf2k.c:	input_dev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_ABS);
./drivers/input/joystick/interact.c:	input_dev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_ABS);
./drivers/input/joystick/turbografx.c:		input_dev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_ABS);
./drivers/input/joystick/gamecon.c:		input_dev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_ABS);
./drivers/input/joystick/gamecon.c:		input_dev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_REL);
./drivers/input/joystick/xpad.c:	input_dev->evbit[0] = BIT_MASK(EV_KEY);
./drivers/input/joystick/spaceball.c:	input_dev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_ABS);
./drivers/input/joystick/as5011.c:	__set_bit(EV_KEY, input_dev->evbit);
./drivers/input/joystick/iforce/iforce-main.c:	input_dev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_ABS) |
./drivers/input/joystick/maplecontrol.c:		idev->evbit[0] |= BIT_MASK(EV_KEY);
./drivers/input/joystick/stinger.c:	input_dev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_ABS);
./drivers/input/joystick/twidjoy.c:	input_dev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_ABS);
./drivers/input/keyboard/bf54x-keys.c:	__set_bit(EV_KEY, input->evbit);
./drivers/input/keyboard/amikbd.c:	dev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_REP);
./drivers/input/keyboard/stmpe-keypad.c:	__set_bit(EV_KEY, input->evbit);
./drivers/input/keyboard/newtonkbd.c:	input_dev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_REP);
./drivers/input/keyboard/omap4-keypad.c:	__set_bit(EV_KEY, input_dev->evbit);
./drivers/input/keyboard/qt1070.c:	__set_bit(EV_KEY, input->evbit);
./drivers/input/keyboard/hil_kbd.c:	input_dev->evbit[0]	= BIT_MASK(EV_KEY) | BIT_MASK(EV_REP);
./drivers/input/keyboard/hil_kbd.c:		input_dev->evbit[0] |= BIT_MASK(EV_KEY);
./drivers/input/keyboard/adp5589-keys.c:	__set_bit(EV_KEY, input->evbit);
./drivers/input/keyboard/nomadik-ske-keypad.c:	__set_bit(EV_KEY, input->evbit);
./drivers/input/keyboard/omap-keypad.c:	__set_bit(EV_KEY, input_dev->evbit);
./drivers/input/keyboard/sunkbd.c:	input_dev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_LED) |
./drivers/input/keyboard/ep93xx_keypad.c:	input_dev->evbit[0] = BIT_MASK(EV_KEY);
./drivers/input/keyboard/tnetv107x-keypad.c:	kp->input_dev->evbit[0]	  = BIT_MASK(EV_KEY);
./drivers/input/keyboard/davinci_keyscan.c:	__set_bit(EV_KEY, key_dev->evbit);
./drivers/input/keyboard/samsung-keypad.c:	input_dev->evbit[0] = BIT_MASK(EV_KEY);
./drivers/input/keyboard/mpr121_touchkey.c:	input_dev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_REP);
./drivers/input/keyboard/mpr121_touchkey.c:		input_set_capability(input_dev, EV_KEY, pdata->keymap[i]);
./drivers/input/keyboard/adp5520-keys.c:	__set_bit(EV_KEY, input->evbit);
./drivers/input/keyboard/lm8323.c:	idev->evbit[0] = BIT(EV_KEY) | BIT(EV_MSC);
./drivers/input/keyboard/gpio_keys_polled.c:		unsigned int type = button->type ?: EV_KEY;
./drivers/input/keyboard/gpio_keys_polled.c:	input->evbit[0] = BIT(EV_KEY);
./drivers/input/keyboard/gpio_keys_polled.c:		unsigned int type = button->type ?: EV_KEY;
./drivers/input/keyboard/atkbd.c:		input_event(dev, EV_KEY, keycode, value);
./drivers/input/keyboard/atkbd.c:	input_dev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_REP) |
./drivers/input/keyboard/pxa27x_keypad.c:	input_dev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_REP);
./drivers/input/keyboard/w90p910_keypad.c:	input_dev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_REP);
./drivers/input/keyboard/imx_keypad.c:	input_dev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_REP);
./drivers/input/keyboard/jornada680_kbd.c:	input_dev->evbit[0] = BIT(EV_KEY) | BIT(EV_REP);
./drivers/input/keyboard/twl4030_keypad.c:	__set_bit(EV_KEY, input->evbit);
./drivers/input/keyboard/hilkbd.c:	hil_dev.dev->evbit[0]	= BIT_MASK(EV_KEY) | BIT_MASK(EV_REP);
./drivers/input/keyboard/opencores-kbd.c:	__set_bit(EV_KEY, input->evbit);
./drivers/input/keyboard/tca6416-keypad.c:			unsigned int type = button->type ?: EV_KEY;
./drivers/input/keyboard/tca6416-keypad.c:		type = (pdata->buttons[i].type) ?: EV_KEY;
./drivers/input/keyboard/matrix_keypad.c:	input_dev->evbit[0]	= BIT_MASK(EV_KEY);
./drivers/input/keyboard/gpio_keys.c: *	keys [ro]              - bitmap of keys (EV_KEY) which can be
./drivers/input/keyboard/gpio_keys.c: * @type: type of button (%EV_KEY, %EV_SW)
./drivers/input/keyboard/gpio_keys.c:	BUG_ON(type != EV_SW && type != EV_KEY);
./drivers/input/keyboard/gpio_keys.c:	return (type == EV_KEY) ? KEY_CNT : SW_CNT;
./drivers/input/keyboard/gpio_keys.c: * @type: button type (%EV_KEY, %EV_SW)
./drivers/input/keyboard/gpio_keys.c: * @type: button type (%EV_KEY, %EV_SW)
./drivers/input/keyboard/gpio_keys.c:ATTR_SHOW_FN(keys, EV_KEY, false);
./drivers/input/keyboard/gpio_keys.c:ATTR_SHOW_FN(disabled_keys, EV_KEY, true);
./drivers/input/keyboard/gpio_keys.c:ATTR_STORE_FN(disabled_keys, EV_KEY);
./drivers/input/keyboard/gpio_keys.c:	unsigned int type = button->type ?: EV_KEY;
./drivers/input/keyboard/gpio_keys.c:    // code: 116, value: 0, state: 0/1, type: EV_KEY;
./drivers/input/keyboard/gpio_keys.c:		unsigned int type = button->type ?: EV_KEY;
./drivers/input/keyboard/atakbd.c:	atakbd_dev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_REP);
./drivers/input/keyboard/jornada720_kbd.c:	input_dev->evbit[0] = BIT(EV_KEY) | BIT(EV_REP);
./drivers/input/keyboard/mcs_touchkey.c:	input_dev->evbit[0] = BIT_MASK(EV_KEY);
./drivers/input/keyboard/spear-keyboard.c:	__set_bit(EV_KEY, input_dev->evbit);
./drivers/input/keyboard/adp5588-keys.c:	__set_bit(EV_KEY, input->evbit);
./drivers/input/keyboard/qt2160.c:	__set_bit(EV_KEY, input->evbit);
./drivers/input/keyboard/max7359_keypad.c:	input_dev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_REP);
./drivers/input/keyboard/stowaway.c:	input_dev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_REP);
./drivers/input/keyboard/pxa930_rotary.c:		__set_bit(EV_KEY, input_dev->evbit);
./drivers/input/keyboard/tegra-kbc.c:	input_dev->evbit[0] = BIT_MASK(EV_KEY);
./drivers/input/keyboard/sh_keysc.c:			input_event(priv->input, EV_KEY, k, 1);
./drivers/input/keyboard/sh_keysc.c:			input_event(priv->input, EV_KEY, k, 0);
./drivers/input/keyboard/sh_keysc.c:	input->evbit[0] = BIT_MASK(EV_KEY);
./drivers/input/keyboard/locomokbd.c:	input_dev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_REP) |
./drivers/input/keyboard/xtkbd.c:	input_dev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_REP);
./drivers/input/keyboard/pmic8xxx-keypad.c:	kp->input->evbit[0]	= BIT_MASK(EV_KEY);
./drivers/input/keyboard/maple_keyb.c:	idev->evbit[0] = BIT(EV_KEY) | BIT(EV_REP);
./drivers/input/keyboard/lkkbd.c:	__set_bit(EV_KEY, input_dev->evbit);
./drivers/input/keyboard/tc3589x-keypad.c:	__set_bit(EV_KEY, input->evbit);
./drivers/input/misc/rb532_button.c:	input_set_capability(poll_dev->input, EV_KEY, RB532_BTN_KSYM);
./drivers/input/misc/atlas_btns.c:	__set_bit(EV_KEY, input_dev->evbit);
./drivers/input/misc/xen-kbdfront.c:	__set_bit(EV_KEY, kbd->evbit);
./drivers/input/misc/xen-kbdfront.c:	__set_bit(EV_KEY, ptr->evbit);
./drivers/input/misc/yealink.c:	input_dev->evbit[0] = BIT_MASK(EV_KEY);
./drivers/input/misc/gpio_matrix.c:							EV_KEY, keycode);
./drivers/input/misc/da9052_onkey.c:	da9052_onkey->input->evbit[0] = BIT_MASK(EV_KEY);
./drivers/input/misc/powermate.c:	input_dev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_REL) |
./drivers/input/misc/keyspan_remote.c:	__set_bit(EV_KEY, input_dev->evbit);
./drivers/input/misc/max8925_onkey.c:	info->idev->evbit[0] = BIT_MASK(EV_KEY);
./drivers/input/misc/ad714x.c:			__set_bit(EV_KEY, input[alloc_idx]->evbit);
./drivers/input/misc/ad714x.c:			__set_bit(EV_KEY, input[alloc_idx]->evbit);
./drivers/input/misc/ad714x.c:			__set_bit(EV_KEY, input[alloc_idx]->evbit);
./drivers/input/misc/ad714x.c:		__set_bit(EV_KEY, input[alloc_idx]->evbit);
./drivers/input/misc/pcf8574_keypad.c:	idev->evbit[0] = BIT_MASK(EV_KEY);
./drivers/input/misc/twl4030-pwrbutton.c:	pwr->evbit[0] = BIT_MASK(EV_KEY);
./drivers/input/misc/sgi_btns.c:	__set_bit(EV_KEY, input->evbit);
./drivers/input/misc/88pm860x_onkey.c:	info->idev->evbit[0] = BIT_MASK(EV_KEY);
./drivers/input/misc/ati_remote.c:	{KIND_LITERAL, 0x3d, 0x78, EV_KEY, BTN_LEFT, 1}, /* left btn down */
./drivers/input/misc/ati_remote.c:	{KIND_LITERAL, 0x3e, 0x79, EV_KEY, BTN_LEFT, 0}, /* left btn up */
./drivers/input/misc/ati_remote.c:	{KIND_LITERAL, 0x41, 0x7c, EV_KEY, BTN_RIGHT, 1},/* right btn down */
./drivers/input/misc/ati_remote.c:	{KIND_LITERAL, 0x42, 0x7d, EV_KEY, BTN_RIGHT, 0},/* right btn up */
./drivers/input/misc/ati_remote.c:	{KIND_FILTERED, 0x3f, 0x7a, EV_KEY, BTN_SIDE, 1}, /* left dblclick */
./drivers/input/misc/ati_remote.c:	{KIND_FILTERED, 0x43, 0x7e, EV_KEY, BTN_EXTRA, 1},/* right dblclick */
./drivers/input/misc/ati_remote.c:	{KIND_FILTERED, 0xd2, 0x0d, EV_KEY, KEY_1, 1},
./drivers/input/misc/ati_remote.c:	{KIND_FILTERED, 0xd3, 0x0e, EV_KEY, KEY_2, 1},
./drivers/input/misc/ati_remote.c:	{KIND_FILTERED, 0xd4, 0x0f, EV_KEY, KEY_3, 1},
./drivers/input/misc/ati_remote.c:	{KIND_FILTERED, 0xd5, 0x10, EV_KEY, KEY_4, 1},
./drivers/input/misc/ati_remote.c:	{KIND_FILTERED, 0xd6, 0x11, EV_KEY, KEY_5, 1},
./drivers/input/misc/ati_remote.c:	{KIND_FILTERED, 0xd7, 0x12, EV_KEY, KEY_6, 1},
./drivers/input/misc/ati_remote.c:	{KIND_FILTERED, 0xd8, 0x13, EV_KEY, KEY_7, 1},
./drivers/input/misc/ati_remote.c:	{KIND_FILTERED, 0xd9, 0x14, EV_KEY, KEY_8, 1},
./drivers/input/misc/ati_remote.c:	{KIND_FILTERED, 0xda, 0x15, EV_KEY, KEY_9, 1},
./drivers/input/misc/ati_remote.c:	{KIND_FILTERED, 0xdc, 0x17, EV_KEY, KEY_0, 1},
./drivers/input/misc/ati_remote.c:	{KIND_FILTERED, 0xc5, 0x00, EV_KEY, KEY_A, 1},
./drivers/input/misc/ati_remote.c:	{KIND_FILTERED, 0xc6, 0x01, EV_KEY, KEY_B, 1},
./drivers/input/misc/ati_remote.c:	{KIND_FILTERED, 0xde, 0x19, EV_KEY, KEY_C, 1},
./drivers/input/misc/ati_remote.c:	{KIND_FILTERED, 0xe0, 0x1b, EV_KEY, KEY_D, 1},
./drivers/input/misc/ati_remote.c:	{KIND_FILTERED, 0xe6, 0x21, EV_KEY, KEY_E, 1},
./drivers/input/misc/ati_remote.c:	{KIND_FILTERED, 0xe8, 0x23, EV_KEY, KEY_F, 1},
./drivers/input/misc/ati_remote.c:	{KIND_FILTERED, 0xdd, 0x18, EV_KEY, KEY_KPENTER, 1},    /* "check" */
./drivers/input/misc/ati_remote.c:	{KIND_FILTERED, 0xdb, 0x16, EV_KEY, KEY_MENU, 1},       /* "menu" */
./drivers/input/misc/ati_remote.c:	{KIND_FILTERED, 0xc7, 0x02, EV_KEY, KEY_POWER, 1},      /* Power */
./drivers/input/misc/ati_remote.c:	{KIND_FILTERED, 0xc8, 0x03, EV_KEY, KEY_TV, 1},         /* TV */
./drivers/input/misc/ati_remote.c:	{KIND_FILTERED, 0xc9, 0x04, EV_KEY, KEY_DVD, 1},        /* DVD */
./drivers/input/misc/ati_remote.c:	{KIND_FILTERED, 0xca, 0x05, EV_KEY, KEY_WWW, 1},        /* WEB */
./drivers/input/misc/ati_remote.c:	{KIND_FILTERED, 0xcb, 0x06, EV_KEY, KEY_BOOKMARKS, 1},  /* "book" */
./drivers/input/misc/ati_remote.c:	{KIND_FILTERED, 0xcc, 0x07, EV_KEY, KEY_EDIT, 1},       /* "hand" */
./drivers/input/misc/ati_remote.c:	{KIND_FILTERED, 0xe1, 0x1c, EV_KEY, KEY_COFFEE, 1},     /* "timer" */
./drivers/input/misc/ati_remote.c:	{KIND_FILTERED, 0xe5, 0x20, EV_KEY, KEY_FRONT, 1},      /* "max" */
./drivers/input/misc/ati_remote.c:	{KIND_FILTERED, 0xe2, 0x1d, EV_KEY, KEY_LEFT, 1},       /* left */
./drivers/input/misc/ati_remote.c:	{KIND_FILTERED, 0xe4, 0x1f, EV_KEY, KEY_RIGHT, 1},      /* right */
./drivers/input/misc/ati_remote.c:	{KIND_FILTERED, 0xe7, 0x22, EV_KEY, KEY_DOWN, 1},       /* down */
./drivers/input/misc/ati_remote.c:	{KIND_FILTERED, 0xdf, 0x1a, EV_KEY, KEY_UP, 1},         /* up */
./drivers/input/misc/ati_remote.c:	{KIND_FILTERED, 0xe3, 0x1e, EV_KEY, KEY_OK, 1},         /* "OK" */
./drivers/input/misc/ati_remote.c:	{KIND_FILTERED, 0xce, 0x09, EV_KEY, KEY_VOLUMEDOWN, 1}, /* VOL + */
./drivers/input/misc/ati_remote.c:	{KIND_FILTERED, 0xcd, 0x08, EV_KEY, KEY_VOLUMEUP, 1},   /* VOL - */
./drivers/input/misc/ati_remote.c:	{KIND_FILTERED, 0xcf, 0x0a, EV_KEY, KEY_MUTE, 1},       /* MUTE  */
./drivers/input/misc/ati_remote.c:	{KIND_FILTERED, 0xd0, 0x0b, EV_KEY, KEY_CHANNELUP, 1},  /* CH + */
./drivers/input/misc/ati_remote.c:	{KIND_FILTERED, 0xd1, 0x0c, EV_KEY, KEY_CHANNELDOWN, 1},/* CH - */
./drivers/input/misc/ati_remote.c:	{KIND_FILTERED, 0xec, 0x27, EV_KEY, KEY_RECORD, 1},     /* ( o) red */
./drivers/input/misc/ati_remote.c:	{KIND_FILTERED, 0xea, 0x25, EV_KEY, KEY_PLAY, 1},       /* ( >) */
./drivers/input/misc/ati_remote.c:	{KIND_FILTERED, 0xe9, 0x24, EV_KEY, KEY_REWIND, 1},     /* (<<) */
./drivers/input/misc/ati_remote.c:	{KIND_FILTERED, 0xeb, 0x26, EV_KEY, KEY_FORWARD, 1},    /* (>>) */
./drivers/input/misc/ati_remote.c:	{KIND_FILTERED, 0xed, 0x28, EV_KEY, KEY_STOP, 1},       /* ([]) */
./drivers/input/misc/ati_remote.c:	{KIND_FILTERED, 0xee, 0x29, EV_KEY, KEY_PAUSE, 1},      /* ('') */
./drivers/input/misc/ati_remote.c:	{KIND_FILTERED, 0xf0, 0x2b, EV_KEY, KEY_PREVIOUS, 1},   /* (<-) */
./drivers/input/misc/ati_remote.c:	{KIND_FILTERED, 0xef, 0x2a, EV_KEY, KEY_NEXT, 1},       /* (>+) */
./drivers/input/misc/ati_remote.c:	{KIND_FILTERED, 0xf2, 0x2D, EV_KEY, KEY_INFO, 1},       /* PLAYING */
./drivers/input/misc/ati_remote.c:	{KIND_FILTERED, 0xf3, 0x2E, EV_KEY, KEY_HOME, 1},       /* TOP */
./drivers/input/misc/ati_remote.c:	{KIND_FILTERED, 0xf4, 0x2F, EV_KEY, KEY_END, 1},        /* END */
./drivers/input/misc/ati_remote.c:	{KIND_FILTERED, 0xf5, 0x30, EV_KEY, KEY_SELECT, 1},     /* SELECT */
./drivers/input/misc/ati_remote.c:	idev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_REL);
./drivers/input/misc/ati_remote.c:		if (ati_remote_tbl[i].type == EV_KEY)
./drivers/input/misc/adxl34x.c:	.ev_code_tap = {BTN_TOUCH, BTN_TOUCH, BTN_TOUCH}, /* EV_KEY {x,y,z} */
./drivers/input/misc/adxl34x.c:	__set_bit(EV_KEY, input_dev->evbit);
./drivers/input/misc/ati_remote2.c:	input_event(idev, EV_KEY, ar2->keycode[mode][index], data[1]);
./drivers/input/misc/ati_remote2.c:	idev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_REP) | BIT_MASK(EV_REL);
./drivers/input/misc/pcap_keys.c:	__set_bit(EV_KEY, input_dev->evbit);
./drivers/input/misc/ab8500-ponkey.c:	input_set_capability(input, EV_KEY, KEY_POWER);
./drivers/input/misc/bfin_rotary.c:		__set_bit(EV_KEY, input->evbit);
./drivers/input/misc/bfin_rotary.c:		__set_bit(EV_KEY, input->evbit);
./drivers/input/misc/cm109.c:	input_dev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_SND);
./drivers/input/misc/pcf50633-input.c:	input_dev->evbit[0] = BIT(EV_KEY) | BIT(EV_PWR);
./drivers/input/misc/apanel.c:	set_bit(EV_KEY, idev->evbit);
./drivers/input/misc/pmic8xxx-pwrkey.c:	input_set_capability(pwr, EV_KEY, KEY_POWER);
./drivers/input/misc/wm831x-on.c:	wm831x_on->dev->evbit[0] = BIT_MASK(EV_KEY);
./drivers/input/misc/cobalt_btns.c:	__set_bit(EV_KEY, input->evbit);
./drivers/input/misc/keychord.c:	if (type != EV_KEY || code >= KEY_MAX)
./drivers/input/misc/keychord.c:	__set_bit(EV_KEY, kdev->device_ids[0].evbit);
./drivers/input/keyreset.c:	if (type != EV_KEY)
./drivers/input/keyreset.c:		.evbit = { BIT_MASK(EV_KEY) },
./drivers/input/tablet/hanwang.c:	EV_KEY, EV_ABS, EV_MSC,
./drivers/input/tablet/gtco.c:	inputdev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_ABS) |
./drivers/input/tablet/aiptek.c:        EV_KEY, EV_ABS, EV_REL, EV_MSC,
./drivers/input/tablet/wacom_wac.c:	input_dev->evbit[0] |= BIT_MASK(EV_KEY) | BIT_MASK(EV_ABS);
./drivers/input/tablet/acecad.c:	input_dev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_ABS);
./drivers/input/tablet/kbtab.c:	input_dev->evbit[0] |= BIT_MASK(EV_KEY) | BIT_MASK(EV_ABS);
./drivers/input/evdev.c:	case EV_KEY: bits = dev->keybit; len = KEY_MAX; break;
./drivers/input/evdev.c:	 * EVIOCGBIT(EV_KEY, KEY_MAX) and not realize that 'len'
./drivers/input/evdev.c:	if (type == EV_KEY && size == OLD_KEY_MAX) {
./drivers/input/touchscreen/ar1020-spi.c:	input_dev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_ABS);
./drivers/input/touchscreen/w90p910_ts.c:	input_dev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_ABS);
./drivers/input/touchscreen/mc13783_ts.c:	idev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_ABS);
./drivers/input/touchscreen/atmel_tsadcc.c:	input_set_capability(input_dev, EV_KEY, BTN_TOUCH);
./drivers/input/touchscreen/pcap_ts.c:	input_dev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_ABS);
./drivers/input/touchscreen/s3c2410_ts.c:	ts.input->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_ABS);
./drivers/input/touchscreen/jornada720_ts.c:	input_dev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_ABS);
./drivers/input/touchscreen/max11801_ts.c:					input_event(data->input_dev, EV_KEY, BTN_TOUCH, 1);
./drivers/input/touchscreen/max11801_ts.c:					input_event(data->input_dev, EV_KEY, BTN_TOUCH, 0);
./drivers/input/touchscreen/max11801_ts.c:					input_event(data->input_dev, EV_KEY, BTN_TOUCH, 1);
./drivers/input/touchscreen/max11801_ts.c:					input_event(data->input_dev, EV_KEY, BTN_TOUCH, 0);
./drivers/input/touchscreen/max11801_ts.c:	__set_bit(EV_KEY, input_dev->evbit);
./drivers/input/touchscreen/88pm860x-ts.c:	__set_bit(EV_KEY, touch->idev->evbit);
./drivers/input/touchscreen/wm97xx-core.c:	__set_bit(EV_KEY, wm->input_dev->evbit);
./drivers/input/touchscreen/ucb1400_ts.c:	ucb->ts_idev->evbit[0]		= BIT_MASK(EV_ABS) | BIT_MASK(EV_KEY);
./drivers/input/touchscreen/touchright.c:	input_dev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_ABS);
./drivers/input/touchscreen/bu21013_ts.c:	__set_bit(EV_KEY, in_dev->evbit);
./drivers/input/touchscreen/ads7846.c:	input_dev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_ABS);
./drivers/input/touchscreen/tsc2005.c:	input_dev->evbit[0] = BIT(EV_ABS) | BIT(EV_KEY);
./drivers/input/touchscreen/mcs5000_ts.c:	__set_bit(EV_KEY, input_dev->evbit);
./drivers/input/touchscreen/tsc2007.c:	input_dev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_ABS);
./drivers/input/touchscreen/h3600_ts_input.c:	input_dev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_ABS) |
./drivers/input/touchscreen/penmount.c:        input_dev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_ABS);
./drivers/input/touchscreen/lpc32xx_ts.c:	input->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_ABS);
./drivers/input/touchscreen/atmel_mxt_ts.c:	__set_bit(EV_KEY, input_dev->evbit);
./drivers/input/touchscreen/fujitsu_ts.c:	input_dev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_ABS);
./drivers/input/touchscreen/stmpe-ts.c:	idev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_ABS);
./drivers/input/touchscreen/wm831x-ts.c:	__set_bit(EV_KEY, input_dev->evbit);
./drivers/input/touchscreen/elo.c:	input_dev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_ABS);
./drivers/input/touchscreen/ar1020-spi.android.c:						input_event(priv->input, EV_KEY,KEY_MENU, 1);
./drivers/input/touchscreen/ar1020-spi.android.c:						input_event(priv->input, EV_KEY,KEY_HOME, 1);
./drivers/input/touchscreen/ar1020-spi.android.c:						input_event(priv->input, EV_KEY,KEY_BACK, 1);
./drivers/input/touchscreen/ar1020-spi.android.c:			    input_event(priv->input, EV_KEY, KEY_BACK, 0);  
./drivers/input/touchscreen/ar1020-spi.android.c:			    input_event(priv->input, EV_KEY, KEY_HOME, 0);  
./drivers/input/touchscreen/ar1020-spi.android.c:	            input_event(priv->input, EV_KEY, KEY_MENU, 0);  
./drivers/input/touchscreen/ar1020-spi.android.c:	__set_bit(EV_KEY, input_dev->evbit);
./drivers/input/touchscreen/ar1020-spi.android.c://	input_dev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_ABS);
./drivers/input/touchscreen/migor_ts.c:	input->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_ABS);
./drivers/input/touchscreen/hampshire.c:	input_dev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_ABS);
./drivers/input/touchscreen/p1003_ts.c:			input_event(input, EV_KEY, BTN_TOUCH, 1);
./drivers/input/touchscreen/p1003_ts.c:	input_event(input, EV_KEY, BTN_TOUCH, 0);
./drivers/input/touchscreen/p1003_ts.c:	__set_bit(EV_KEY, input_dev->evbit);
./drivers/input/touchscreen/htcpen.c:	htcpen_dev->evbit[0] = BIT_MASK(EV_ABS) | BIT_MASK(EV_KEY);
./drivers/input/touchscreen/cy8ctmg110_ts.c:	input_dev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_ABS);
./drivers/input/touchscreen/ad7877.c:	__set_bit(EV_KEY, input_dev->evbit);
./drivers/input/touchscreen/ft5x06_ts.c:	__set_bit(EV_KEY, input_dev->evbit);
./drivers/input/touchscreen/intel-mid-touch.c:	input->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_ABS);
./drivers/input/touchscreen/touchit213.c:	input_dev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_ABS);
./drivers/input/touchscreen/eeti_ts.c:	input->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_ABS);
./drivers/input/touchscreen/novatek_ts.c:	__set_bit(EV_KEY, ts->input_dev->evbit);
./drivers/input/touchscreen/ad7879.c:	__set_bit(EV_KEY, input_dev->evbit);
./drivers/input/touchscreen/wacom_w8001.c:	dev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_ABS);
./drivers/input/touchscreen/tps6507x-ts.c:	input_dev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_ABS);
./drivers/input/touchscreen/mk712.c:	mk712_dev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_ABS);
./drivers/input/touchscreen/da9034-ts.c:	__set_bit(EV_KEY, input_dev->evbit);
./drivers/input/touchscreen/hp680_ts_input.c:	hp680_ts_dev->evbit[0] = BIT_MASK(EV_ABS) | BIT_MASK(EV_KEY);
./drivers/input/touchscreen/gt9xx.c:    ts->pen_dev->evbit[0] = BIT_MASK(EV_SYN) | BIT_MASK(EV_KEY) | BIT_MASK(EV_ABS) ;
./drivers/input/touchscreen/gt9xx.c:    input_set_capability(ts->pen_dev, EV_KEY, BTN_STYLUS);
./drivers/input/touchscreen/gt9xx.c:    input_set_capability(ts->pen_dev, EV_KEY, BTN_STYLUS2);
./drivers/input/touchscreen/gt9xx.c:    ts->input_dev->evbit[0] = BIT_MASK(EV_SYN) | BIT_MASK(EV_KEY) | BIT_MASK(EV_ABS) ;
./drivers/input/touchscreen/gt9xx.c:        input_set_capability(ts->input_dev, EV_KEY, touch_key_array[index]);  
./drivers/input/touchscreen/gt9xx.c:    input_set_capability(ts->input_dev, EV_KEY, KEY_POWER);
./drivers/input/touchscreen/dynapro.c:	input_dev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_ABS);
./drivers/input/touchscreen/st1232.c:	__set_bit(EV_KEY, input_dev->evbit);
./drivers/input/touchscreen/da9052_tsi.c:			BIT_MASK(EV_KEY) |
./drivers/input/touchscreen/gunze.c:	input_dev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_ABS);
./drivers/input/touchscreen/elan_ts.c:	set_bit(EV_KEY, elan_touch_data.input->evbit);
./drivers/input/touchscreen/mtouch.c:	input_dev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_ABS);
./drivers/input/touchscreen/inexio.c:	input_dev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_ABS);
./drivers/input/touchscreen/tnetv107x-ts.c:	__set_bit(EV_KEY,    ts->input_dev->evbit);
./drivers/input/touchscreen/usbtouchscreen.c:	input_dev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_ABS);
./drivers/input/touchscreen/gt9xx/gt9xx.c:    ts->pen_dev->evbit[0] = BIT_MASK(EV_SYN) | BIT_MASK(EV_KEY) | BIT_MASK(EV_ABS) ;
./drivers/input/touchscreen/gt9xx/gt9xx.c:    input_set_capability(ts->pen_dev, EV_KEY, BTN_STYLUS);
./drivers/input/touchscreen/gt9xx/gt9xx.c:    input_set_capability(ts->pen_dev, EV_KEY, BTN_STYLUS2);
./drivers/input/touchscreen/gt9xx/gt9xx.c:    ts->input_dev->evbit[0] = BIT_MASK(EV_SYN) | BIT_MASK(EV_KEY) | BIT_MASK(EV_ABS) ;
./drivers/input/touchscreen/gt9xx/gt9xx.c:        input_set_capability(ts->input_dev, EV_KEY, touch_key_array[index]);  
./drivers/input/touchscreen/gt9xx/gt9xx.c:    input_set_capability(ts->input_dev, EV_KEY, KEY_POWER);
./drivers/input/touchscreen/touchwin.c:	input_dev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_ABS);
./drivers/input/touchscreen/synaptics_i2c_rmi.c:	set_bit(EV_KEY, ts->input_dev->evbit);
./drivers/input/mousedev.c:	case EV_KEY:
./drivers/input/mousedev.c:		.evbit = { BIT_MASK(EV_KEY) | BIT_MASK(EV_REL) },
./drivers/input/mousedev.c:		.evbit = { BIT_MASK(EV_KEY) | BIT_MASK(EV_REL) },
./drivers/input/mousedev.c:		.evbit = { BIT_MASK(EV_KEY) | BIT_MASK(EV_ABS) },
./drivers/input/mousedev.c:		.evbit = { BIT_MASK(EV_KEY) | BIT_MASK(EV_ABS) },
./drivers/input/mousedev.c:		.evbit = { BIT_MASK(EV_KEY) | BIT_MASK(EV_ABS) },
./drivers/input/mouse/alps.c:	dev1->evbit[BIT_WORD(EV_KEY)] |= BIT_MASK(EV_KEY);
./drivers/input/mouse/alps.c:	dev2->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_REL);
./drivers/input/mouse/hgpk.c:	__set_bit(EV_KEY, input->evbit);
./drivers/input/mouse/vsxxxaa.c:	__set_bit(EV_KEY, input_dev->evbit);		/* We have buttons */
./drivers/input/mouse/synaptics_i2c.c:	__set_bit(EV_KEY, input->evbit);
./drivers/input/mouse/psmouse-base.c:	input_dev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_REL);
./drivers/input/mouse/pc110pad.c:	pc110pad_dev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_ABS);
./drivers/input/mouse/bcm5974.c:	__set_bit(EV_KEY, input_dev->evbit);
./drivers/input/mouse/lifebook.c:	dev2->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_REL);
./drivers/input/mouse/lifebook.c:	dev1->evbit[0] = BIT_MASK(EV_ABS) | BIT_MASK(EV_KEY);
./drivers/input/mouse/gpio_mouse.c:		input_set_capability(input, EV_KEY, BTN_LEFT);
./drivers/input/mouse/gpio_mouse.c:		input_set_capability(input, EV_KEY, BTN_MIDDLE);
./drivers/input/mouse/gpio_mouse.c:		input_set_capability(input, EV_KEY, BTN_RIGHT);
./drivers/input/mouse/rpcmouse.c:	rpcmouse_dev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_REL);
./drivers/input/mouse/inport.c:	inport_dev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_REL);
./drivers/input/mouse/amimouse.c:	dev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_REL);
./drivers/input/mouse/appletouch.c:	set_bit(EV_KEY, input_dev->evbit);
./drivers/input/mouse/atarimouse.c:	atamouse_dev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_REL);
./drivers/input/mouse/logibm.c:	logibm_dev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_REL);
./drivers/input/mouse/maplemouse.c:	input_dev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_REL);
./drivers/input/mouse/sermouse.c:	input_dev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_REL);
./drivers/input/mouse/elantech.c:	__set_bit(EV_KEY, dev->evbit);
./drivers/input/mouse/synaptics.c:	__set_bit(EV_KEY, dev->evbit);
./drivers/input/mouse/touchkit_ps2.c:		dev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_ABS);
./drivers/input/input.c:		input_pass_event(dev, EV_KEY, dev->repeat_key, 2);
./drivers/input/input.c:	case EV_KEY:
./drivers/input/input.c:	if (is_event_supported(EV_KEY, dev->evbit, EV_MAX)) {
./drivers/input/input.c:				input_pass_event(dev, EV_KEY, code, 0);
./drivers/input/input.c:	if (test_bit(EV_KEY, dev->evbit) &&
./drivers/input/input.c:		input_pass_event(dev, EV_KEY, old_keycode, 0);
./drivers/input/input.c:	if (test_bit(EV_KEY, dev->evbit))
./drivers/input/input.c:	if (test_bit(EV_KEY, dev->evbit))
./drivers/input/input.c: * @type: type of the event (EV_KEY, EV_REL, etc...)
./drivers/input/input.c:	case EV_KEY:
./drivers/input/joydev.c:	case EV_KEY:
./drivers/input/joydev.c:	if (test_bit(EV_KEY, dev->evbit) && test_bit(BTN_TOUCH, dev->keybit))
./drivers/input/joydev.c:	if (test_bit(EV_KEY, dev->evbit) && test_bit(BTN_DIGI, dev->keybit))
./drivers/input/joydev.c:		.evbit = { BIT_MASK(EV_KEY) },
./drivers/input/joydev.c:		.evbit = { BIT_MASK(EV_KEY) },
./drivers/input/joydev.c:		.evbit = { BIT_MASK(EV_KEY) },
./drivers/input/input-mt.c:	input_event(dev, EV_KEY, BTN_TOOL_FINGER, count == 1);
./drivers/input/input-mt.c:	input_event(dev, EV_KEY, BTN_TOOL_DOUBLETAP, count == 2);
./drivers/input/input-mt.c:	input_event(dev, EV_KEY, BTN_TOOL_TRIPLETAP, count == 3);
./drivers/input/input-mt.c:	input_event(dev, EV_KEY, BTN_TOOL_QUADTAP, count == 4);
./drivers/input/input-mt.c:	input_event(dev, EV_KEY, BTN_TOUCH, count > 0);
./drivers/input/sparse-keymap.c:			__set_bit(EV_KEY, dev->evbit);
./drivers/input/sparse-keymap.c:	if (test_bit(EV_KEY, dev->evbit)) {
./drivers/hid/hid-debug.c:	[EV_SYN] = "Sync",			[EV_KEY] = "Key",
./drivers/hid/hid-debug.c:	[EV_SYN] = syncs,			[EV_KEY] = keys,
./drivers/hid/hid-lg.c:		EV_KEY, (c))
./drivers/hid/hid-lg.c:			hid_map_usage(hi, usage, bit, max, EV_KEY,
./drivers/hid/hid-lg.c:	if ((quirks & LG_BAD_RELATIVE_KEYS) && usage->type == EV_KEY &&
./drivers/hid/hid-lg.c:	if ((quirks & LG_DUPLICATE_USAGES) && (usage->type == EV_KEY ||
./drivers/hid/hid-sunplus.c:		EV_KEY, (c))
./drivers/hid/hid-wacom.c:	input->evbit[0] |= BIT(EV_KEY) | BIT(EV_ABS) | BIT(EV_REL);
./drivers/hid/hid-gyration.c:					EV_KEY, (c))
./drivers/hid/hid-zydacron.c:	hid_map_usage_clear(hi, usage, bit, max, EV_KEY, (c))
./drivers/hid/hid-zydacron.c:				input_event(zc->input_ep81, EV_KEY, key, 0);
./drivers/hid/hid-zydacron.c:				input_event(zc->input_ep81, EV_KEY, key, 1);
./drivers/hid/hid-ntrig.c:	if (usage->type == EV_KEY || usage->type == EV_REL
./drivers/hid/hid-prodikeys.c:			input_event(pm->input_ep82, EV_KEY,
./drivers/hid/hid-prodikeys.c:			input_event(pm->input_ep82, EV_KEY, key, 1);
./drivers/hid/hid-monterey.c:					EV_KEY, (c))
./drivers/hid/hid-lcpower.c:					EV_KEY, (c))
./drivers/hid/usbhid/usbkbd.c:	input_dev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_LED) |
./drivers/hid/usbhid/usbmouse.c:	input_dev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_REL);
./drivers/hid/hid-samsung.c:	hid_map_usage_clear(hi, usage, bit, max, EV_KEY, (c))
./drivers/hid/hid-petalynx.c:					EV_KEY, (c))
./drivers/hid/hid-magicmouse.c:	__set_bit(EV_KEY, input->evbit);
./drivers/hid/hid-microsoft.c:					EV_KEY, (c))
./drivers/hid/hid-kensington.c:#define ks_map_key(c)	hid_map_usage(hi, usage, bit, max, EV_KEY, (c))
./drivers/hid/hid-belkin.c:					EV_KEY, (c))
./drivers/hid/hid-twinhan.c:					EV_KEY, (c))
./drivers/hid/hid-chicony.c:					EV_KEY, (c))
./drivers/hid/hid-quanta.c:			hid_map_usage(hi, usage, bit, max, EV_KEY, BTN_TOUCH);
./drivers/hid/hid-quanta.c:	if (usage->type == EV_KEY || usage->type == EV_ABS)
./drivers/hid/hid-quanta.c:			input_event(input, EV_KEY, BTN_TOUCH, 0);
./drivers/hid/hid-quanta.c:			input_event(input, EV_KEY, BTN_TOUCH, 1);
./drivers/hid/hid-cherry.c:					EV_KEY, (c))
./drivers/hid/hid-apple.c:		hid_map_usage_clear(hi, usage, bit, max, EV_KEY, KEY_FN);
./drivers/hid/hid-apple.c:			hid_map_usage(hi, usage, bit, max, EV_KEY, BTN_2);
./drivers/hid/hid-apple.c:			hid_map_usage(hi, usage, bit, max, EV_KEY, BTN_1);
./drivers/hid/hid-topseed.c:					EV_KEY, (c))
./drivers/hid/hid-ezkey.c:#define ez_map_key(c)	hid_map_usage(hi, usage, bit, max, EV_KEY, (c))
./drivers/hid/hid-picolcd.c:		input_set_capability(idev, EV_KEY, data->keycode[i]);
./drivers/hid/hid-input.c:#define map_key(c)	hid_map_usage(hidinput, usage, &bit, &max, EV_KEY, (c))
./drivers/hid/hid-input.c:		&max, EV_KEY, (c))
./drivers/hid/hid-input.c:	return (usage->type == EV_KEY && usage->code == keycode);
./drivers/hid/hid-input.c:					if (usage->type == EV_KEY || usage->type == 0) {
./drivers/hid/hid-input.c:		ke->keycode = usage->type == EV_KEY ?
./drivers/hid/hid-input.c:		*old_keycode = usage->type == EV_KEY ?
./drivers/hid/hid-input.c:				set_bit(EV_KEY, input->evbit);
./drivers/hid/hid-input.c:	if (usage->type == EV_KEY) {
./drivers/hid/hid-input.c:		input_event(input, EV_KEY, BTN_TOUCH, value > a + ((b - a) >> 3));
./drivers/hid/hid-input.c:	if ((usage->type == EV_KEY) && (usage->code == 0)) /* Key 0 is "unassigned", not KEY_UNKNOWN */
./drivers/hid/hid-input.c:			input_event(input, EV_KEY, direction, 1);
./drivers/hid/hid-input.c:			input_event(input, EV_KEY, direction, 0);
./drivers/hid/hid-input.c:	if (usage->type == EV_KEY && !!test_bit(usage->code, input->key) != value)
./drivers/hid/hid-input.c:	if ((field->flags & HID_MAIN_ITEM_RELATIVE) && (usage->type == EV_KEY))
./drivers/hid/hid-multitouch.c:			hid_map_usage(hi, usage, bit, max, EV_KEY, BTN_TOUCH);
./drivers/hid/hid-multitouch.c:			input_set_capability(hi->input, EV_KEY, BTN_TOUCH);
./drivers/hid/hid-multitouch.c:	if (usage->type == EV_KEY || usage->type == EV_ABS)
./drivers/usb/storage/onetouch.c:	set_bit(EV_KEY, input_dev->evbit);
./drivers/char/sonypi.c:	jog_dev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_REL);
./drivers/char/sonypi.c:	key_dev->evbit[0] = BIT_MASK(EV_KEY);
./drivers/acpi/video.c:	input->evbit[0] = BIT(EV_KEY);
./drivers/acpi/button.c:		input->evbit[0] = BIT_MASK(EV_KEY);
./drivers/acpi/button.c:		input->evbit[0] = BIT_MASK(EV_KEY);
./drivers/staging/speakup/fakekey.c:	__set_bit(EV_KEY, virt_keyboard->evbit);
./drivers/staging/quickstart/quickstart.c:		set_bit(EV_KEY, quickstart_input->evbit);
./drivers/staging/ste_rmi4/synaptics_i2c_rmi4.c:	set_bit(EV_KEY, rmi4_data->input_dev->evbit);
./drivers/staging/nvec/nvec_kbd.c:	idev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_REP) | BIT_MASK(EV_LED);
./drivers/staging/cptm1217/clearpad_tm1217.c:		input_dev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_ABS);
./drivers/macintosh/adbhid.c:		input_dev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_LED) |
./drivers/macintosh/adbhid.c:		input_dev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_REL);
./drivers/macintosh/adbhid.c:			input_dev->evbit[0] = BIT_MASK(EV_KEY) |
./drivers/macintosh/adbhid.c:			input_dev->evbit[0] = BIT_MASK(EV_KEY) |
./drivers/macintosh/ams/ams-input.c:	set_bit(EV_KEY, input->evbit);
./drivers/macintosh/via-pmu-event.c:	set_bit(EV_KEY, pmu_input_dev->evbit);
./drivers/macintosh/mac_hid.c:	mac_hid_emumouse_dev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_REL);
./drivers/macintosh/mac_hid.c:	if (type != EV_KEY)
./drivers/macintosh/mac_hid.c:		.evbit = { BIT_MASK(EV_KEY) },
./drivers/net/wireless/ath/carl9170/main.c:	input_set_capability(input, EV_KEY, KEY_WPS_BUTTON);
./drivers/mfd/ucb1x00-ts.c:	idev->evbit[0]   = BIT_MASK(EV_ABS) | BIT_MASK(EV_KEY);
./drivers/tty/vt/keyboard.c:	if (event_type == EV_KEY)
./drivers/tty/vt/keyboard.c:	if (test_bit(EV_KEY, dev->evbit)) {
./drivers/tty/vt/keyboard.c:                .evbit = { BIT_MASK(EV_KEY) },
./drivers/tty/sysrq.c:		input_inject_event(handle, EV_KEY, alt_code, 1);
./drivers/tty/sysrq.c:		input_inject_event(handle, EV_KEY, KEY_SYSRQ, 1);
./drivers/tty/sysrq.c:		input_inject_event(handle, EV_KEY, KEY_SYSRQ, 0);
./drivers/tty/sysrq.c:		input_inject_event(handle, EV_KEY, alt_code, 0);
./drivers/tty/sysrq.c:	case EV_KEY:
./drivers/tty/sysrq.c:		.evbit = { BIT_MASK(EV_KEY) },
./drivers/tty/serial/kgdboc.c:		.evbit = { BIT_MASK(EV_KEY) },
./drivers/platform/x86/thinkpad_acpi.c:	TP_HKEY_EV_KEY_NUMLOCK		= 0x6000, /* NumLock key pressed */
./drivers/platform/x86/thinkpad_acpi.c:	TP_HKEY_EV_KEY_FN		= 0x6005, /* Fn key pressed? E420 */
./drivers/platform/x86/thinkpad_acpi.c:			input_set_capability(tpacpi_inputdev, EV_KEY,
./drivers/platform/x86/thinkpad_acpi.c:	case TP_HKEY_EV_KEY_NUMLOCK:
./drivers/platform/x86/thinkpad_acpi.c:	case TP_HKEY_EV_KEY_FN:
./drivers/platform/x86/intel_mid_powerbtn.c:		input_event(input, EV_KEY, KEY_POWER,
./drivers/platform/x86/intel_mid_powerbtn.c:	input_set_capability(input, EV_KEY, KEY_POWER);
./drivers/platform/x86/classmate-laptop.c:	set_bit(EV_KEY, inputdev->evbit);
./drivers/platform/x86/sony-laptop.c:	__set_bit(EV_KEY, key_dev->evbit);
./drivers/platform/x86/sony-laptop.c:	input_set_capability(jog_dev, EV_KEY, BTN_MIDDLE);
./drivers/platform/x86/fujitsu-laptop.c:	input->evbit[0] = BIT(EV_KEY);
./drivers/platform/x86/fujitsu-laptop.c:	set_bit(EV_KEY, input->evbit);
./arch/powerpc/platforms/cell/cbe_powerbutton.c:	set_bit(EV_KEY, dev->evbit);
./arch/blackfin/mach-bf548/boards/ezkit.c:	.ev_code_tap = {BTN_TOUCH, BTN_TOUCH, BTN_TOUCH}, /* EV_KEY x,y,z */
./arch/blackfin/mach-bf548/boards/ezkit.c:/*	.ev_code_ff = KEY_F,*/		/* EV_KEY */
./arch/blackfin/mach-bf548/boards/ezkit.c:/*	.ev_code_act_inactivity = KEY_A,*/	/* EV_KEY */
./arch/blackfin/mach-bf548/boards/ezkit.c:	/* EV_KEY {+Z, +Y, +X, -X, -Y, -Z} */
./arch/blackfin/mach-bf537/boards/stamp.c:	.ev_code_tap = {BTN_TOUCH, BTN_TOUCH, BTN_TOUCH}, /* EV_KEY x,y,z */
./arch/blackfin/mach-bf537/boards/stamp.c:/*	.ev_code_ff = KEY_F,*/		/* EV_KEY */
./arch/blackfin/mach-bf537/boards/stamp.c:/*	.ev_code_act_inactivity = KEY_A,*/	/* EV_KEY */
./arch/blackfin/mach-bf537/boards/stamp.c:	/* EV_KEY {+Z, +Y, +X, -X, -Y, -Z} */
./arch/blackfin/mach-bf527/boards/tll6527m.c:	.ev_code_tap = {BTN_TOUCH, BTN_TOUCH, BTN_TOUCH}, /* EV_KEY x,y,z */
./arch/blackfin/mach-bf527/boards/tll6527m.c:/*	.ev_code_ff = KEY_F,*/		/* EV_KEY */
./arch/blackfin/mach-bf527/boards/tll6527m.c:	.ev_code_act_inactivity = KEY_A,	/* EV_KEY */
./arch/x86/platform/mrst/mrst.c:	{KEY_POWER,		-1, 1, "power_btn",	EV_KEY, 0, 3000},
./arch/x86/platform/mrst/mrst.c:	{KEY_PROG1,		-1, 1, "prog_btn1",	EV_KEY, 0, 20},
./arch/x86/platform/mrst/mrst.c:	{KEY_PROG2,		-1, 1, "prog_btn2",	EV_KEY, 0, 20},
./arch/x86/platform/mrst/mrst.c:	{KEY_VOLUMEUP,		-1, 1, "vol_up",	EV_KEY, 0, 20},
./arch/x86/platform/mrst/mrst.c:	{KEY_VOLUMEDOWN,	-1, 1, "vol_down",	EV_KEY, 0, 20},
./arch/x86/platform/mrst/mrst.c:	{KEY_CAMERA,		-1, 1, "camera_full",	EV_KEY, 0, 20},
./arch/x86/platform/mrst/mrst.c:	{KEY_CAMERA_FOCUS,	-1, 1, "camera_half",	EV_KEY, 0, 20},
./arch/mips/ath79/mach-ap81.c:		.type		= EV_KEY,
./arch/mips/ath79/mach-ap81.c:		.type		= EV_KEY,
./arch/mips/ath79/mach-pb44.c:		.type		= EV_KEY,
./arch/mips/ath79/mach-pb44.c:		.type		= EV_KEY,
./arch/unicore32/kernel/puv3-nb0916.c:		.type	= EV_KEY,
./arch/unicore32/kernel/puv3-nb0916.c:		.type	= EV_KEY,
./arch/arm/mach-exynos4/mach-universal_c210.c:		.type			= EV_KEY,
./arch/arm/mach-exynos4/mach-universal_c210.c:		.type			= EV_KEY,
./arch/arm/mach-exynos4/mach-universal_c210.c:		.type			= EV_KEY,
./arch/arm/mach-exynos4/mach-universal_c210.c:		.type			= EV_KEY,
./arch/arm/mach-exynos4/mach-universal_c210.c:		.type			= EV_KEY,
./arch/arm/mach-exynos4/mach-nuri.c:		.type			= EV_KEY,
./arch/arm/mach-exynos4/mach-nuri.c:		.type			= EV_KEY,
./arch/arm/mach-exynos4/mach-nuri.c:		.type			= EV_KEY,
./arch/arm/mach-davinci/board-da850-evm.c:		.type			= EV_KEY,
./arch/arm/mach-davinci/board-da850-evm.c:		.type			= EV_KEY,
./arch/arm/mach-mx6/board-mx6q_hdmidongle.c:	.type		= EV_KEY,				\
./arch/arm/mach-mx6/board-mx6q_sabresd.c:	.type		= EV_KEY,				\
./arch/arm/mach-mx6/board-mx6q_sabreauto.c:	.type		= EV_KEY,				\
./arch/arm/mach-mx6/board-mx6q_sabrelite.c:	.type		= EV_KEY,				\
./arch/arm/mach-omap1/board-htcherald.c:	{BTN_0,  HTCHERALD_GPIO_POWER, 1, "POWER", EV_KEY, 1, 20},
./arch/arm/mach-omap1/board-htcherald.c:	{KEY_LEFT,  HTCPLD_GPIO_LEFT_KBD,  1, "LEFT",  EV_KEY, 1, 20},
./arch/arm/mach-omap1/board-htcherald.c:	{KEY_RIGHT, HTCPLD_GPIO_RIGHT_KBD, 1, "RIGHT", EV_KEY, 1, 20},
./arch/arm/mach-omap1/board-htcherald.c:	{KEY_UP,    HTCPLD_GPIO_UP_KBD,    1, "UP",    EV_KEY, 1, 20},
./arch/arm/mach-omap1/board-htcherald.c:	{KEY_DOWN,  HTCPLD_GPIO_DOWN_KBD,  1, "DOWN",  EV_KEY, 1, 20},
./arch/arm/mach-omap1/board-htcherald.c:	{KEY_LEFT,  HTCPLD_GPIO_LEFT_DPAD,   1, "DLEFT",  EV_KEY, 1, 20},
./arch/arm/mach-omap1/board-htcherald.c:	{KEY_RIGHT, HTCPLD_GPIO_RIGHT_DPAD,  1, "DRIGHT", EV_KEY, 1, 20},
./arch/arm/mach-omap1/board-htcherald.c:	{KEY_UP,    HTCPLD_GPIO_UP_DPAD,     1, "DUP",    EV_KEY, 1, 20},
./arch/arm/mach-omap1/board-htcherald.c:	{KEY_DOWN,  HTCPLD_GPIO_DOWN_DPAD,   1, "DDOWN",  EV_KEY, 1, 20},
./arch/arm/mach-omap1/board-htcherald.c:	{KEY_ENTER, HTCPLD_GPIO_ENTER_DPAD,  1, "DENTER", EV_KEY, 1, 20},
./arch/arm/mach-s5pv210/mach-goni.c:		.type		= EV_KEY,
./arch/arm/mach-s5pv210/mach-aquila.c:		.type		= EV_KEY,
./arch/arm/mach-imx/mach-imx27_visstrim_m10.c:		.type	= EV_KEY,
./arch/arm/mach-imx/mach-imx27_visstrim_m10.c:		.type	= EV_KEY,
./arch/arm/mach-imx/mach-imx27_visstrim_m10.c:		.type   = EV_KEY,
./arch/arm/mach-imx/mach-pcm037_eet.c:		.type	= EV_KEY,
./arch/arm/mach-imx/mach-pcm037_eet.c:		.type	= EV_KEY,
./arch/arm/mach-imx/mach-pcm037_eet.c:		.type	= EV_KEY,
./arch/arm/mach-imx/mach-pcm037_eet.c:		.type	= EV_KEY,
./arch/arm/mach-imx/mach-pcm037_eet.c:		.type	= EV_KEY,
./arch/arm/mach-imx/mach-pcm037_eet.c:		.type	= EV_KEY,
./arch/arm/mach-imx/mach-pcm037_eet.c:		.type	= EV_KEY,
./arch/arm/mach-imx/mach-pcm037_eet.c:		.type	= EV_KEY,
./arch/arm/mach-imx/mach-pcm037_eet.c:		.type	= EV_KEY,
./arch/arm/mach-imx/mach-pcm037_eet.c:		.type	= EV_KEY,
./arch/arm/mach-imx/mach-pcm037_eet.c:		.type	= EV_KEY,
./arch/arm/mach-imx/mach-pcm037_eet.c:		.type	= EV_KEY,
./arch/arm/mach-imx/mach-pcm037_eet.c:		.type	= EV_KEY,
./arch/arm/mach-imx/mach-pcm037_eet.c:		.type	= EV_KEY,
./arch/arm/mach-gemini/board-wbd111.c:		.type		= EV_KEY,
./arch/arm/mach-gemini/board-wbd222.c:		.type		= EV_KEY,
./arch/arm/mach-gemini/board-rut1xx.c:		.type		= EV_KEY,
./arch/arm/mach-gemini/board-nas4220b.c:		.type		= EV_KEY,
./arch/arm/mach-gemini/board-nas4220b.c:		.type		= EV_KEY,
./arch/arm/mach-pxa/ezx.c:		.type       = EV_KEY,
./arch/arm/mach-pxa/ezx.c:		.type       = EV_KEY,
./arch/arm/mach-pxa/raumfeld.c:		.type			= EV_KEY,
./arch/arm/mach-pxa/raumfeld.c:		.type			= EV_KEY,
./arch/arm/mach-pxa/raumfeld.c:		.type			= EV_KEY,
./arch/arm/mach-pxa/raumfeld.c:		.type			= EV_KEY,
./arch/arm/mach-pxa/raumfeld.c:		.type			= EV_KEY,
./arch/arm/mach-pxa/raumfeld.c:		.type			= EV_KEY,
./arch/arm/mach-pxa/magician.c:		.type   = EV_KEY,	\
./arch/arm/mach-pxa/hx4700.c:		.type       = EV_KEY,			\
./arch/arm/mach-pxa/palmtc.c:	{KEY_F8, GPIO_NR_PALMTC_HOTSYNC_BUTTON, 1, "HotSync Button", EV_KEY, 1},
./arch/arm/mach-pxa/z2.c:		.type		= EV_KEY,
./arch/arm/mach-pxa/mainstone.c:		.type	= EV_KEY,
./arch/arm/mach-pxa/tosa.c:		.type	= EV_KEY,
./arch/arm/mach-pxa/tosa.c:		.type	= EV_KEY,
./arch/arm/mach-pxa/mioa701.c:	.desc = (_desc), .type = EV_KEY, .wakeup = (_wakeup) }
./arch/arm/mach-orion5x/net2big-setup.c:		.type		= EV_KEY,
./arch/arm/mach-orion5x/d2net-setup.c:		.type		= EV_KEY,
./arch/arm/mach-tegra/board-seaboard.c:		.type		= EV_KEY,
./arch/arm/mach-s3c64xx/mach-smartq5.c:		.type                   = EV_KEY,
./arch/arm/mach-s3c64xx/mach-smartq5.c:		.type                   = EV_KEY,
./arch/arm/mach-s3c64xx/mach-smartq5.c:		.type                   = EV_KEY,
./arch/arm/mach-s3c64xx/mach-smartq5.c:		.type                   = EV_KEY,
./arch/arm/mach-s3c64xx/mach-smartq7.c:		.type                   = EV_KEY,
./arch/arm/mach-s3c64xx/mach-smartq7.c:		.type                   = EV_KEY,
./arch/arm/mach-s3c64xx/mach-smartq7.c:		.type                   = EV_KEY,
./arch/arm/mach-s3c64xx/mach-smartq7.c:		.type                   = EV_KEY,
./arch/arm/mach-s3c64xx/mach-smartq7.c:		.type                   = EV_KEY,
./arch/arm/mach-s3c64xx/mach-smartq7.c:		.type                   = EV_KEY,
./arch/arm/mach-s3c2440/mach-gta02.c:		.type = EV_KEY,
./arch/arm/mach-s3c2440/mach-gta02.c:		.type = EV_KEY,
./arch/arm/mach-shmobile/board-mackerel.c:#define KEYPAD_BUTTON_LOW(event_code) KEYPAD_BUTTON(EV_KEY, event_code, 1)
./arch/arm/mach-omap2/board-omap3pandora.c:	GPIO_BUTTON(gpio_num, EV_KEY, event_code, 1, description)
./arch/arm/mach-omap2/board-omap3pandora.c:	GPIO_BUTTON(100, EV_KEY, KEY_LEFTALT, 0, "alt"),
./arch/arm/mach-omap2/board-rx51-peripherals.c:		.type			= EV_KEY,
./arch/arm/mach-omap2/board-rx51-peripherals.c:		.type			= EV_KEY,
./arch/arm/mach-omap2/board-rx51-peripherals.c:		.type			= EV_KEY,
./arch/arm/mach-at91/board-gsia18s.c:		.type		= EV_KEY,
./arch/arm/mach-at91/board-gsia18s.c:		.type		= EV_KEY,
./arch/arm/mach-at91/board-gsia18s.c:		.type		= EV_KEY,
./arch/arm/mach-at91/board-gsia18s.c:		.type		= EV_KEY,
./arch/arm/mach-mx5/board-mx53_loco.c:	.type		= EV_KEY,				\
./arch/arm/mach-mx5/board-mx53_smd.c:	.type           = EV_KEY,                               \
./arch/arm/mach-sa1100/h3xxx.c:		.type		= EV_KEY,
./arch/arm/mach-sa1100/h3xxx.c:		.type		= EV_KEY,
./arch/avr32/boards/atngw100/mrmt.c:		.type		= EV_KEY,
./arch/avr32/boards/atngw100/mrmt.c:		.type		= EV_KEY,
./arch/avr32/boards/atngw100/mrmt.c:		.type		= EV_KEY,
./sound/core/jack.c:		input_set_capability(jack->input_dev, EV_KEY, jack->key[i]);
./sound/usb/caiaq/input.c:		input->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_ABS);
./sound/usb/caiaq/input.c:		input->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_ABS);
./sound/usb/caiaq/input.c:		input->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_ABS);
./sound/usb/caiaq/input.c:		input->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_ABS);
./sound/usb/caiaq/input.c:		input->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_ABS);
./sound/usb/caiaq/input.c:		input->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_ABS);
./sound/pci/maestro3.c:	__set_bit(EV_KEY, input_dev->evbit);
./sound/pci/es1968.c:	__set_bit(EV_KEY, input_dev->evbit);
./net/bluetooth/hidp/core.c:		set_bit(EV_KEY, input->evbit);
./net/bluetooth/hidp/core.c:		input->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_REL);
./net/rfkill/input.c:	if (type == EV_KEY && data == 1) {
./net/rfkill/input.c:		.evbit = { BIT_MASK(EV_KEY) },
./net/rfkill/input.c:		.evbit = { BIT_MASK(EV_KEY) },
./net/rfkill/input.c:		.evbit = { BIT_MASK(EV_KEY) },
./net/rfkill/input.c:		.evbit = { BIT_MASK(EV_KEY) },
./net/rfkill/input.c:		.evbit = { BIT_MASK(EV_KEY) },
