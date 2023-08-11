#include "lcm.h"
#include <linux/fb.h>
#include <byteswap.h>

CLCM* lcm;

fb_fix_screeninfo fbFix;
fb_var_screeninfo fbVar;
char* fbMem;
int fbDevice;

CLCM lcm7735
{
    .name = "ST7735",
    .width = 128,
    .height = 160,
    .init = &st7735Init,
    .presentBuffer = &st7735Bitmap
};

CLCM* lcmList[] = {
  &lcm7735  
};

void gpHelperSetState(int gpFd, int state)
{
    char chr;

    if(state)
        chr = '1';
    else
        chr = '0';

    write(gpFd, &chr, sizeof(chr));
}

unsigned short* buf;

bool setupFrameBuffer()
{
    LOG("Open framebuffer device");
    fbDevice = open("/dev/fb0", O_RDWR);
    
    if(!fbDevice)
    {
        LOG("Failed to open primary framebuffer");
        return false;
    }
    ioctl(fbDevice, FBIOGET_VSCREENINFO, &fbVar);

    fbVar.xres = lcm->width;
    fbVar.yres = lcm->height;

    if(ioctl(fbDevice, FBIOPUT_VSCREENINFO, &fbVar) < 0)
    {
        LOG("Unable to set framebuffer size :c");
        return false;
    }

    ioctl(fbDevice, FBIOGET_VSCREENINFO, &fbVar); // Get yet another time for test
    LOGF("Parent FB: %ix%i %i-bits", fbVar.xres, fbVar.yres, fbVar.bits_per_pixel);
    ioctl(fbDevice, FBIOGET_FSCREENINFO, &fbFix);

    fbMem = (char*)mmap(0, fbFix.smem_len, PROT_READ | PROT_WRITE, MAP_SHARED, fbDevice, 0);
    buf = (unsigned short*)malloc(lcm->width * lcm->height * 2);
    //fbMem = malloc(fbFix.smem_len);
    if(!fbMem)
    {
        LOG("mmap failed");
        
        return false;
    }

    return true;
}

__inline unsigned short lcmTo565(unsigned int r, unsigned int g, unsigned int b)
{
    short ret = ((r & 0b11111000) << 8) | ((g & 0b11111100) << 3) | (b >> 3);
    return bswap_16(ret);
}

void lcmCopyFramebuffer()
{
    int bpp = fbVar.bits_per_pixel / 8;

    for(int i = 0; i < lcm->width; i++)
    {
        for(int j = 0; j < lcm->height; j++)
        {
            unsigned char* rgbData = (unsigned char*)&fbMem[(j * fbFix.line_length) + (i * bpp)];

            buf[j * lcm->width + i] = lcmTo565(rgbData[0], rgbData[1], rgbData[2]);
        }
    }

    lcm->presentBuffer(buf);
}

int main(int argc, char** argv)
{
    LOG("Starting lcmsvc");

    lcm = &lcm7735;

    LOGF("Driver: %s", lcm7735.name);
    LOGF("Screen size: %ix%i", lcm->width, lcm->height);

    if(!lcm->init || !lcm->presentBuffer)
    {
        LOG("LCM driver is incomplete or broken");
        return -1;
    }

    lcm->init();
    if(!setupFrameBuffer())
        return -1;

    LOG("LCM loop started");

    while(1)
        lcmCopyFramebuffer();
}