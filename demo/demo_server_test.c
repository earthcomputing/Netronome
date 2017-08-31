/* 
 * Node.js Communication test
 * Copyright(c) 2016 Earth Computing.
 *
 *  Author: Atsushi Kasuya
 *
 */

#include <stdio.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <sys/socket.h>
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

static int sockfd, w_socket ;
static struct sockaddr_in sockaddr, w_sockaddr ;

static int sin_port ;

#define PRINTF printf
#define DEFAULT_DBG_PORT  1337
#define NUM_INTERFACES 4

static char *port_name[NUM_INTERFACES] = {"enp6s0","enp7s0","enp8s0","enp9s0"};

static int open_socket( char *addr ) {
  	int st = -1 ;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if( sockfd < 0 ) {
	    PRINTF( "Can't create socket\n" ) ;
	    return 0 ;
	}

	sockaddr.sin_family = AF_INET ;
	sockaddr.sin_addr.s_addr = inet_addr(addr) ; 

  sockaddr.sin_port = htons(DEFAULT_DBG_PORT) ;
    st = connect( sockfd, (struct sockaddr *) &sockaddr, sizeof(struct sockaddr) );
	sin_port = DEFAULT_DBG_PORT ;

	if( st < 0 ) {
	    PRINTF( 
	      "Can't bind socket to the port %d\n",
	      sockaddr.sin_port
	    ) ;
	    close( sockfd ) ;
	    return 0 ;
	}
	else {
	    
	    PRINTF( 
	      "Bind socket to the port %d %d\n",
	      sockaddr.sin_port, sin_port
	    ) ;
	    
	}
	return sin_port ; // sockaddr.sin_port ;
}

#define INMAX 1024
static char inlin[INMAX];

static int read_window() {
 	int rr, n;
   	n = INMAX;
   	//printf( "calling read\n" ) ;
   	rr = read(w_socket,inlin,n);
   	//printf( "done read with %d \n", rr ) ;
   	if (rr <= 0) {
      	rr = 0;
   	}
   	inlin[rr] = '\0';
   	printf( "got %s\n", inlin ) ;
   	return rr ;
}

static pthread_t read_thread ;


char *entlStateString[] = {"IDLE","HELLO","WAIT","SEND","RECEIVE","AM","BM","AH","BH","ERROR"};

void main( int argc, char *argv[] ) {
	int port, a_len ;
	int count = 0 ;
	int err ;
  int link[4] = {0,1,0,1} ;

  //if( argc != 2 ) {
  //  printf( "%s needs <server address> (e.g. 127.0.0.1) as the argument\n", argv[0] ) ;
  //  return ;
  //}
  //printf( "Server Address: %s \n", argv[1] ) ;

	port = open_socket( "127.0.0.1" ) ;

  if( !port ) {
	    printf( "Can't open socket\n" ) ;
 		return ;
	}
  printf( "got port %d\n", port ) ;

	printf( "listening %d\n", port ) ;

	//if( listen( sockfd, 5 ) < 0 ) {
  //  PRINTF( "listen error on port %d\n", port ) ;
  //  close( sockfd ) ;
  //  return ;
  //}
 	//printf( "accepting %d\n", port ) ;

  //a_len = sizeof(w_sockaddr) ;
  w_socket =  sockfd ; // accept( sockfd, (struct sockaddr *)&w_sockaddr, &a_len ) ;

  printf( "exit accept\n") ;
		//ioctl(sockfd, FIONBIO, &iMode);  
  if( 0 ) { // w_socket < 0) {
      printf( "accept error on port %d\n", port ) ;
      close( sockfd ) ;
      return ;
  }
  printf( "accept on %d\n", w_socket ) ;


  //err = pthread_create( &read_thread, NULL, read_task, NULL );


	while( 1 ) {
		char message[512] ;
    char s0r0[20], s0r1[20], s0r2[20], s0r3[20], s0r4[20], buf0[20] ;
    char s1r0[20], s1r1[20], s1r2[20], s1r3[20], s1r4[20], buf1[20] ;
    int port = (count%4) ;
  	//printf( "sleeping 1 sec on %d\n", count ) ;

		sleep(1) ;
    sprintf( s0r0, "s0r0 %d", count ) ;
    sprintf( s0r1, "s0r1 %d", count+1 ) ;
    sprintf( s0r2, "s0r2 %d", count+2 ) ;
    sprintf( s0r3, "s0r3 %d", count+3 ) ;
    sprintf( s0r4, "s0r4 %d", count+4 ) ;
    sprintf( buf0, "buf0 %d", count+200 ) ;
   sprintf( s1r0, "s1r0 %d", count ) ;
    sprintf( s1r1, "s1r1 %d", count+1 ) ;
    sprintf( s1r2, "s1r2 %d", count+2 ) ;
    sprintf( s1r3, "s1r3 %d", count+3 ) ;
    sprintf( s1r4, "s1r4 %d", count+4 ) ;
    sprintf( buf1, "buf1 %d", count+200 ) ;
    sprintf( message ,"{\"state0\": %d, \"state1\": %d, \"s0r0\": \"%s\", \"s0r1\": \"%s\", \"s0r2\": \"%s\", \"s0r3\": \"%s\", \"s0r4\": \"%s\", \"buf0\": \"%s\", \"s1r0\": \"%s\", \"s1r1\": \"%s\", \"s1r2\": \"%s\", \"s1r3\": \"%s\", \"s1r4\": \"%s\", \"buf1\": \"%s\" }",
       count % 7, (count + 3 ) % 7, s0r0, s0r1, s0r2, s0r3, s0r4, buf0, s1r0, s1r1, s1r2, s1r3, s1r4, buf1 
    );

    printf( "%s\n", message ) ;

    count++ ;

		write( w_socket, message, strlen(message) ) ;
		//printf( "%s\n", message ) ;

	}


}