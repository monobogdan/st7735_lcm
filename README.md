# What is it?
This is very simple library for working with various SPI LCM's, found on AliExpress. Right now, it supports only ST7735 - which is used in 1.8" 128x160 displays.
At this moment it doesn't implement any kind of drawing functions, excepting presenting ready 565 buffer on the screen, since this code was directly taken from my home-project and drawing-logics are split.

Also this repo contains lcmsvc - simple service, that prepares and copies primary framebuffer (/dev/fb0) to our LCM. It allows us to show X Window System, or, for exmaple, SDL viewport on our device without hacking Linux kernel.
This is tested on OrangePi One, but should work on any other single-boarded computer.

# How to use

Just call `Init`, and then send your ready bitmap using `Bitmap`. Particulary this example uses 20 and 30 GPIO on OrangePi One as DC and RESET pins, other wiring are default (3.3v to 3.3v supply on header, 5v to LCM backlight, CS jumper to ground, and MOSI/CLK to header).

# TODO

Personally, i don't like current method of managing GPIO's through sysfs. It should be replaced in future. Also, i will add support for other LCM's.
