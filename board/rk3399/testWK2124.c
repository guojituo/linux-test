#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include "uart.h"

static pthread_t thr0, thr1, thr2, thr3;

static unsigned char buf[512];
static const unsigned int bufSize=512;

//LED Light
//baud: 1000000M
static void *thread_wk0(void *arg)
{
	while(1){

		sleep(1);
	}

	return 0;
}

//2.4G
//baud: 1000000M
static void *thread_wk1(void *arg)
{
	int uartHandle;
	int ret,i;
while(1) sleep(10);
	uartHandle = uartInit(1, 1000000, 'N', 8, 1);
	printf("uartHandle: %d\n", uartHandle);
	if(uartHandle<=0) {
		printf("uart init fail.\n");
		while(1) sleep(1);
	}

	while(1) {
		ret = uartRecv(uartHandle, buf, bufSize, 1000);
		if(ret>0){
			//printf("recv: %d\n", ret);
			for(i=0; i<ret; i++){
				printf("0x%2x ", buf[i]);
				if((i+1)%8 == 0) printf("  ");
				if((i+1)%16 == 0) printf("\n");
			}
			printf("\n");
		}
//		else{
//			printf("timeout.");
//		}
	}

	return 0;
}

//IMU Module MP-6050
//Baud: 1000000bps
static void *thread_wk2(void *arg)
{
	while(1){
		//printf("theadk_wk2\n");
		sleep(10);
	}

	return 0;
}

//reserved
static void *thread_wk3(void *arg)
{
	while(1){
		//printf("thread_wk3\n");
		sleep(10);
	}

	return 0;
}


int testWK2124(void)
{
	int ret;

	ret = pthread_create(&thr0, NULL, thread_wk0, NULL);
	if(ret!=0) printf("## wk0 ## pthread create fail\n");
	ret = pthread_create(&thr1, NULL, thread_wk1, NULL);
	if(ret!=0) printf("## wk1 ## pthread create fail\n");
	ret = pthread_create(&thr2, NULL, thread_wk2, NULL);
	if(ret!=0) printf("## wk2 ## pthread create fail.\n");
	ret = pthread_create(&thr3, NULL, thread_wk3, NULL);
	if(ret!=0) printf("## wk2 ## pthread create fail.\n");

	getchar();
	getchar();
	getchar();
	printf("exit.\n");

	return 0;
}
