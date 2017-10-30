#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <time.h>
#include <errno.h>

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

struct message{
    int sent_flag;
    char msg[6];
};

struct message ms[100000000];
static int cnt = 0;
int portarray[4];
void *processrequests(void *);
void *threadprocess(void *);
pthread_mutex_t mutex;
char buffer[256];
char msgbuffer[256];
pthread_t tid[2];
static int clientcount=0,numrequests=0,servertime=rand() % 25,flag=0;
static int ts=rand() % 10;
int portno;
int portno1;
int portno2;
static int flag1;
socklen_t clilen;     
struct sockaddr_in serv_addr, cli_addr;
struct hostent *server;
static int local_ts = 0;
static int server_ts = 0;
 
int main(int argc, char *argv[])
{
     
    
    FILE *output;
    pthread_t pidsend,pidrecv;
    
    // argument count check
    /*if (argc < 2) {
       printf("Error: Less no. of arguments");
       exit(1);
    }*/

    portno = atoi(argv[1]);
    //portno1 = atoi(argv[2]);
    //portno2 = atoi(argv[3]);
    for(int z=0;z<4;z++)
    {
        portarray[z] = atoi(argv[z+2]);
    }
    flag1 = atoi(argv[6]);
    /*printf("Port no. is:%d",portno);
    numrequests= atoi(argv[1]);
    server = gethostbyname(argv[2]);*/
    //server = "127.0.0.1";
    if(flag1 == 1)
    {
    pthread_create(&pidsend,NULL,processrequests,NULL);
    pthread_create(&pidrecv,NULL,threadprocess,NULL);
    pthread_join(pidsend,NULL);
    pthread_join(pidrecv,NULL);
    }
    else if(flag1 == 2)
    {
        //printf("\nStarting listener thread");
        pthread_create(&pidsend,NULL,processrequests,NULL);
        pthread_join(pidsend,NULL);
    }
        
}

void *processrequests(void *args){
    //Server first creates socket
    
    printf("\nInside receiver thread\n");
    int sockfd, newsockfd;
     socklen_t clilen;
     //char buffer[256];
     struct sockaddr_in serv_addr, cli_addr;
     int n;
     
     //portno = 9799;
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0) 
        error("ERROR opening socket");
     bzero((char *) &serv_addr, sizeof(serv_addr));
     //portno = atoi(argv[1]);
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);
     printf("Port no. is:%d\n",portno);
     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0) 
              error("ERROR on binding");
          //printf("\nBefore listen" );
     int lis = listen(sockfd,5);
     //printf("\nLis:%d",lis);
     //printf("\nAfter listen");
     clilen = sizeof(cli_addr);
     while(1)
     {
     newsockfd = accept(sockfd, 
                 (struct sockaddr *) &cli_addr, 
                 &clilen);

     if (newsockfd < 0) 
          error("ERROR on accept");
     //bzero(buffer,256);

        bzero(buffer,256);
     n = read(newsockfd,buffer,256);
     if (n < 0) error("ERROR reading from socket");

     int q = 0;
     int seq_msg = 0;
     int seq_ts = 0;
     //printf("\nMessage received from process");
     while(buffer[q] != '\0')
     {
        //printf("\nChecking if message is from sequencer");
        if(buffer[q] == ',')
        {
            seq_msg = 1;
            //printf("\nMessage from sequencer, seq_msg:%d",seq_msg);
        }
        q++;
     }
     if(seq_msg == 0)
     {
        printf("\nMessage received is %s",buffer);
        cnt++;
        strcpy(ms[cnt-1].msg,buffer);
        ms[cnt-1].sent_flag = 0;
    }
     //printf("\nHere is the message: %s\n",buffer);
    if(seq_msg == 1)
    {
        //printf("\nMessage received from sequencer, checking in structure");
        int b = 0;
        char buffer_seq[256];
        char buff_ts[256];
        bzero(buffer_seq,256);
        while(buffer[b] != ',')
        {
            buffer_seq[b] = buffer[b];
            //printf("\nbuffer_seq[b]:%c",buffer_seq[b]);
            //printf("\nbuffer[b]:%c",buffer[b]);
            b++;
        }
        //buffer_seq[b] = '\0';
        b++;
        int c = 0;
        while(buffer[b] != '\0')
        {
                buff_ts[c] = buffer[b];
                b++;
                c++;
        }
        buff_ts[c] = '\0';
        printf("\nMessage from sequencer is %s",buffer);
        //printf("\ntimestamp from sequencer is %s",buff_ts);
        seq_ts = atoi(buff_ts);
        //printf("\ntimestamp from sequencer:%d",seq_ts); 
     for(int p = 0;p<cnt;p++)
     {
        int found = 1;
        //printf("\nRunning loop in structure to match the message");
        //printf("\nms[p].msg:%s",ms[p].msg);
        //printf("\nbuffer_seq:%s",buffer_seq);
        //printf("\nms[p].sent_flag:%d",ms[p].sent_flag);
        int r = 0;
        while(ms[p].msg[r] != '\0' && buffer_seq[r] != '\0')
        {
            if(ms[p].msg[r] != buffer_seq[r])
            {
                found = 0;
            }
            r++;
        }

        int len5 = strlen(ms[p].msg);
        int len6 = strlen(buffer_seq);

        //printf("\nlen5:%d and len6:%d",len5,len6);
        if(found == 1 && ms[p].sent_flag == 0 && (local_ts+1) == seq_ts)
        {
            //printf("\nFound");
            printf("\n\nMessage %s sent to application\n\n",buffer_seq);
            local_ts++;
            ms[p].sent_flag = 1;

        }
     }
 }
     }
     close(newsockfd);
     close(sockfd);
    return 0; 
}


void *threadprocess(void *args)
{
    //int nnsocket1 = *(int*)sockfd1;
    //struct hostent *server;
    sleep(3);
    printf("\nInside sender thread");   
    int sockfd,n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    //char buffer[256];

    while(1)
    {  
    usleep(0.9);  
    //printf("\nPlease enter the message: ");
    //bzero(buffer,6);
    //fgets(buffer,6,stdin);
        server_ts++;
    bzero(buffer,256);
    sprintf(buffer,"%d",server_ts);

    //portno = 9799;
    for(int x = 0;x<4;x++)
    {
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");
    server = gethostbyname("127.0.0.1");
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portarray[x]);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");
    
    n = write(sockfd,buffer,strlen(buffer));
    if (n < 0) 
         error("ERROR writing to socket");
     
     close(sockfd);
 }
 printf("\nMessage:%d multicasted to processes and sequencer",server_ts);
 if(server_ts == 250)
 {
    printf("\n250 messages are multicasted");
    break;
 }
}
    return 0;
}