#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <errno.h>
#include <string.h>
#include <math.h>
#include "cs402.h"

#include "my402list.h"

#include "thr_func.h"
//void *token_gen(void *);

int num = 20;

int B = 10;
int P = 3;
double r = 1.5;
double lambda = 0.5;
double mu = 0.35;
int flag=0;
struct timeval time1;
double t;
pthread_t arrival, token_thrd, server1, server2,cntrlthrd;
int main(int argc,char *argv[])
{

sigemptyset(&set);
sigaddset(&set,SIGINT);
sigprocmask(SIG_BLOCK,&set, 0);
    
//time.tv_sec*1000000)+(time.tv_usec
int arg;char *str;char buff[1024]; int filearg=0;
    if(argc==2) {
                fprintf(stderr,"%s muct have a value\n",argv[argc-1]);
                return 0;
            }
for(arg=1;argc-1>arg;arg=arg+2)
{
if(!strcmp(argv[arg],"-lambda"))
   { lambda=atof(argv[arg+1]);
    if(lambda == 0)
            {
                fprintf(stderr,"%s should have a positive value\n", argv[arg]);
                return 0;
            }
   
   }
    
else if(!strcmp(argv[arg],"-mu"))
   { mu=atof(argv[arg+1]);
    if(mu == 0)
            {
                fprintf(stderr,"%s should have a positive value\n", argv[arg]);
                return 0;
            }
  
   }

else if(!strcmp(argv[arg],"-r"))
   { r=atof(argv[arg+1]);
    if(r == 0)
            {
                fprintf(stderr,"%s should have a positive value\n", argv[arg]);
                return 0;
            }
  
   }

else if(!strcmp(argv[arg],"-B"))
   { B=atoi(argv[arg+1]);
    if(B == 0||B>2147483647)
            {
                fprintf(stderr,"%s should have a positive valid value\n", argv[arg]);
                return 0;
            }
  
   }

else if(!strcmp(argv[arg],"-P"))
   { P=atoi(argv[arg+1]);
    if(P == 0||P>2147483647)
            {
                fprintf(stderr,"%s should have a positive valid value\n", argv[arg]);
                return 0;
            }
  
   }
    
else if(!strcmp(argv[arg],"-n"))
   { num=atoi(argv[arg+1]);
    if(num == 0||num>2147483647)
            {
                fprintf(stderr,"%s should have a positive valid value\n", argv[arg]);
                return 0;
            }
  
   }

else if(!strcmp(argv[arg],"-t"))
   { fp=fopen(argv[arg+1],"r");
    if(fp==NULL)
            {
                fprintf(stderr,"cannot read the file %s\n", argv[arg+1]);
                return 0;
            }
    flag=1;
    filearg=arg+1;
    
    fgets(buff,sizeof(buff),fp);
     str = buff;

    num=atoi(str);
    if(num==0)
            {
                fprintf(stderr,"Wrong format of file %s\n", argv[arg+1]);
                return 0;
            }
   }
    
 else
     if(argc%2==0)
     {
      fprintf(stderr,"Invalid commandline format, each parameter should have a value\n");
                return 0;
 }
 else {
      fprintf(stderr,"Invalid parameter %s\n", argv[arg]);
                return 0;
 }

}
    
//output parameters    
printf("Emulation Parameters:\n");
printf("Number to arrive = %d\n",num);
    
if(flag==0)
    {
    printf("lambda = %f\n",lambda);
    printf("mu = %f\n",mu);
    
    }
printf("r = %f\n",r);
printf("B = %d\n",B);
    
if(flag==0)
    printf("P = %d\n",P);  
if(flag==1)   
    printf("tsfile = %s\n",argv[filearg]);  
    

    gettimeofday(&time1,0);
    t= time1.tv_sec+ (time1.tv_usec/1000000.0);
    printf("%012.3fms : emulation begins:\n",t-t);
    pthread_create(&arrival,NULL,arrival_thrd,NULL);
   pthread_create(&token_thrd,NULL,token_gen,NULL);
   pthread_create(&server1,NULL,server_1,NULL);
   pthread_create(&server2,NULL,server_2,NULL);
   pthread_create(&cntrlthrd, 0,monitor, 0);

    pthread_join(arrival,NULL);
   pthread_join(token_thrd,NULL);
   pthread_join(server1,NULL);
   pthread_join(server2,NULL);
       statistics();
return (0);
}
