/* 
 * Netronome NIC Data Tester Test
 * Copyright(c) 2017 Earth Computing.
 *
 *  Author: Atsushi Kasuya
 *
 */

#include <stdio.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <net/ethernet.h>
#include <sys/socket.h>
#include <linux/if_packet.h>
#include <linux/if_ether.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <errno.h>
 
#define ETH_FRAME_LEN 1518
#define ETH_P_ECLP  0xEAC0    /* Earth Computing Link Protocol [ NOT AN OFFICIALLY REGISTERED ID ] */

typedef pthread_mutex_t mutex_t;
static mutex_t access_mutex ;
static mutex_t write_mutex ;

#define ACCESS_LOCK pthread_mutex_lock( &access_mutex )  
#define ACCESS_UNLOCK pthread_mutex_unlock( &access_mutex )

#define WRITE_LOCK pthread_mutex_lock( &write_mutex )  
#define WRITE_UNLOCK pthread_mutex_unlock( &write_mutex )

#define PRINTF printf
#define DEFAULT_DBG_PORT  2540

//#include "entl_state_defs.h"

//#ifndef STANDALONE_DEBUG

//#include "entl_user_api.h"

//#endif

#define DUMP_WINDOW_ONLY

static int sock, sock_r ;
static struct sockaddr_ll saddr, daddr;

static pthread_t receive_thread ;


static void receive_task( void *me ) {
    unsigned char *buffer=malloc(ETH_FRAME_LEN);
	struct ethhdr *eth = (struct ethhdr *)buffer;
    int data_size ;
    int saddr_size = sizeof(struct sockaddr) ;
	printf( "receive_task started\n") ;
    while(1) {
    	//if( entangled ) {
    		memset( buffer, 0, ETH_FRAME_LEN) ;
	    	data_size = recvfrom(sock_r , buffer , ETH_FRAME_LEN , 0 ,(struct sockaddr *) &saddr , (socklen_t*)&saddr_size);
	        if(data_size <0 )
	        {
	            //printf("Recvfrom error , failed to get packets\n");
	            sleep(1) ;
	        }
	        else if( eth->h_proto == htons(ETH_P_ECLP) )
	        {
	        	printf("Received %d bytes\n",data_size);
	        	char *data = &buffer[14] ;
	        	printf( "  data: %s\n", data ) ;
	        }
	        else {
	        	printf("Received %d bytes t: %04x\n", data_size, eth->h_proto );
	        }
    	//}
    	//else {
    	//	sleep(1) ;
    	//}

    }
}

//#define MAC_FMT "%02x:%02x:%02x:%02x:%02x:%02x"

int main( int argc, char *argv[] ) {
	int count = 0 ;
	int err ;
	//u32 event_i_know = 0 ;
	char *sname = argv[1] ;
	char *dname = argv[2] ;
    unsigned char *buffer=malloc(ETH_FRAME_LEN);
    unsigned char *data = &buffer[14] ;
	struct ethhdr *eth = (struct ethhdr *)buffer;
	struct ifreq if_macs;
	struct ifreq if_macd;

	if( argc != 3 ) {
		printf( "%s needs <src device name> <dst device name> (e.g. enp6s0 enp6s1) as the argument\n", argv[0] ) ;
		return 0 ;
	}
  	printf( "Loopback test on %s to %s.. \n", argv[1], argv[2] ) ;

	// Creating socet
	if ((sock = socket(AF_PACKET, SOCK_RAW, IPPROTO_RAW)) < 0) {
		perror("cannot create socket");
		return 0;
	}
	memset(&if_macs, 0, sizeof(struct ifreq));
	strncpy(if_macs.ifr_name, sname, IFNAMSIZ-1);
	if (ioctl(sock, SIOCGIFHWADDR, &if_macs) < 0) {
	    perror("SIOCGIFHWADDR");
	    return 0;
	}

	memset(&if_macd, 0, sizeof(struct ifreq));
	strncpy(if_macd.ifr_name, dname, IFNAMSIZ-1);
	if (ioctl(sock, SIOCGIFHWADDR, &if_macd) < 0) {
	    perror("SIOCGIFHWADDR");
	    return 0;
	}

	printf( "%s = %02x:%02x:%02x:%02x:%02x:%02x\n", if_macs.ifr_name,
		((uint8_t *)&if_macs.ifr_hwaddr.sa_data)[0],
		((uint8_t *)&if_macs.ifr_hwaddr.sa_data)[1],
		((uint8_t *)&if_macs.ifr_hwaddr.sa_data)[2],
		((uint8_t *)&if_macs.ifr_hwaddr.sa_data)[3],
		((uint8_t *)&if_macs.ifr_hwaddr.sa_data)[4],
		((uint8_t *)&if_macs.ifr_hwaddr.sa_data)[5]
	 ) ;

	printf( "%s = %02x:%02x:%02x:%02x:%02x:%02x\n", if_macd.ifr_name,
		((uint8_t *)&if_macd.ifr_hwaddr.sa_data)[0],
		((uint8_t *)&if_macd.ifr_hwaddr.sa_data)[1],
		((uint8_t *)&if_macd.ifr_hwaddr.sa_data)[2],
		((uint8_t *)&if_macd.ifr_hwaddr.sa_data)[3],
		((uint8_t *)&if_macd.ifr_hwaddr.sa_data)[4],
		((uint8_t *)&if_macd.ifr_hwaddr.sa_data)[5]
	 ) ;

	sock_r = socket( PF_PACKET , SOCK_RAW , htons(ETH_P_ALL)) ; // receiveing socket
    if( sock_r < 0 ) {
    	printf( "Can't open PF_PACKET socket, should be run on su\n") ;
    	exit(1) ;
    }

    memset(&saddr, 0, sizeof(struct sockaddr_ll));
    saddr.sll_family = AF_PACKET;
    saddr.sll_protocol = htons(ETH_P_ALL);
    //saddr.sll_protocol = htons(ETH_P_ECLP);
    saddr.sll_ifindex = if_nametoindex(sname);
    //saddr.sll_hatype = ARPHRD_ETHER ;
    saddr.sll_pkttype = PACKET_OTHERHOST ;

    printf( "got ifindex %d\n", saddr.sll_ifindex ) ;

    err = pthread_create( &receive_thread, NULL, receive_task, NULL );

	sleep(10) ;

  	while( 1 ) {
  		int send_result ;
		memset(buffer, 0, ETH_FRAME_LEN);
		eth->h_dest[0] = ((uint8_t *)&if_macd.ifr_hwaddr.sa_data)[0] ;
		eth->h_dest[1] = ((uint8_t *)&if_macd.ifr_hwaddr.sa_data)[1] ;
		eth->h_dest[2] = ((uint8_t *)&if_macd.ifr_hwaddr.sa_data)[2] ;
		eth->h_dest[3] = ((uint8_t *)&if_macd.ifr_hwaddr.sa_data)[3] ;
		eth->h_dest[4] = ((uint8_t *)&if_macd.ifr_hwaddr.sa_data)[4] ;
		eth->h_dest[5] = ((uint8_t *)&if_macd.ifr_hwaddr.sa_data)[5] ;
		eth->h_source[0] = ((uint8_t *)&if_macs.ifr_hwaddr.sa_data)[0] ;
		eth->h_source[1] = ((uint8_t *)&if_macs.ifr_hwaddr.sa_data)[1] ;
		eth->h_source[2] = ((uint8_t *)&if_macs.ifr_hwaddr.sa_data)[2] ;
		eth->h_source[3] = ((uint8_t *)&if_macs.ifr_hwaddr.sa_data)[3] ;
		eth->h_source[4] = ((uint8_t *)&if_macs.ifr_hwaddr.sa_data)[4] ;
		eth->h_source[5] = ((uint8_t *)&if_macs.ifr_hwaddr.sa_data)[5] ;
		eth->h_proto = htons(ETH_P_ECLP) ;
		sprintf( data, "%s to %s Data %d", sname, dname, count ) ; ;


		send_result = sendto( sock_r, buffer, 100, 0, (struct sockaddr*)&saddr, sizeof(saddr));

		if (send_result < 0 ) {
			printf( "sendto failed on %s at %d errno:%d \n",sname, count, errno );
		}
		else {
			printf( "sent %d on %s at %d\n", send_result, sname, count );
		}

		count++ ;
		sleep(5) ;
  	}
}


