#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/videodev2.h>

#define CLEAR(x) memset(&(x), 0, sizeof(x))
#define FMT_NUM_PLANES 1

static int parseArg(int argc, char **argv)
{
	int i;

	printf("\n## argc: %d\n",argc);
	for(i=0;i<argc;i++){
		printf("argv[%d] # %s\n", i, argv[i]);
	}
	printf("\n");


	return 0;
}

static int xioctl(int fh, int request, void *arg)
{
	int ret;
	do {
		ret = ioctl(fh, request, arg);
	} while (-1 == ret && EINTR == errno);
	return ret;
}

struct buffer {
        void *start;
        size_t length;
};

int v4l2_capture(int argc, char *argv[])
{
	FILE *fp;
	int ret,i;
	int fd;
	int nBuffers;
	int size;
	void *p;
	char *devName;
	char *defName="/dev/video0";
	struct v4l2_capability cap;
	struct v4l2_fmtdesc fmtdesc;
	struct v4l2_format fmt;
	struct v4l2_requestbuffers req;
	struct v4l2_buffer buf;
	enum v4l2_buf_type type;
	struct v4l2_plane planes[FMT_NUM_PLANES];
	struct buffer *buffers;

	int width = 640;
	int height = 480;
	int format = V4L2_PIX_FMT_NV12;
	enum v4l2_buf_type buf_type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

	//parameter parse
	ret=parseArg(argc, argv);
	if(ret<0) return -1;

	if(argc>1) devName = argv[1];
	else devName = defName;
	printf("open device: %s\n", devName);

	fp=fopen("camera.jpg", "w");
	if(!fp) {
		printf("文件创建失败\n");
		return -1;
	}

	//打开显示设备,以阻塞方式打开
	fd = open(devName, O_RDWR, 0);
	if(fd<0){
		printf("camera open fail.\n");
		return 0;
	}

	//显示设备信息
	ret = xioctl(fd, VIDIOC_QUERYCAP, &cap);
	if(ret<0){
		printf("查询失败\n");
		goto errOut;
	}

//	if(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE){
//		printf("找到Camera Capture设备:%s\n", cap.driver);
//	}
//	else{
//		printf("没找到Camera Capture设备.\n");
//	}

    if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE) &&
    		!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE_MPLANE)) {
//        fprintf(stderr, "%s is not a video capture device, capabilities: %x\n", devName, cap.capabilities);
//        exit(EXIT_FAILURE);
    	printf("%s is not a video capture device, capabilities: %x\n", devName, cap.capabilities);
        goto errOut;
    }

    if (!(cap.capabilities & V4L2_CAP_STREAMING)) {
//		fprintf(stderr, "%s does not support streaming i/o\n", devName);
//		exit(EXIT_FAILURE);
    	printf("%s does not support streaming i/o\n", devName);
		goto errOut;
    }

	printf("\n");
	printf("********************************\n");
	printf("Camera Capabilities:");
	printf("\n");
	printf("driver: %s\n", cap.driver);		//驱动名字
	printf("card: %s\n", cap.card);			//设备名称
	printf("bus_info: %s\n", cap.bus_info);	//设备在系统中的位置
	printf("version: %u.%u.%u (0x%x)\n", (cap.version>>16)&0xff, (cap.version>>8)&0xff, cap.version&0xff, cap.version); //驱动版本号 x.x.x
	printf("capabilities: 0x%x\n", cap.capabilities);	//设备支持的操作
	printf("device_caps: 0x%x\n", cap.device_caps);		//
	printf("********************************\n");

    if (cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)
        buf_type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    else if (cap.capabilities & V4L2_CAP_VIDEO_CAPTURE_MPLANE)
        buf_type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;

    //枚举 >> 查询摄像头可捕捉的图片类型
	memset(&fmtdesc, 0x0, sizeof(fmtdesc));
	fmtdesc.index = 0;
	fmtdesc.type = buf_type;
	do{
		ret = xioctl(fd, VIDIOC_ENUM_FMT, &fmtdesc);
		if(ret<0) break;

		printf("\n");
		printf("********************************\n");
		printf("index: %d\n", fmtdesc.index+1);
		printf("type: 0x%x\n", fmtdesc.type);
		printf("flags: 0x%x\n", fmtdesc.flags);
		printf("description: %s\n", fmtdesc.description);
		printf("pixelformat: 0x%x\n", fmtdesc.pixelformat);
		printf("********************************\n");

		fmtdesc.index ++;
		fmtdesc.type = buf_type;
	} while(ret==0);

    memset(&fmt, 0x0, sizeof(fmt));
    fmt.type = buf_type;
    fmt.fmt.pix.pixelformat = format;
    if(-1 == xioctl(fd, VIDIOC_TRY_FMT, &fmt)) {
    	printf("VIDIOC_TRY_FMT faild.\n");
    	goto errOut;
    }

    fmt.type = buf_type;
    fmt.fmt.pix.width = width;
    fmt.fmt.pix.height = height;
    fmt.fmt.pix.pixelformat = format;
    fmt.fmt.pix.field = V4L2_FIELD_INTERLACED;
    if (-1 == xioctl(fd, VIDIOC_S_FMT, &fmt)) {
//    	errno_exit("VIDIOC_S_FMT");
    	printf("VIDIOC_S_FMT faild.\n");
    	goto errOut;
    }

    //申请缓存，count是申请的数量
    memset(&req, 0x0, sizeof(req));
    req.count = 4;
    req.type = buf_type;			/* enum v4l2_buf_type */
    req.memory = V4L2_MEMORY_MMAP;	/* enum v4l2_memory */
    if (-1 == xioctl(fd, VIDIOC_REQBUFS, &req)) {
		if (EINVAL == errno) {
			printf("%s does not support " "memory mapping\n", devName);
			//exit(EXIT_FAILURE);
		}
//		else {
//			errno_exit("VIDIOC_REQBUFS");
//		}

		goto errOut;
    }

    if (req.count < 2) {
		printf("Insufficient buffer memory on %s\n", devName);
		//exit(EXIT_FAILURE);
		goto errOut;
    }

    printf("\n");
    printf("count:%d\n", req.count);
    printf("type: 0x%x\n", req.type);
    printf("memory: 0x%x\n", req.memory);

    buffers = (struct buffer *) calloc(req.count, sizeof(*buffers));
    if(!buffers){
    	printf("Out of memory.\n");
    	goto errOut;
    }

    for(nBuffers=0; nBuffers<req.count; nBuffers++){
        CLEAR(buf);
        CLEAR(planes);

        buf.type = buf_type;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = nBuffers;
        if(V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE == buf_type) {
        	buf.m.planes = planes;
        	buf.length = FMT_NUM_PLANES;
        }

        if (-1 == xioctl(fd, VIDIOC_QUERYBUF, &buf)) { //errno_exit("VIDIOC_QUERYBUF");
        	printf("VIDIOC QUERYBUF Faild.\n");
        	goto errOut;
        }

		if (V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE == buf_type) {
			buffers[nBuffers].length = buf.m.planes[0].length;
			buffers[nBuffers].start =
				mmap(NULL /* start anywhere */,
					  buf.m.planes[0].length,
					  PROT_READ | PROT_WRITE /* required */,
					  MAP_SHARED /* recommended */,
					  fd, buf.m.planes[0].m.mem_offset);
		} else {
			buffers[nBuffers].length = buf.length;
			buffers[nBuffers].start =
				mmap(NULL /* start anywhere */,
					  buf.length,
					  PROT_READ | PROT_WRITE /* required */,
					  MAP_SHARED /* recommended */,
					  fd, buf.m.offset);
		}

		if (MAP_FAILED == buffers[nBuffers].start) { //errno_exit("mmap");
			printf("Memory MAP FAILED.\n");
			goto errOut;
		}
    }

    //cameraCapturing();
    for(i=0; i<nBuffers; i++) {
    	CLEAR(buf);
    	CLEAR(planes);

    	buf.type = buf_type;
    	buf.memory = V4L2_MEMORY_MMAP;
    	buf.index = i;

    	if(V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE == buf_type) {
    		buf.m.planes = planes;
    		buf.length = FMT_NUM_PLANES;
    	}

    	if (-1 == xioctl(fd, VIDIOC_QBUF, &buf)) {
    		printf("VIDIOC_QBUF Fail.\n");
    		goto errOut;
    	}

    }

    //开始捕捉图像数据
    type = buf_type;
	if(-1 == xioctl(fd, VIDIOC_STREAMON, &type)){
		printf ("VIDIOC_STREAMON failed\n");
		goto errOut;
	}

	CLEAR(buf);
	buf.type = buf_type;
	buf.memory = V4L2_MEMORY_MMAP;
	if (V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE == buf_type ) {
		buf.m.planes = planes;
		buf.length = FMT_NUM_PLANES;
	}

	if( -1 == xioctl(fd, VIDIOC_DQBUF, &buf)) {
		printf("VIDIOC_DQBUF Fail.\n");
		goto errOut;
	}

	if(V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE == buf_type) {
		p = buffers[buf.index].start;
		size = buf.m.planes[0].bytesused;
	}
	else {
		p = buffers[buf.index].start;
		size = buf.bytesused;
	}

	ret = fwrite(p, size, 1, fp);
	printf("fwrite ret=%d\n", ret);

	if (-1 == xioctl(fd, VIDIOC_QBUF, &buf)) {
		printf("VIDIOC_DQBUF Fail.\n");
		goto errOut;
	}

    type = buf_type;
    if (-1 == xioctl(fd, VIDIOC_STREAMOFF, &type)){
		printf ("VIDIOC_STREAMOFF failed\n");
		goto errOut;
	}

    //


    //
    for (i = 0; i < nBuffers; ++i) {
    	if (-1 == munmap(buffers[i].start, buffers[i].length))
    		printf("munmap fail.\n");
    }
    free(buffers);
    fclose(fp);
    close(fd);

    return 0;

errOut:
	fclose(fp);
	close(fd);
	printf("exit.\n");

	return 0;
}
