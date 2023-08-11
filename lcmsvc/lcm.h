#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <poll.h>
#include <linux/spi/spidev.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <sys/mman.h>

#define LOGF(fmt, ...) printf("[%s]: " fmt "\n", &__FUNCTION__, __VA_ARGS__)
#define LOG(fmt, ...) printf("[%s]: " fmt "\n", &__FUNCTION__)

struct CLCM
{
    char* name;
    int width, height;
    void(*init)();
    void(*presentBuffer)(void* buf);
};

void gpHelperSetState(int gpFd, int state);

void st7735Bitmap(void* data);
void st7735Init();