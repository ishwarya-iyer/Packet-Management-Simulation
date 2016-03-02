#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <errno.h>
#include "cs402.h"

#include "my402list.h"


typedef struct packet
{
    int packet_no;
    double lambda;
    int tok;
    double service;
    double arrivl;
    double q1_entry;
    double q1_exit;
    double q2_entry;
    double q2_exit;
    double server_entry;
    double server_exit;
    
} pkt_struct;



extern int i;
extern pthread_t arrival, token_thrd, server1, server2,cntrlthrd;
struct timeval time1;
extern void *token_gen();
extern void *arrival_thrd();
extern void *server_1();
extern void *server_2();
extern void *monitor();
extern void clean();
extern void statistics();
extern double t,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,tend;
extern double s1,s2,s3,s4,s5,s6;
extern int j;
extern int complete;

extern int num;
extern int B;
extern int P;
extern sigset_t set;
extern double r;
extern double lambda;
extern double mu;
extern int flag;
extern double sq;


extern FILE *fp;


My402List q1;
My402List q2;
