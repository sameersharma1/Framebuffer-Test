#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <errno.h>

struct fb_var_screeninfo vinfo;
struct fb_fix_screeninfo finfo;
char *frameBuffer = 0;

void drawrect (int x0, int y0, int width, int height, int color)
{
	const int bytesPerPixel = 4;
	const int stride = finfo.line_length / bytesPerPixel;

	int *dest = (int *) (frameBuffer) + (y0 + vinfo.yoffset) * stride + (x0 + vinfo.xoffset);

	int x, y;
	for (y = 0; y <height; ++y)
	{
		for (x = 0; x <width; ++x)
		{
			dest[x] = color;
		}
		dest += stride;
	}
}


int
main (int argc, char **argv)
{
	const char *devfile = "/dev/fb0";
	long int screensize = 0;
	int fbFd = 0;

	if((fbFd = open (devfile, O_RDWR))<0)
	{
		perror ("Error: cannot open framebuffer device");
		exit (1);
	}

	if (ioctl (fbFd, FBIOGET_FSCREENINFO, &finfo) < 0)
	{
		perror ("Error while reading fixed information");
		exit (2);
	}
	if (ioctl (fbFd, FBIOGET_VSCREENINFO, &vinfo) == -1)
	{
		perror ("Error while reading variable information");
		exit (3);
	}
	
	screensize = finfo.smem_len;

	frameBuffer = (char *) mmap (0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED,fbFd, 0);
	if(frameBuffer == MAP_FAILED)
	{
		perror ("Error: Failed to map framebuffer device to memory");
		exit (4);
	}

	drawrect (vinfo.xres     / 8, vinfo.yres     / 8, vinfo.xres / 8, vinfo.yres / 8, 0xffff0000);
	drawrect (vinfo.xres * 2 / 8, vinfo.yres     / 8, vinfo.xres / 8, vinfo.yres / 8, 0xff00ff00);
	drawrect (vinfo.xres * 3 / 8, vinfo.yres     / 8, vinfo.xres / 8, vinfo.yres / 8, 0xff0000ff);


	drawrect (vinfo.xres     / 8, vinfo.yres * 2 / 8, vinfo.xres / 8, vinfo.yres / 8, 0xff00ff00);
	drawrect (vinfo.xres * 2 / 8, vinfo.yres * 2 / 8, vinfo.xres / 8, vinfo.yres / 8, 0xff0000ff);
	drawrect (vinfo.xres * 3 / 8, vinfo.yres * 2 / 8, vinfo.xres / 8, vinfo.yres / 8, 0xffff0000);

	drawrect (vinfo.xres     / 8, vinfo.yres * 3 / 8, vinfo.xres / 8, vinfo.yres / 8, 0xff0000ff);
	drawrect (vinfo.xres * 2 / 8, vinfo.yres * 3 / 8, vinfo.xres / 8, vinfo.yres / 8, 0xffff0000);
	drawrect (vinfo.xres * 3 / 8, vinfo.yres * 3 / 8, vinfo.xres / 8, vinfo.yres / 8, 0xff00ff00);

	munmap (frameBuffer, screensize);

	close (fbFd);
	return 0;
}
