/*
 * This file is part of the Xilinx DMA IP Core driver tool for Linux
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
#include <time.h>

#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include "dma_utils.h"

static int eop_flush = 0;

int dma_from_device(const char *devname, uint64_t addr, uint64_t aperture, uint64_t size, uint64_t offset, uint64_t count)
{
    ssize_t rc = 0;
	size_t bytes_done = 0;
	uint64_t i;
    uint64_t apt_loop = aperture ? (size + aperture - 1) / aperture : 0;
    int fpga_fd;

	/*
     * use O_TRUNC to indicate to the driver to flush the data up based on EOP (end-of-packet), streaming mode only
	 */
	if (eop_flush)
		fpga_fd = open(devname, O_RDWR | O_TRUNC);
	else
		fpga_fd = open(devname, O_RDWR);

	if (fpga_fd < 0) {
        fprintf(stderr, "unable to open device %s, %d.\n", devname, fpga_fd);
		perror("open device");
        return -EINVAL;
    }

    char buffer[1024];

    for (i = 0; i < count; i++) {
		if (apt_loop) {
			uint64_t j;
			uint64_t len = size;
			char *buf = buffer;

			bytes_done = 0;
			for (j = 0; j < apt_loop; j++, len -= aperture, buf += aperture) {
				uint64_t bytes = (len > aperture) ? aperture : len,
                rc = read_to_buffer(fpga_fd, buf, bytes, addr);
                if (rc < 0)
					goto out;
				bytes_done += rc;
			}
		} else {
            rc = read_to_buffer(fpga_fd, buffer, size, addr);
			if (rc < 0)
				goto out;
			bytes_done = rc;

        }
	}

    if (eop_flush) {
        rc = 0;
	} else 
		rc = -EIO;

out:
    close(fpga_fd);

	return rc;
}
