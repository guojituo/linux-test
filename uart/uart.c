#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/select.h>

//static const char *const uartDevName[] = {
//		(char*)"/dev/ttysWK0",
//		(char*)"/dev/ttysWK1",
//		(char*)"/dev/ttysWK2",
//		(char*)"/dev/ttysWK3"
//};

//struct termios2 {
//    tcflag_t c_iflag;       /* input mode flags */
//    tcflag_t c_oflag;       /* output mode flags */
//    tcflag_t c_cflag;       /* control mode flags */
//    tcflag_t c_lflag;       /* local mode flags */
//    cc_t c_line;            /* line discipline */
//    cc_t c_cc[19];          /* control characters */
//    speed_t c_ispeed;       /* input speed */
//    speed_t c_ospeed;       /* output speed */
//};

static int uartBaudCalc(int baud)
{
	int speed=-1;

    /* C_ISPEED     Input baud (new interface)
       C_OSPEED     Output baud (new interface)
    */
	switch(baud){
	case 110:
		speed = B110;
		break;
    case 300:
        speed = B300;
        break;
    case 600:
        speed = B600;
        break;
    case 1200:
        speed = B1200;
        break;
    case 2400:
        speed = B2400;
        break;
    case 4800:
        speed = B4800;
        break;
    case 9600:
        speed = B9600;
        break;
    case 19200:
        speed = B19200;
        break;
    case 38400:
        speed = B38400;
        break;
    case 57600:
        speed = B57600;
        break;
    case 115200:
        speed = B115200;
        break;
    case 230400:
    	speed = B230400;
    	break;
    case 460800:
    	speed = B460800;
    	break;
    case 500000:
    	speed = B500000;
    	break;
    case 576000:
    	speed = B576000;
    	break;
	case 1000000:
		speed = B1000000;
		break;
	case 1152000:
		speed = B1152000;
		break;
	case 1500000:
		speed = B1500000;
		break;
	case 2500000:
		speed = B2500000;
		break;
	case 3000000:
		speed = B3000000;
		break;
	case 3500000:
		speed = B3500000;
		break;
	case 4000000:
		speed = B4000000;
		break;
	default:
		speed = -1;
		break;
	}

	return speed;
}
#if 0
static int uartBaudSet(void)
{
	return 0;
}

static void uartDataBitSet(struct termios *tio, int data_bit)
{
    /* Set data bits (5, 6, 7, 8 bits)
       CSIZE        Bit mask for data bits
    */
    tio->c_cflag &= ~CSIZE;
    switch (data_bit) {
    case 5:
        tio->c_cflag |= CS5;
        break;
    case 6:
        tio->c_cflag |= CS6;
        break;
    case 7:
        tio->c_cflag |= CS7;
        break;
    case 8:
    	tio->c_cflag |= CS8;
    	break;
    default:
        tio->c_cflag |= CS8;
        break;
    }
}

static void uartParitySet(struct termios *tio, int parity)
{
    /* PARENB       Enable parity bit
       PARODD       Use odd parity instead of even */
    if (parity == 'N') {
        /* None */
        tio->c_cflag &=~ PARENB;
    } else if (parity == 'E') {
        /* Even */
        tio->c_cflag |= PARENB;
        tio->c_cflag &=~ PARODD;
    } else {
        /* Odd */
        tio->c_cflag |= PARENB;
        tio->c_cflag |= PARODD;
    }
}

static void uartStopBitSet(struct termios *tio, int stop_bit)
{
    /* Stop bit (1 or 2) */
    if (stop_bit == 1)
        tio->c_cflag &=~ CSTOPB;
    else /* 2 */
        tio->c_cflag |= CSTOPB;
}

static int uartPortInit(int uartPort, struct termios *tio)
{
    tio->c_cflag |= CLOCAL;
    tio->c_cc[VTIME] = 0;
    tio->c_cc[VMIN] = 0;
}
#endif

//port   : 0 1 2 3
//badu   : 110 300 600 1200 2400 4800 9600 19200 38400 57600 115200 default[9600]
//parity : 'N' 'E' 'O'
//data_bit : 5 6 7 8
//stop_bit : 1 2
int uartInit(const char *devName, int baud, char parity, int data_bit, int stop_bit)
{
	int s;
	int speed;
	struct termios tios;
	struct termios tios_old;

	if(!devName) return -1;

	printf("dev: %s\n", devName);
    s = open(devName, O_RDWR | O_NOCTTY | O_NDELAY | O_EXCL | O_CLOEXEC);
    if (s == -1) {
        printf("ERROR Can't open the device %s\r\n",devName);
        return -1;
    }

    tcgetattr(s, &tios_old);
    memset(&tios, 0x0, sizeof(struct termios));

    speed = uartBaudCalc(baud);
    if(speed == -1){
    	printf("not support baud(%d)\n", baud);
    	close(s);
    	return -1;
    }

    /* Set the baud rate */
    if ((cfsetispeed(&tios, speed) < 0) ||
        (cfsetospeed(&tios, speed) < 0)) {
        close(s);
        s = -1;
        return -1;
    }

// termios2 ???
// struct termios2 tio;
//    ioctl(s, TCGETS2, &tio);
//    tio.c_cflag &= ~CBAUD;
//    tio.c_cflag |= BOTHER;
//    tio.c_ispeed = speed;
//    tio.c_ospeed = speed;
//    ioctl(s, TCSETS2, &tio);
//    return s;

    /* C_CFLAG      Control options
       CLOCAL       Local line - do not change "owner" of port
       CREAD        Enable receiver
    */
    tios.c_cflag |= (CREAD | CLOCAL);
    /* CSIZE, HUPCL, CRTSCTS (hardware flow control) */

    /* Set data bits (5, 6, 7, 8 bits)
       CSIZE        Bit mask for data bits
    */
    tios.c_cflag &= ~CSIZE;
    switch (data_bit) {
    case 5:
        tios.c_cflag |= CS5;
        break;
    case 6:
        tios.c_cflag |= CS6;
        break;
    case 7:
        tios.c_cflag |= CS7;
        break;
    case 8:
    default:
        tios.c_cflag |= CS8;
        break;
    }

    /* Stop bit (1 or 2) */
    if (stop_bit == 1)
        tios.c_cflag &=~ CSTOPB;
    else /* 2 */
        tios.c_cflag |= CSTOPB;

    /* PARENB       Enable parity bit
       PARODD       Use odd parity instead of even */
    if (parity == 'N') {
        /* None */
        tios.c_cflag &=~ PARENB;
    } else if (parity == 'E') {
        /* Even */
        tios.c_cflag |= PARENB;
        tios.c_cflag &=~ PARODD;
    } else {
        /* Odd */
        tios.c_cflag |= PARENB;
        tios.c_cflag |= PARODD;
    }

    /* Read the man page of termios if you need more information. */

    /* This field isn't used on POSIX systems
       tios.c_line = 0;
    */

    /* C_LFLAG      Line options

       ISIG Enable SIGINTR, SIGSUSP, SIGDSUSP, and SIGQUIT signals
       ICANON       Enable canonical input (else raw)
       XCASE        Map uppercase \lowercase (obsolete)
       ECHO Enable echoing of input characters
       ECHOE        Echo erase character as BS-SP-BS
       ECHOK        Echo NL after kill character
       ECHONL       Echo NL
       NOFLSH       Disable flushing of input buffers after
       interrupt or quit characters
       IEXTEN       Enable extended functions
       ECHOCTL      Echo control characters as ^char and delete as ~?
       ECHOPRT      Echo erased character as character erased
       ECHOKE       BS-SP-BS entire line on line kill
       FLUSHO       Output being flushed
       PENDIN       Retype pending input at next read or input char
       TOSTOP       Send SIGTTOU for background output

       Canonical input is line-oriented. Input characters are put
       into a buffer which can be edited interactively by the user
       until a CR (carriage return) or LF (line feed) character is
       received.

       Raw input is unprocessed. Input characters are passed
       through exactly as they are received, when they are
       received. Generally you'll deselect the ICANON, ECHO,
       ECHOE, and ISIG options when using raw input
    */

    /* Raw input */
    tios.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);

    /* C_IFLAG      Input options

       Constant     Description
       INPCK        Enable parity check
       IGNPAR       Ignore parity errors
       PARMRK       Mark parity errors
       ISTRIP       Strip parity bits
       IXON Enable software flow control (outgoing)
       IXOFF        Enable software flow control (incoming)
       IXANY        Allow any character to start flow again
       IGNBRK       Ignore break condition
       BRKINT       Send a SIGINT when a break condition is detected
       INLCR        Map NL to CR
       IGNCR        Ignore CR
       ICRNL        Map CR to NL
       IUCLC        Map uppercase to lowercase
       IMAXBEL      Echo BEL on input line too long
    */
    if (parity == 'N') {
        /* None */
        tios.c_iflag &= ~INPCK;
    } else {
        tios.c_iflag |= INPCK;
    }

    /* Software flow control is disabled */
    tios.c_iflag &= ~(IXON | IXOFF | IXANY);

    /* C_OFLAG      Output options
       OPOST        Postprocess output (not set = raw output)
       ONLCR        Map NL to CR-NL

       ONCLR ant others needs OPOST to be enabled
    */

    /* Raw ouput */
    tios.c_oflag &=~ OPOST;

    /* C_CC         Control characters
       VMIN         Minimum number of characters to read
       VTIME        Time to wait for data (tenths of seconds)

       UNIX serial interface drivers provide the ability to
       specify character and packet timeouts. Two elements of the
       c_cc array are used for timeouts: VMIN and VTIME. Timeouts
       are ignored in canonical input mode or when the NDELAY
       option is set on the file via open or fcntl.

       VMIN specifies the minimum number of characters to read. If
       it is set to 0, then the VTIME value specifies the time to
       wait for every character read. Note that this does not mean
       that a read call for N bytes will wait for N characters to
       come in. Rather, the timeout will apply to the first
       character and the read call will return the number of
       characters immediately available (up to the number you
       request).

       If VMIN is non-zero, VTIME specifies the time to wait for
       the first character read. If a character is read within the
       time given, any read will block (wait) until all VMIN
       characters are read. That is, once the first character is
       read, the serial interface driver expects to receive an
       entire packet of characters (VMIN bytes total). If no
       character is read within the time allowed, then the call to
       read returns 0. This method allows you to tell the serial
       driver you need exactly N bytes and any read call will
       return 0 or N bytes. However, the timeout only applies to
       the first character read, so if for some reason the driver
       misses one character inside the N byte packet then the read
       call could block forever waiting for additional input
       characters.

       VTIME specifies the amount of time to wait for incoming
       characters in tenths of seconds. If VTIME is set to 0 (the
       default), reads will block (wait) indefinitely unless the
       NDELAY option is set on the port with open or fcntl.
    */
    /* Unused because we use open with the NDELAY option */
    tios.c_cc[VMIN] = 0;
    tios.c_cc[VTIME] = 0;

    if (tcsetattr(s, TCSANOW, &tios) < 0) {
        close(s);
        s = -1;
        return -1;
    }

	return s;
}

int uartDeInit(int uartHandle)
{
	close(uartHandle);
	return 0;
}

int uartSend(int uartHandle, const unsigned char *buf, const unsigned int len)
{
	int ret;
	if(!buf) return -1;
	ret = write(uartHandle, buf, len);
	if(ret != len) return 0;
	return ret;
}

int uartRecv(int uartHandle, unsigned char *buf, const unsigned int bufSize, const int timeout)
{
	int rc, len;
	fd_set rfds;
	struct timeval tv, *ptv;

	if(uartHandle<1) return -1;

	FD_ZERO(&rfds);
	FD_SET(uartHandle, &rfds);
	if(timeout == 0) {
		ptv = NULL;
	}
	else {
		tv.tv_sec = timeout/1000;
		tv.tv_usec = (timeout%1000) * 1000 * 1000;
		ptv = &tv;
	}

	rc = FD_ISSET(uartHandle, &rfds);
	if(!rc) printf("uartHandle is not set???\n");

	do {
		rc = select(uartHandle+1, &rfds, NULL, NULL, ptv);
		if(rc==0) {
			printf("select timeout.\n");
			break;
		}
		else if(rc==-1) {
			printf("select fail.\n");
			FD_ZERO(&rfds);
			FD_SET(uartHandle, &rfds);
		}
	}while(rc==-1);

	len = read(uartHandle, buf, bufSize);

	return len;
}



