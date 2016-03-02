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
int serviced=0;
int pkt_drop = 0;
int token= 0;//current tok
int tot_token= 0;//num of token so far
int tok_dropped= 0;
int j=0;
int complete=0;
double sq=0.0;
 double time_spent_q1=0;
 double time_spent_q2=0;
 double time_spent_s1=0;
 double time_spent_s2=0;
 double avg_inter=0.0;
double sys_spent=0.0;
int stop_flag=0;
int removed=0;int i;
double end1,end2;
double t,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,tend;
double s1,s2,s3,s4,s5,s6;
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cv = PTHREAD_COND_INITIALIZER;
FILE *fp;
pthread_t arrival, token_thrd, server1, server2,cntrlthrd;
sigset_t set;
 void *token_gen();
 void *arrival_thrd();
 void *server_1();
 void *server_2();
 void *monitor();
 void clean();
 void statistics();

void *arrival_thrd()
{
(void)My402ListInit(&q1);
(void)My402ListInit(&q2);
char buf[1026];char *str_ptr, *tab_ptr;char *str;
pkt_struct *pkt=NULL;
double last_pkt_time=t; 
int time_to_sleep;
for(i=1;i<=num;i++)
    {
    /*----------------------packet initialization starts--------------------*/
    pkt=malloc(sizeof(struct packet));
    pkt->packet_no=i;
    pkt->arrivl=0.0;
    pkt->q1_entry=0.0;
    pkt->q1_exit=0.0;
    pkt->q2_entry=0.0;
    pkt->q2_exit=0.0;
    pkt->server_entry=0.0;
    pkt->server_exit=0.0;
    if(flag==1)
    {
        if(fgets( buf, sizeof(buf), fp ) != NULL )
        {
        double service,inter; int tok;
        str_ptr = buf;
            while(*str_ptr == ' ' || *str_ptr == '\t')
                {
                    str_ptr=str_ptr+1;
                }
        tab_ptr = strchr(str_ptr,' ');
         if(tab_ptr==0)
         {
                fprintf(stderr,"Wrong format of file\n");
                exit(0);
        }
	   *tab_ptr++ ='\0';
        inter=atof(str_ptr);
        if(inter==0)
            {
                fprintf(stderr,"Wrong format of file specifying the lambda value in line %d\n",i);
                 exit(0);
            }
        pkt->lambda=inter;
       
            while(*tab_ptr == ' ' || *tab_ptr == '\t')
                {
                    tab_ptr=tab_ptr+1;
                }
            str_ptr=tab_ptr;
        tab_ptr = strchr(str_ptr,' ');
	 if(tab_ptr==0)
        {
                fprintf(stderr,"Wrong format of file\n");
                 exit(0);
        }
	*tab_ptr++ ='\0';	
    tok=atoi(str_ptr);
        if(tok==0)
            {
                fprintf(stderr,"Wrong format of file specifing the tokens required in line %d\n",i);
                  exit(0);
            }
        pkt->tok=tok; 
             while(*tab_ptr == ' ' || *tab_ptr == '\t')
                {
                    tab_ptr=tab_ptr+1;
             }
        str_ptr = tab_ptr;
       if(*str_ptr=='\n')
     {
                fprintf(stderr,"Empty service time field in line %d\n",i);
                  exit(0);
        }
	str = strchr(str_ptr,'\n');
	if(!str) {
                fprintf(stderr,"Wrong format of file\n");
                exit(0);}
	*str++='\0';
      service=atof(str_ptr);
            if(!service)
              {
                fprintf(stderr,"Wrong format of file specifing the service time required in line %d\n",i);
                 exit(0);
            }  
        pkt->service=service;
        }//end of file-line parsing      
    }//end of if file input
     else
    {
        double inter = (1000.0)/lambda;
		if( inter > 10000 )
		{	
			inter = 10000.0;
		}
		pkt->lambda = inter;
		pkt->tok = P;
		double service = (1000.0)/mu;
		if( service > 10000 )
		{
			service = 10000.0;
		}
    pkt->service=service;
    }
    /*--------------------------------packet initialization ends------------------------------*/
    gettimeofday(&time1,0);
    t1= (time1.tv_sec)+ (time1.tv_usec/1000000.0);
    time_to_sleep=((pkt->lambda)*1000)-(t1-last_pkt_time);
    if(time_to_sleep<=1000)
       { long slp;
        struct timespec wait1;
         slp=(long) time_to_sleep;
         wait1.tv_sec=slp/1000000;
         wait1.tv_nsec= (slp%1000000)*1000;
         nanosleep(&wait1,NULL);}
    else usleep(time_to_sleep);
    gettimeofday(&time1,0);
    t2= (time1.tv_sec)+ (time1.tv_usec/1000000.0);
     avg_inter=((avg_inter*(i-1))+((t2-last_pkt_time)*1000))/i;
    if(pkt->tok>B)
    {
        printf("%012.3fms : p%d arrives, needs %d tokens,inter-arrival time = %.3fm, dropped\n",(t2-t)*1000,i,pkt->tok,(t2-last_pkt_time)*1000);
     pkt_drop++;
      last_pkt_time=t2;
     //pthread_mutex_unlock(&mutex1);
    }
    else
    {j++;
        printf("%012.3fms : p%d arrives, needs %d tokens, inter-arrival time = %.3fms\n",(t2-t)*1000,i,pkt->tok,(t2-last_pkt_time)*1000);
        last_pkt_time=t2;
        pkt->arrivl=t2;
    pthread_mutex_lock(&mutex1);
        (void)My402ListAppend(&q1, pkt);
    pthread_mutex_unlock(&mutex1);
        gettimeofday(&time1,0);
        t3= (time1.tv_sec)+ (time1.tv_usec/1000000.0);
        printf("%012.3fms : p%d enters Q1\n",(t3-t)*1000,i);
        pkt->q1_entry=t3;
    
        if(My402ListLength(&q1)==1&&(pkt->tok)<=token)
        {
        My402ListElem *elem=My402ListFirst(&q1);
        pkt_struct *first=elem->obj;
    pthread_mutex_lock(&mutex1);
           token-=first->tok;
           My402ListUnlink(&q1, elem);
 
           gettimeofday(&time1,0);
           t4= (time1.tv_sec)+ (time1.tv_usec/1000000.0);
           first->q1_exit=t4;
           printf("%012.3fms : p%d leaves Q1,time in Q1 = %.3fms, token bucket has now %d tokens\n",(t4-t)*1000,first->packet_no,(t4-first->q1_entry)*1000,token);  
           time_spent_q1+=(t4-first->q1_entry);
           (void)My402ListAppend(&q2, first);
            pthread_cond_broadcast(&cv);// check it out-----------------------------------------------------------!
   
            gettimeofday(&time1,0);
           t5= (time1.tv_sec)+ (time1.tv_usec/1000000.0);
           first->q2_entry=t5; 
           printf("%012.3fms : p%d enters Q2\n",(t5-t)*1000,first->packet_no);
pthread_mutex_unlock(&mutex1);
        }
      
    } //end of else     
    }//end of for loop
pthread_exit(&arrival);
}//end of arrival_thread

void *token_gen()
{
int tsleep;//=(1000000.0/r);
double last_token=t;
    while(i <= num||(My402ListLength(&q1)!=0))
    {double R= (1000.0)/r;
		if( R > 10000 )
		{	
			R = 10000.0;
		}
        gettimeofday(&time1,0);
        t6= (time1.tv_sec)+ (time1.tv_usec/1000000.0);
        
        tsleep=(int)((R*1000)-(t6-last_token));
        if(tsleep<1000)
        { long slp;struct timespec wait1;
         slp=(long) tsleep;
         wait1.tv_sec=slp/1000000;
         wait1.tv_nsec= (slp%1000000)*1000;
         nanosleep(&wait1,NULL);}
    else
        usleep(tsleep);
        gettimeofday(&time1,0);
        t7= (time1.tv_sec)+ (time1.tv_usec/1000000.0);
        tot_token++;
        if(token>=B)
        {tok_dropped=tok_dropped+1;
            printf("%012.3fms : token t%d arrives, dropped\n",(t7-t)*1000,tot_token);}
        else{
    pthread_mutex_lock(&mutex1);    
        token++;
    pthread_mutex_unlock(&mutex1);
        printf("%012.3fms : token t%d arrives, token bucket has now %d tokens\n",(t7-t)*1000,tot_token,token);
        }
        
        last_token=t7;
        
        while(My402ListLength(&q1)!=0)//checking if q1 is not empty
        {
            My402ListElem *elem=My402ListFirst(&q1);My402ListElem *elem1;
pkt_struct *first1;
            pkt_struct *first=elem->obj;
    if((first->tok)< (token+1))
            {
            gettimeofday(&time1,0);
             t8= (time1.tv_sec)+ (time1.tv_usec/1000000.0);
            if(t8-last_token>=(1000000.0/r))
            { tot_token++;
        if(token>=B){
	    tok_dropped=tok_dropped+1;
            printf("%012.3fms : token t%d arrives, dropped\n",(t8-t)*1000,tot_token);}
        else{
    pthread_mutex_lock(&mutex1);
        token++;
     pthread_mutex_unlock(&mutex1);
         printf("%012.3fms : token t%d arrives, token bucket has now %d tokens\n",(t8-t)*1000,tot_token,token);
            } 
            }//end of token generation inside 
       elem1=My402ListFirst(&q1);
        first1=elem1->obj;        
        if(first==first1)
        { 
     pthread_mutex_lock(&mutex1);
            token-=first->tok;
           My402ListUnlink(&q1, elem1);
           gettimeofday(&time1,0);
           t9= (time1.tv_sec)+ (time1.tv_usec/1000000.0);      
           first->q1_exit=t9;
           printf("%012.3fms : p%d leaves Q1,time in Q1 = %.3fms, token bucket has now %d tokens\n",(t9-t)*1000,first->packet_no,(t9-first->q1_entry)*1000,token);
                time_spent_q1+=(t9-first->q1_entry);
                (void)My402ListAppend(&q2, first1);
            pthread_cond_broadcast(&cv);// check it out-----------------------------------------------------------!
    
            gettimeofday(&time1,0);
           t10= (time1.tv_sec)+ (time1.tv_usec/1000000.0);
           first->q2_entry=t10; 
           printf("%012.3fms : p%d enters Q2\n",(t10-t)*1000,first->packet_no);
 pthread_mutex_unlock(&mutex1);
        }
        else 
        {break;} 
        }   
    else break;
        }//end of transfer while      
  }//end of main while
   pthread_mutex_lock(&mutex1);
    complete=1;
    pthread_cond_broadcast(&cv);
  pthread_mutex_unlock(&mutex1);   
pthread_exit(&token_thrd);
}


void *server_1()
{
    while(complete!=1||stop_flag!=1){
pthread_mutex_lock(&mutex1);
    while(complete==0&&My402ListLength(&q2)==0&&stop_flag==0)
    {
    pthread_cond_wait(&cv,&mutex1); 
    }
        if((complete==1&&My402ListLength(&q2)==0)||stop_flag==1)
    {   pthread_cond_broadcast(&cv);
     pthread_mutex_unlock(&mutex1);
        pthread_exit(&server1);
    }
        if((My402ListLength(&q2)!=0||complete==0)||stop_flag!=1)
        {int ssleep;
       My402ListElem *elem1=My402ListFirst(&q2);
            pkt_struct *first=elem1->obj;
            My402ListUnlink(&q2, elem1);

            gettimeofday(&time1,0);
           s1= (time1.tv_sec)+ (time1.tv_usec/1000000.0);      
           first->q2_exit=s1;
           printf("%012.3fms : p%d leaves Q2,time in Q2 = %.3fms\n",(s1-t)*1000,first->packet_no,(s1-(first->q2_entry))*1000);
            time_spent_q2+=(s1-first->q2_entry);
            
            gettimeofday(&time1,0);
            s2= (time1.tv_sec)+ (time1.tv_usec/1000000.0); 
            first->server_entry=s2;
pthread_mutex_unlock(&mutex1);
           printf("%012.3fms : p%d begins service at S1, requesting %.3fms of service\n",(s2-t)*1000,first->packet_no,(first->service)); 
         serviced++;
    
           ssleep=first->service;
         if(ssleep<1)
         { long slp;
          struct timespec wait1;
         slp=(long) (ssleep*1000);
         wait1.tv_sec=slp/1000000;
         wait1.tv_nsec= (slp%1000000)*1000;
         nanosleep(&wait1,NULL);}
         
         else usleep(((ssleep*2.0+1)/2)*1000);
         gettimeofday(&time1,0);
        s3= (time1.tv_sec)+ (time1.tv_usec/1000000.0);
         first->server_exit=s3;
          sys_spent+=(s3-first->q1_entry);
         sq+=((s3-first->q1_entry)*(s3-first->q1_entry));
        printf("%012.3fms : p%d departs from S1, service time = %.3fms, time in system = %.3fms\n",(s3-t)*1000,first->packet_no,(s3-s2)*1000,(s3-first->q1_entry)*1000);  
         time_spent_s1+=(s3-first->server_entry);
       }

    if(serviced>=(num-pkt_drop-removed))
    {stop_flag=1;
pthread_mutex_lock(&mutex1);
pthread_cond_broadcast(&cv);
     
pthread_mutex_unlock(&mutex1);
pthread_exit(&server1);}
    }
    pthread_exit(&server1);
}

void *server_2()
{
   while(complete!=1||stop_flag!=1){
pthread_mutex_lock(&mutex1);
    while(complete==0&&My402ListLength(&q2)==0&&stop_flag==0)
    {
    pthread_cond_wait(&cv,&mutex1); }
    if((complete==1&&My402ListLength(&q2)==0)||stop_flag==1)
    {  pthread_cond_broadcast(&cv);
     pthread_mutex_unlock(&mutex1);
        pthread_exit(&server2);
    }
        if((My402ListLength(&q2)!=0||complete==0)||stop_flag!=1)
        {int ssleep;
       My402ListElem *elem1=My402ListFirst(&q2);
            pkt_struct *first=elem1->obj;
            My402ListUnlink(&q2, elem1);
            gettimeofday(&time1,0);
           s4= (time1.tv_sec)+ (time1.tv_usec/1000000.0);      
           first->q2_exit=s4;
           printf("%012.3fms : p%d leaves Q2,time in Q2 = %.3fms\n",(s4-t)*1000,first->packet_no,(s4-(first->q2_entry))*1000);
            time_spent_q2+=(s4-first->q2_entry);
           // first->service;
            gettimeofday(&time1,0);
            s5= (time1.tv_sec)+ (time1.tv_usec/1000000.0); 
            first->server_entry=s5;
           printf("%012.3fms : p%d begins service at S2, requesting %.3fms of service\n",(s5-t)*1000,first->packet_no,(first->service)); 
         serviced++;
         pthread_mutex_unlock(&mutex1);
           ssleep=first->service;
        if(ssleep<1){ long slp;
          struct timespec wait1;
         slp=(long) (ssleep*1000);
         wait1.tv_sec=slp/1000000;
         wait1.tv_nsec= (slp%1000000)*1000;
         nanosleep(&wait1,NULL);}
        else usleep(((ssleep*2.0+1)/2)*1000);
         gettimeofday(&time1,0);
        s6= (time1.tv_sec)+ (time1.tv_usec/1000000.0);
         first->server_exit=s6;
         sys_spent+=(s6-first->q1_entry);
          sq+=((s6-first->q1_entry)*(s6-first->q1_entry));
        printf("%012.3fms : p%d departs from S2, service time = %.3fms, time in system = %.3fms\n",(s6-t)*1000,first->packet_no,(s6-s5)*1000,(s6-first->q1_entry)*1000);  

         time_spent_s2+=s6-first->server_entry;
      }
       
     if((serviced>=(num-pkt_drop-removed))||complete==1)
     {stop_flag=1;
pthread_mutex_lock(&mutex1);
pthread_cond_broadcast(&cv);
pthread_mutex_unlock(&mutex1);
pthread_exit(&server2);}
    }
    pthread_exit(&server2);
}

void statistics()
{
    double q1pkt,q2pkt,s1pkt,s2pkt; double avg_ser,vari;double vari1;
gettimeofday(&time1,0);
    tend=time1.tv_sec+ (time1.tv_usec/1000000.0);
    printf("%012.3fms : emulation ends\n",(tend-t)*1000);
    tend=tend-t;
    printf("\nStatistics\n\n");
	 printf("Created Packets: %d\n",j);
printf("Dropped Packets: %d\n",(pkt_drop));
if(stop_flag==1)printf("Removed Packets: %d\n",(removed));

    printf("\naverage packet inter-arrival time = %.6gs\n",avg_inter/1000);
    if(serviced>0)
    printf("average packet service time = %.8gs\n",(time_spent_s1+time_spent_s2)/serviced);
    else
    printf("average packet service time = 0s\n"); 
    q1pkt=(time_spent_q1/tend);
    printf("\naverage number of packets in Q1 = %.6g\n",q1pkt);
    q2pkt=(time_spent_q2/tend);
    printf("average number of packets in Q2 = %.6g\n",q2pkt);
    s1pkt=(time_spent_s1/tend);
    printf("average number of packets in S1 = %.6g\n",s1pkt);
    s2pkt=(time_spent_s2/tend);
    printf("average number of packets in S2 = %6g\n",s2pkt);
   if(serviced>0) 
       avg_ser=(sys_spent/(serviced));
    else
        avg_ser=0;
    printf("\naverage a packet spent in system = %.6gs\n",avg_ser);
    if(serviced>0)
    {vari1=(sq/(serviced))-((sys_spent/(serviced))*(sys_spent/(serviced)));
    vari=sqrt(vari1);}
    else
        vari=0;
    printf("standard deviation for time spent in system = %.6g\n",vari);

    printf("\ntoken drop probability = %.6g\n",(tok_dropped*1.0)/tot_token);
    printf("packet drop probability = %.6g\n",(pkt_drop*1.0)/(num-removed));

}

void *monitor()
{

 while (1) {
sigwait(&set);
 pthread_mutex_lock(&mutex1);
     pthread_cancel(arrival);
     pthread_cancel(token_thrd);
     stop_flag=1;
     clean();
     pthread_cond_broadcast(&cv);
pthread_mutex_unlock(&mutex1);
 
  pthread_exit(&cntrlthrd);
 }
 
}

void clean()
{
while(My402ListLength(&q1)!=0)
{ My402ListElem *elem1=My402ListFirst(&q1);
 pkt_struct *first=elem1->obj;
gettimeofday(&time1,0);
end1= (time1.tv_sec)+ (time1.tv_usec/1000000.0); 
printf("%012.3fms : p%d removed from Q1\n",(end1-t)*1000,first->packet_no); 
 My402ListUnlink(&q1, elem1);
 removed++;    
}
    while(My402ListLength(&q2)!=0)
{ My402ListElem *elem1=My402ListFirst(&q2);
 pkt_struct *first=elem1->obj;
gettimeofday(&time1,0);
end2= (time1.tv_sec)+ (time1.tv_usec/1000000.0); 
printf("%012.3fms : p%d removed from Q2\n",(end2-t)*1000,first->packet_no); 
 My402ListUnlink(&q2, elem1);   
}
}

