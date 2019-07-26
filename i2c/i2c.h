#ifndef _I2C_H_
#define _I2C_H_

int i2cInit(const char *devName, unsigned char slaveAddress);
int i2cRead(const int fd);
int i2cWrite(const int fd);

#endif
