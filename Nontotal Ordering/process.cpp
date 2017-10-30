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
    for(int z=0;z<3;z++)
    {
        portarray[z] = atoi(argv[z+2]);
    }
    flag1 = atoi(argv[5]);
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
    
    printf("\nReceiver thread\n");
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
          printf("\nBefore listen" );
     int lis = listen(sockfd,5);
     printf("\nLis:%d",lis);
     printf("\nAfter listen");
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
    
     //printf("\nHere is the message: %s\n",buffer);

 printf("\n\nMessage: %s sent to application\n\n",buffer);
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
    printf("\nSender thread");   
    int sockfd,n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    //char buffer[256];

    while(1)
    {  
    //sleep(1);  
    //printf("\nPlease enter the message: ");
    //bzero(buffer,6);
    //fgets(buffer,6,stdin);
        server_ts++;
    bzero(buffer,256);
    sprintf(buffer,"%d",server_ts);

    //portno = 9799;
    for(int x = 0;x<3;x++)
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
 printf("\nMessage:%d multicasted to processes",server_ts);
 sleep(1);
 if(server_ts == 500)
 {
    printf("500 Messages tranasferred");
    break;
 }
}
    
    return 0;
}