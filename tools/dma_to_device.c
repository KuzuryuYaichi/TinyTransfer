/*
 * This file is part of the Xilinx DMA IP Core driver tools for Linux
 *
 * Copyright (c) 2016-present,  Xilinx, Inc.
 * All rights reserved.
 *
 * This source code is licensed under BSD-style license (found in the
 * LICENSE file in the root directory of this source tree)
 */

#define _DEFAULT_SOURCE
#include <assert.h>
#include <fcntl.h>
#include <getopt.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>

#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include "dma_utils.h"

int dma_to_device(const char *devname, uint64_t addr, uint64_t aperture, uint64_t size, uint64_t offset, uint64_t count)
{
	ssize_t rc;
    size_t bytes_done = 0;
    uint64_t apt_loop = aperture ? (size + aperture - 1) / aperture : 0;

    int fpga_fd = open(devname, O_RDWR);
	if (fpga_fd < 0) {
        fprintf(stderr, "unable to open device %s, %d.\n", devname, fpga_fd);
		perror("open device");
		return -EINVAL;
    }

    char buffer[1024];

    for (uint64_t i = 0; i < count; i++) {
        /* write buffer to AXI MM address using SGDMA */

		if (apt_loop) {
			uint64_t j;
			uint64_t len = size;
            char *buf = buffer;

			bytes_done = 0;
            for (j = 0; j < apt_loop; j++, len -= aperture, buf += aperture) {
				uint64_t bytes = (len > aperture) ? aperture : len,
                rc = write_from_buffer(fpga_fd, buf, bytes, addr);
				if (rc < 0)
					goto out;

                bytes_done += rc;
			}
		} else {
            rc = write_from_buffer(fpga_fd, buffer, size, addr);
			if (rc < 0)
				goto out;

            bytes_done = rc;
        }
    }

out:
    close(fpga_fd);

	if (rc < 0)
		return rc;
	/* treat underflow as error */
    return 0;
}
