#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
 .name = KBUILD_MODNAME,
 .init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
 .exit = cleanup_module,
#endif
 .arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0xc77a97a, "prp_enc_deselect" },
	{ 0x631cb49f, "csi_enc_deselect" },
	{ 0x761b20eb, "prp_still_deselect" },
	{ 0xca55481b, "prp_still_select" },
	{ 0xafd9dc5e, "prp_enc_select" },
	{ 0x7e14dc74, "csi_enc_select" },
	{ 0xf0aa37df, "bg_overlay_sdc_deselect" },
	{ 0xb9729003, "foreground_sdc_deselect" },
	{ 0xd2a84747, "bg_overlay_sdc_select" },
	{ 0x39d0857c, "foreground_sdc_select" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=ipu_prp_enc,ipu_csi_enc,ipu_still,ipu_bg_overlay_sdc,ipu_fg_overlay_sdc";

