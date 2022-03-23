#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

#include <linux/can.h>
#include <linux/can/raw.h>




int main(int argc, char **argv)
{
    pthread_t thread_read, thread_write;
    
    pthread_create(&thread_write, NULL, CanSendTest, NULL);
    pthread_create(&thread_read, NULL, CanReceiveTest, NULL);
    
    pthread_join(thread_write, NULL);
    pthread_join(thread_read, NULL);

    /*
    if(CanReceiveTest()!=0)
    {
        printf("Erreur receive\n");
    }

    if(CanSendTest()!=0)
    {
        printf("Erreur send\n");
    }
    */
    return 0;
}

int CanReceiveWithFilter()
{
    int s, i; 
	int nbytes;
	struct sockaddr_can addr;
	struct ifreq ifr;
	struct can_frame frame;

	printf("CAN Sockets Receive Filter Demo\r\n");

	if ((s = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
		perror("Socket");
		return 1;
	}

	strcpy(ifr.ifr_name, "vcan0" );
	ioctl(s, SIOCGIFINDEX, &ifr);

	memset(&addr, 0, sizeof(addr));
	addr.can_family = AF_CAN;
	addr.can_ifindex = ifr.ifr_ifindex;

	if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		perror("Bind");
		return 1;
	}

	struct can_filter rfilter[1];

	rfilter[0].can_id   = 0x550;
	rfilter[0].can_mask = 0xFF0;
	//rfilter[1].can_id   = 0x200;
	//rfilter[1].can_mask = 0x700;

	setsockopt(s, SOL_CAN_RAW, CAN_RAW_FILTER, &rfilter, sizeof(rfilter));

	nbytes = read(s, &frame, sizeof(struct can_frame));

	if (nbytes < 0) {
		perror("Read");
		return 1;
	}

	printf("0x%03X [%d] ",frame.can_id, frame.can_dlc);

	for (i = 0; i < frame.can_dlc; i++)
		printf("%02X ",frame.data[i]);

	printf("\r\n");

	if (close(s) < 0) {
		perror("Close");
		return 1;
	}

	return 0;
}

int CanReceiveTest()
{
    int s, i; 
	int nbytes;
	struct sockaddr_can addr;
	struct ifreq ifr;
	struct can_frame frame;

	printf("CAN Sockets Receive Demo\r\n");

	if ((s = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
		perror("Socket");
		return 1;
	}

	strcpy(ifr.ifr_name, "vcan0" );
	ioctl(s, SIOCGIFINDEX, &ifr);

	memset(&addr, 0, sizeof(addr));
	addr.can_family = AF_CAN;
	addr.can_ifindex = ifr.ifr_ifindex;

	if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		perror("Bind");
		return 1;
	}

	nbytes = read(s, &frame, sizeof(struct can_frame));

 	if (nbytes < 0) {
		perror("Read");
		return 1;
	}

	printf("0x%03X [%d] ",frame.can_id, frame.can_dlc);

	for (i = 0; i < frame.can_dlc; i++)
		printf("%02X ",frame.data[i]);

	printf("\r\n");

	if (close(s) < 0) {
		perror("Close");
		return 1;
	}

	return 0;
}


int CanSendTest()
{
    while(1==1)
    {
        int s; 
        struct sockaddr_can addr;
        struct ifreq ifr;
        struct can_frame frame;

        printf("CAN Sockets Demo\r\n");

        if ((s = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
            perror("Socket");
            return 1;
        }

        strcpy(ifr.ifr_name, "vcan0" );
        ioctl(s, SIOCGIFINDEX, &ifr);

        memset(&addr, 0, sizeof(addr));
        addr.can_family = AF_CAN;
        addr.can_ifindex = ifr.ifr_ifindex;

        if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
            perror("Bind");
            return 1;
        }

        frame.can_id = 0x555;
        frame.can_dlc = 5;
        sprintf(frame.data, "Hello");

        if (write(s, &frame, sizeof(struct can_frame)) != sizeof(struct can_frame)) {
            perror("Write");
            return 1;
        }

        if (close(s) < 0) {
            perror("Close");
            return 1;
        }
    }
        
        return 0;
}