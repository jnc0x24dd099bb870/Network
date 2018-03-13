#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<string.h> 
#include<unistd.h> 
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<linux/ip.h>
#include<linux/tcp.h>



#define DATA "nullQWasHere"

#define IPHSIZE	20
#define TCPHSIZE 20
#define PSEUDOTCPHSIZE	12

/* generic calculation of checksum				                                       */
/* !!!Do you ever read the stuff I give ya?!			                                       */		
/* Pseudo header & chksum        				                                       */
/* http://www.tcpipguide.com/free/t_TCPChecksumCalculationandtheTCPPseudoHeader-2.htm		       */

static unsigned short cksum(unsigned short *ptr,int nbytes)
{
  register long sum;
  unsigned short oddbyte;
  register short answer;

  sum=0;
  while(nbytes>1) {
    sum+=*ptr++;
    nbytes-=2;
  }

  if(nbytes==1) {
    oddbyte=0;
    *((unsigned char *)&oddbyte)=*(unsigned char *)ptr;
    sum+=oddbyte;
  }

 sum = (sum>>16)+(sum & 0xffff);
  sum = sum + (sum>>16);
  answer=(short)~sum;

  return(answer);
}

struct pseudo_header {
  uint32_t s_addr;
  uint32_t d_addr;
  uint8_t nil;
  uint8_t IP_protocol;
  uint16_t tot_Len;
};

int main(int argc, char **argv) {
 char c[1000];
  char s_ip[1000];
  FILE *file;
  int sock, bytes, one = 1;
  struct iphdr *ip_h;
  struct tcphdr *tcp_h;

//such horror
char* randomz = system("perl -e \'$,=\".\";print map int rand 256,1..4\'> huehue.txt");
file=fopen("/home/ipy/huehue.txt", "r");
fscanf(file,"%[^\n]", c);

 strcpy(s_ip, c);
 
  char *d_ip = argv[1];
  int d_port = 8667;
  int s_port = 8666;

/*													
* K'chin'! http://packetlife.net/blog/2010/jun/7/understanding-tcp-sequence-acknowledgment-numbers/
*													*/

  uint32_t TCPSeqAck = 294967;


  char *data;

  char p[512];

  struct sockaddr_in addr_in;

  struct pseudo_header pseudo_tcp;

  char *pseudo_p;
  
  if((sock = socket(PF_INET, SOCK_RAW, IPPROTO_RAW)) < 0) 
  {
    perror("Error while creating socket");
    exit(-1);
  }

/*The IPv4 layer generates an IP header when sending a packet   */
/*unless the IP_HDRINCL socket option is enabled on the socket. */
/*When it is enabled, the p must contain an IP header.          */ 
/*For receiving the IP header is always included in the packet  */

  if(setsockopt(sock, IPPROTO_IP, IP_HDRINCL, (char *)&one, sizeof(one)) < 0) 
  {
    perror("Error while setting socket options");
    exit(-1);
  }

  addr_in.sin_family = PF_INET;
  addr_in.sin_port = htons(d_port);
  addr_in.sin_addr.s_addr = inet_addr(argv[1]);

  memset(p, 0, sizeof(p));

  ip_h = (struct iphdr *) p;
  tcp_h = (struct tcphdr *) (p + sizeof(struct iphdr));
  data = (char *) (p + sizeof(struct iphdr) + sizeof(struct tcphdr));
  strcpy(data, DATA);

  //Populate ip_h
  ip_h->ihl=IPHSIZE>>2;
  ip_h->version = 4; 
  ip_h->tos = 0;
  ip_h->tot_len = IPHSIZE + TCPHSIZE+strlen(data);
  ip_h->id = htons(rand()%65535); 
  ip_h->frag_off = 0x00; 
  ip_h->ttl = 0xFF; 
  ip_h->protocol = IPPROTO_TCP; 
  ip_h->check = 0; 
  ip_h->saddr = inet_addr(s_ip); 
  ip_h->daddr = inet_addr(argv[1]); 
  ip_h->check = cksum((unsigned short *) p, ip_h->tot_len); 

  //Populate tcp_h
  tcp_h->source = htons(s_port); 
  tcp_h->dest = htons(d_port); 
  tcp_h->seq = 0x0; 
  tcp_h->ack_seq = 0x0; 
  tcp_h->doff = 5; 


    tcp_h->syn=1;
    tcp_h->rst=0;
    tcp_h->psh=0;
    tcp_h->ack=0;
    tcp_h->urg=0;
    tcp_h->window = htons (5550); /* maximum window size */
    tcp_h->check = 0; 
    tcp_h->urg_ptr = 0;
    tcp_h->fin = 0;


  //checksum for the TCP header
  pseudo_tcp.s_addr = inet_addr(s_ip); 
  pseudo_tcp.d_addr =  inet_addr(d_ip);
  pseudo_tcp.nil = 0; 
  pseudo_tcp.IP_protocol = IPPROTO_TCP; 

  pseudo_tcp.tot_Len=htons(sizeof(struct tcphdr) + strlen(data));
  
  pseudo_p = (void *)malloc((int) (sizeof(struct pseudo_header) + sizeof(struct tcphdr) + strlen(data)));
  memset(pseudo_p, 0, sizeof(struct pseudo_header) + sizeof(struct tcphdr) + strlen(data));

  memcpy(pseudo_p, (void *) &pseudo_tcp, sizeof(struct pseudo_header));

  tcp_h->seq = htonl(TCPSeqAck);
  tcp_h->check = 0;

  memcpy(pseudo_p + sizeof(struct pseudo_header), tcp_h, sizeof(struct tcphdr) + strlen(data));

  tcp_h->check = (cksum((unsigned short *) pseudo_p, (int) (sizeof(struct pseudo_header) +
          sizeof(struct tcphdr) +  strlen(data))));

        //release the packet!!
  if((bytes = sendto(sock, p, ip_h->tot_len, 0, (struct sockaddr *) &addr_in, sizeof(addr_in))) < 0)
   {
      perror("Error on sendto()");
    }
    else {
      printf("Success! Sent %d bytes.\n", bytes);
    }

//...sheesh!

  return 0;
}
