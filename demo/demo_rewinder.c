/* 
 * Node.js Communication test
 * Copyright(c) 2017 Earth Computing.
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
#include <time.h>   // for nanosleep

FILE *log_file = NULL ;

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

char json_string[512];

char *entlStateString[] = {"IDLE","HELLO","WAIT","SEND","RECEIVE","AM","BM","AH","BH","ERROR"};

void main( int argc, char *argv[] ) {
	int port, a_len ;
	int count = 0 ;
	int err ;
  int link[4] = {0,1,0,1} ;
  char *fname ;
  struct timespec ts;
  struct timespec tp;

  ts.tv_sec = 0;
  ts.tv_nsec = 500000000; // .5 sec

  tp.tv_sec = 0;
  tp.tv_nsec = 40000000; // .04 sec


  //if( argc != 2 ) {
  //  printf( "%s needs <server address> (e.g. 127.0.0.1) as the argument\n", argv[0] ) ;
  //  return ;
  //}
  //printf( "Server Address: %s \n", argv[1] ) ;
  if( argc == 2 ) {
      fname = argv[1] ;
      printf( "fname:%s\n", fname ) ;
      log_file = fopen( fname, "r") ;
      if (! log_file ) {
        printf( "Can't open log file %s\n", log_file ) ;
        exit(1) ;
      }
  }
  else {

    exit(1) ;
  }

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

  while(fgets(json_string,sizeof json_string,log_file)!= NULL) /* read a line from a file */ {
    write( w_socket, json_string, strlen(json_string) ) ;
    printf( "%s\n", json_string) ;
    if( strcmp( json_string, "{\"state0") > 0 )
      nanosleep(&ts, NULL);
    else
      nanosleep(&tp, NULL);

  }

  fclose(log_file);


		
		//printf( "%s\n", message ) ;




}