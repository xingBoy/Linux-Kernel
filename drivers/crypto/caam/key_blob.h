
/*
 * Copyright (C) 2012-2015 Freescale Semiconductor, Inc., All Rights Reserved
 */

#ifndef KEY_BLOB_H
#define KEY_BLOB_H

#include <linux/ioctl.h>
#include <linux/types.h>

typedef struct {
	uint32_t key_len;
	uint32_t blob_len;
}kb_parameter_t;

typedef struct {
	uint8_t *key_addr;
	uint8_t *blob_addr;
	dma_addr_t key_phy_addr;
	dma_addr_t blob_phy_addr;
}kb_addr_t;

#define KB_IOCTL_ENCAP			_IOWR('K', 0, kb_parameter_t)
#define KB_IOCTL_DECAP			_IOWR('K', 1, kb_parameter_t)
#define KB_IOCTL_GET_PHY_ADDR	_IOR('K', 2, kb_addr_t)

#endif /* KEY_BLOB_H */

