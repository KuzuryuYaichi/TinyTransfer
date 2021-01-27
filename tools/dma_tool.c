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

static int OpenDevice(const char *devname, int eop_flush)
{
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
    }else{
        return fpga_fd;
    }
}

static int CloseDevice(int fd)
{
    close(fd);
}

int read_from_device(int fpga_fd, uint64_t addr, uint64_t size)
{
    char buffer[1024];
    ssize_t rc;
    while(1)
    {
        rc = read_to_buffer(fpga_fd, buffer, size, addr);
        if (rc < 0) {
            fprintf(stderr, "%s, read failed %ld.\n", DEVICE_NAME_C2H, rc);
            perror("read file");
        }
    }
    close(fpga_fd);

    return rc;
}

int write_to_device(int fpga_fd, uint64_t addr, uint64_t size)
{
    char buffer[1024];
    ssize_t rc;
    while(1)
    {
        rc = write_from_buffer(fpga_fd, buffer, size, addr);
        if (rc < 0) {
            fprintf(stderr, "%s, write failed %ld.\n", DEVICE_NAME_H2C, rc);
            perror("write file");
        }
    }
    close(fpga_fd);

    return 0;
}

void DMA_Loop()
{
    int fd_c2h0 = OpenDevice(DEVICE_NAME_C2H, 1);
    int fd_h2c0 = OpenDevice(DEVICE_NAME_H2C, 0);
    read_from_device(fd_c2h0, 0, 1024);
//    CloseDevice(fd_c2h0);
    CloseDevice(fd_h2c0);
}
