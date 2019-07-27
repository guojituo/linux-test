#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

#define I2C_ADDRESS (0x18)
//#define I2C_SLAVE		0x0703

/**\name    Accel Chip Id register */
#define BMI08X_ACCEL_CHIP_ID_REG                    (0x00)

int main(void)
{
	int fd, ret;
	int i;
	unsigned char buf[64];

	fd = open("/dev/i2c-4", O_RDWR);
	if(fd<0) {
		printf("i2c-4 open fail. #%d\n", errno);
		return -1;
	}

	ret = ioctl(fd, I2C_SLAVE, I2C_ADDRESS);
	if(ret<0){
		printf("i2c-4 set address fail.\n");
		goto exit;
	}

	ret = ioctl(fd, I2C_TENBIT, 0);
	if(ret<0){
		printf("i2c-4 set fail.\n");
		goto exit;
	}
//	ret = ioctl(fd, I2C_BUS_MODE, 1);
//	if(ret<0){
//		printf("i2c-4 set bus mode fail.\n");
//		return -1;
//	}

	// read
	buf[0] = BMI08X_ACCEL_CHIP_ID_REG;
	ret = write(fd, &buf[0], 1);
	if(ret!=1){
		printf("write fail.\n");
		goto exit;
	}

	usleep(100);

	memset(buf, 0x0, 64);
	ret = read(fd, buf, 1);
	printf("ret=%d \n", ret);
	if(ret>0){
		for(i=0;i<ret;i++) {
			printf("0x%x ", buf[i]);
		}
		printf("\n");
	}
exit:
	close(fd);

	printf("exit.\n");

	return 0;
}
