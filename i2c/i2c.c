#include <stdio.h>
#include <fcntl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

int i2cInit(const char *devName, const unsigned char slaveAddress)
{
	int fd;

	if(!devName) return -1;
	fd = open(devName, O_RDWR);
	if(fd<0) return -1;

	return fd;
}

int i2cRead(const int fd)
{

	return 0;
}

int i2cWrite(const int fd)
{

	return 0;
}
