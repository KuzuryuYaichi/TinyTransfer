/*
 * This file is part of the Xilinx DMA IP Core driver tools for Linux
 *
 * Copyright (c) 2016-present,  Xilinx, Inc.
 * All rights reserved.
 *
 * This source code is licensed under BSD-style license (found in the
 * LICENSE file in the root directory of this source tree)
 */

#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <sys/types.h>

#include "dma_utils.h"
/*
 * man 2 write:
 * On Linux, write() (and similar system calls) will transfer at most
 * 	0x7ffff000 (2,147,479,552) bytes, returning the number of bytes
 *	actually transferred.  (This is true on both 32-bit and 64-bit
 *	systems.)
 */

ssize_t read_to_buffer(int fd, char *buffer, uint64_t size, uint64_t base)
{
    const char *fname = DEVICE_NAME_C2H;
    ssize_t rc;
    uint64_t count = 0;
    char *buf = buffer;
    off_t offset = base;

    uint64_t bytes = size - count;

    if (bytes > RW_MAX_SIZE)
        bytes = RW_MAX_SIZE;

    /* read data from file into memory buffer */
    rc = read(fd, buf, bytes);
    if (rc < 0) {
        fprintf(stderr, "%s, read 0x%lx @ 0x%lx failed %ld.\n", fname, bytes, offset, rc);
        perror("read file");
        return -EIO;
    }

    buf += bytes;
    offset += bytes;
    return count;
}

ssize_t write_from_buffer(int fd, char *buffer, uint64_t size, uint64_t base)
{
    const char *fname = DEVICE_NAME_H2C;
    ssize_t rc;
    uint64_t count = 0;
    char *buf = buffer;
    off_t offset = base;
    int loop = 0;

    while (count < size) {
        uint64_t bytes = size - count;

        if (bytes > RW_MAX_SIZE)
            bytes = RW_MAX_SIZE;

        if (offset) {
            rc = lseek(fd, offset, SEEK_SET);
            if (rc != offset) {
                fprintf(stderr, "%s, seek off 0x%lx != 0x%lx.\n", fname, rc, offset);
                perror("seek file");
                return -EIO;
            }
        }

        /* write data to file from memory buffer */
        rc = write(fd, buf, bytes);
        if (rc < 0) {
            fprintf(stderr, "%s, write 0x%lx @ 0x%lx failed %ld.\n", fname, bytes, offset, rc);
            perror("write file");
            return -EIO;
        }

        count += rc;
        if (rc != bytes) {
            fprintf(stderr, "%s, write underflow 0x%lx/0x%lx @ 0x%lx.\n", fname, rc, bytes, offset);
            break;
        }
        buf += bytes;
        offset += bytes;

        loop++;
    }

    if (count != size && loop)
        fprintf(stderr, "%s, write underflow 0x%lx/0x%lx.\n", fname, count, size);

    return count;
}
