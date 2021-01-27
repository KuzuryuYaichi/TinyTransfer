/*
 * This file is part of the Xilinx DMA IP Core driver tools for Linux
 *
 * Copyright (c) 2016-present,  Xilinx, Inc.
 * All rights reserved.
 *
 * This source code is licensed under BSD-style license (found in the
 * LICENSE file in the root directory of this source tree)
 */
#ifndef _DMA_UTILS_H
#define _DMA_UTILS_H

#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <sys/types.h>

/*
 * man 2 write:
 * On Linux, write() (and similar system calls) will transfer at most
 * 	0x7ffff000 (2,147,479,552) bytes, returning the number of bytes
 *	actually transferred.  (This is true on both 32-bit and 64-bit
 *	systems.)
 */

#define RW_MAX_SIZE	0x7ffff000

#define DEVICE_NAME_H2C "/dev/xdma0_h2c_0"
#define DEVICE_NAME_C2H "/dev/xdma0_c2h_0"
#define SIZE_DEFAULT (32)
#define COUNT_DEFAULT (1)

int dma_from_device(const char *devname, uint64_t addr, uint64_t aperture, uint64_t size, uint64_t offset, uint64_t count);

int dma_to_device(const char *devname, uint64_t addr, uint64_t aperture, uint64_t size, uint64_t offset, uint64_t count);

ssize_t read_to_buffer(int fd, char *buffer, uint64_t size, uint64_t base);

ssize_t write_from_buffer(int fd, char *buffer, uint64_t size, uint64_t base);

#endif
