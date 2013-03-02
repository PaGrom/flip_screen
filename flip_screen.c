#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>


int set_interface_attribs(int fd, int speed, int parity) {

	struct termios tty;
	memset(&tty, 0, sizeof tty);
	if (tcgetattr(fd, &tty) != 0) {
		printf("error %d from tcgetattr", errno);
		return -1;
	}

	cfsetospeed(&tty, speed);
	cfsetispeed(&tty, speed);

	tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;     // 8-bit chars
	// disable IGNBRK for mismatched speed tests; otherwise receive break
	// as \000 chars
	tty.c_iflag &= ~IGNBRK;         // ignore break signal
	tty.c_lflag = 0;                // no signaling chars, no echo,
									// no canonical processing
	tty.c_oflag = 0;                // no remapping, no delays
	tty.c_cc[VMIN]  = 0;            // read doesn't block
	tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

	tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl

	tty.c_cflag |= (CLOCAL | CREAD);// ignore modem controls,
									// enable reading
	tty.c_cflag &= ~(PARENB | PARODD);      // shut off parity
	tty.c_cflag |= parity;
	tty.c_cflag &= ~CSTOPB;
	tty.c_cflag &= ~CRTSCTS;

	if (tcsetattr(fd, TCSANOW, &tty) != 0) {
		printf("error %d from tcsetattr", errno);
		return -1;
	}

	return 0;
}

void set_blocking(int fd, int should_block) {

	struct termios tty;
	memset (&tty, 0, sizeof tty);
	if (tcgetattr(fd, &tty) != 0) {
		printf("error %d from tggetattr", errno);
		return;
	}

	tty.c_cc[VMIN]  = should_block ? 1 : 0;
	tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

	if (tcsetattr (fd, TCSANOW, &tty) != 0)
		printf("error %d setting term attributes", errno);
}

int main(int argc, char const *argv[]) {

	if (argc < 2) {
		printf("Please start with %s /dev/ttyS1 (for example)\n", argv[0]);
		return;
	}

	int fd = open(argv[1], O_RDWR | O_NOCTTY | O_SYNC);
	if (fd < 0) {
		printf("error %d opening %s: %s\n", errno, argv[1], strerror(errno));
		return;
	}

	set_interface_attribs(fd, B115200, 0);  // set speed to 115,200 bps, 8n1 (no parity)
	set_blocking(fd, 0);                // set no blocking
	
	int rotation;
	char buf[100];

	int is_first_time = 1;

	while (1) {

		write(fd, "hello!\n", 7);           // send 7 character greeting
		
		int n = read(fd, buf, sizeof buf);  // read up to 100 characters if ready to read
		printf("%s %d\n", buf, strlen(buf));

		if (is_first_time) {
			if (strlen(buf))
				rotation = 1;
			else
				rotation = 0;
			is_first_time = 0;
			continue;
		}

		if (strlen(buf)) {
			if (rotation == 0) {
				system("xrandr -o left\n");
				rotation = 1;
			}
		}
		else {
			if (rotation == 1) {
				system("xrandr -o normal\n");
				rotation = 0;
			}
		}
		printf("rotation: %d\n", rotation);
		memset(buf, 0, sizeof(buf));
		sleep(1);
	}

	return 0;
}
