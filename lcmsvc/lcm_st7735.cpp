#include "lcm.h"


int fd, dcFd, resetFd;

void st7735Command(unsigned char cmd)
{
    spi_ioc_transfer tf;
    memset(&tf, 0, sizeof(tf));
    tf.bits_per_word = 8;
    tf.len = 1;
    tf.speed_hz = 64000000;
    tf.tx_buf = (unsigned long)&cmd;

    gpHelperSetState(dcFd, 0);

    if(ioctl(fd, SPI_IOC_MESSAGE(1), &tf) < 0)
        LOG("SPI transfer failed\n");
}

void st7735Data(unsigned char data)
{
    spi_ioc_transfer tf;
    memset(&tf, 0, sizeof(tf));
    tf.bits_per_word = 8;
    tf.len = 1;
    tf.speed_hz = 64000000;
    tf.tx_buf = (unsigned long)&data;

    gpHelperSetState(dcFd, 1);

    if(ioctl(fd, SPI_IOC_MESSAGE(1), &tf) < 0)
        LOG("SPI transfer failed\n");
}

void st7735Reset()
{
	st7735Command(0x11);//Sleep out
	usleep(120000);
	//ST7735R Frame Rate
	st7735Command(0xB1);
	st7735Data(0x01);
	st7735Data(0x2C);
	st7735Data(0x2D);
	st7735Command(0xB2);
	st7735Data(0x01);
	st7735Data(0x2C);
	st7735Data(0x2D);
	st7735Command(0xB3);
	st7735Data(0x01);
	st7735Data(0x2C);
	st7735Data(0x2D);
	st7735Data(0x01);
	st7735Data(0x2C);
	st7735Data(0x2D);
	//------------------------------------End ST7735R Frame Rate-----------------------------------------//
	st7735Command(0xB4);//Column inversion
	st7735Data(0x07);
	//------------------------------------ST7735R Power Sequence-----------------------------------------//
	st7735Command(0xC0);
	st7735Data(0xA2);
	st7735Data(0x02);
	st7735Data(0x84);
	st7735Command(0xC1);
	st7735Data(0xC5);
	st7735Command(0xC2);
	st7735Data(0x0A);
	st7735Data(0x00);
	st7735Command(0xC3);
	st7735Data(0x8A);
	st7735Data(0x2A);
	st7735Command(0xC4);
	st7735Data(0x8A);
	st7735Data(0xEE);
	//---------------------------------End ST7735R Power Sequence-------------------------------------//
	st7735Command(0xC5);//VCOM
	st7735Data(0x0E);
	st7735Command(0x36);//MX, MY, RGB mode
	st7735Data(0xC8);
	//------------------------------------ST7735R Gamma Sequence-----------------------------------------//
	st7735Command(0xe0);
	st7735Data(0x02);
	st7735Data(0x1c);
	st7735Data(0x07);
	st7735Data(0x12);
	st7735Data(0x37);
	st7735Data(0x32);
	st7735Data(0x29);
	st7735Data(0x2d);
	st7735Data(0x29);
	st7735Data(0x25);
	st7735Data(0x2b);
	st7735Data(0x39);
	st7735Data(0x00);
	st7735Data(0x01);
	st7735Data(0x03);
	st7735Data(0x10);
	st7735Command(0xe1);
	st7735Data(0x03);
	st7735Data(0x1d);
	st7735Data(0x07);
	st7735Data(0x06);
	st7735Data(0x2e);
	st7735Data(0x2c);
	st7735Data(0x29);
	st7735Data(0x2d);
	st7735Data(0x2e);
	st7735Data(0x2e);
	st7735Data(0x37);
	st7735Data(0x3f);
	st7735Data(0x00);
	st7735Data(0x00);
	st7735Data(0x02);
	st7735Data(0x10);
		st7735Command(0x2A);
	st7735Data(0x00);
	st7735Data(0x02);
	st7735Data(0x00);
	st7735Data(0x81);

	st7735Command(0x2B);
	st7735Data(0x00);
	st7735Data(0x01);
	st7735Data(0x00);
	st7735Data(0xA0);
	//------------------------------------End ST7735R Gamma Sequence-----------------------------------------//

	//st7735Command(0x3A);
	//st7735Data(0x05);
	st7735Command(0x3A);//65k mode
	st7735Data(0x05);
	st7735Command(0x2C);//Display on

	st7735Command(0x29);//Display on

	// Set viewport
	int x1 = 0;
	int x2 = 128;
	int y1 = 0;
	int y2 = 160;

	st7735Command(0x2A);
	st7735Data(x1>>8);
	st7735Data(x1);
	st7735Data(x2>>8);
	st7735Data(x2);

	st7735Command(0x2B);
	st7735Data(y1>>8);
	st7735Data(y1);
	st7735Data(y2);
	st7735Data(y2);

	st7735Command(0x2C);
}

void st7735Bitmap(void* data)
{
    int numTrans = (128 * 160 * 2) / 64;

    gpHelperSetState(dcFd, 1);

    for(int i = 0; i < 640; i++)
    {
        spi_ioc_transfer tf;
        memset(&tf, 0, sizeof(tf));
        tf.bits_per_word = 8;
        tf.len = 64;
        tf.speed_hz = 32000000;
        tf.tx_buf = (unsigned long)data;

        data += 64;

        if(ioctl(fd, SPI_IOC_MESSAGE(1), &tf) < 0)
            LOG("SPI transfer failed\n");
    }
}

void st7735Init()
{
    fd = open("/dev/spidev0.0", O_RDWR | O_NONBLOCK);
    dcFd = open("/sys/class/gpio/gpio10/value", O_RDWR);
    resetFd = open("/sys/class/gpio/gpio20/value", O_RDWR);

    if(!fd)
    {
        LOG("Failed to open spi device");
        return;
    }

    if(!dcFd)
    {
        LOG("Failed to open DC GPIO");
        return;
    }

    int mode = SPI_NO_CS;

    gpHelperSetState(resetFd, 0);
    sleep(1);
    gpHelperSetState(resetFd, 1);

    st7735Reset();
    LOG("Successfuly intiailized");
}