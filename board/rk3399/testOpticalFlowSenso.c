#include <stdio.h>
#include <pthread.h>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>

static pthread_t thr0;

static unsigned char buf[512];
static const unsigned int bufSize=512;

static void *thread0(void *arg)
{
	int uartHandle;
	int ret,i;

	//uartHandle = uartInit("/dev/ttyUSB0", 1000000, 'N', 8, 1);
	uartHandle = uartInit("/dev/ttyS3", 1000000, 'N', 8, 1);
	printf("uartHandle: %d\n", uartHandle);
	if(uartHandle<=0) {
		printf("uart init fail.\n");
		while(1) sleep(1);
	}

//	memset(buf, 0x0, bufSize);
//	ret = ioctl(uartHandle, FIONREAD, buf);
//	printf("ret: %d\n", ret);
//	for(i=0;i<4;i++)
//		printf("bytes:%d\n", buf[i]);
//
//	while(1) sleep(100);

	while(1) {
		ret = uartRecv(uartHandle, buf, bufSize, 1000);
		if(ret>0){
			printf("recv: %d\n", ret);
			for(i=0; i<ret; i++){
				printf("0x%2x ", buf[i]);
				if((i+1)%8 == 0) printf("  ");
				if((i+1)%16 == 0) printf("\n");
			}
			printf("\n\n ********************* \n\n");
		}
//		else{
//			printf("timeout.");
//		}
	}
}



int testOptiocalFlowSenso(void)
{
	int ret;

	ret = pthread_create(&thr0, NULL, thread0, NULL);
	if(ret!=0) printf("## wk1 ## pthread create fail\n");

	getchar();
	getchar();
	getchar();
	printf("exit.\n");

	return 0;
}

