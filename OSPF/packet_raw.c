#include<unistd.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netinet/tcp.h>
#include<netinet/ip.h>
#include<netpacket/packet.h>
#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<getopt.h>
#include<string.h>
#include<arpa/inet.h>
#include<stdio.h>
#include<arpa/inet.h>
#include<malloc.h>
#include<net/ethernet.h>
#include<sys/ioctl.h>
#include<net/if.h>
#include<sys/types.h>
#include<time.h>


#define MAX 300
#define ETH_P_ALL 0x0003
#define ETHSIZE 14
#define IPHSIZE 20
#define TCPHSIZE 20

typedef unsigned char BYTE;

enum _Boolean_ {FALSE=0,TRUE=1};

int create_socket(char*device)
{
	int packet_socket;
	struct ifreq ifr;
	memset(&ifr, 0, sizeof(ifr));
	struct sockaddr_ll rawsll;
	memset(&rawsll, 0, sizeof(rawsll));
	packet_socket=socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
	if(packet_socket==0)
	{
		printf("ERR:socket creation for device:%s\n",device);
		return FALSE;
	}


	strncpy(ifr.ifr_name, device, sizeof(ifr.ifr_name));
	if(ioctl(packet_socket, SIOCGIFINDEX,&ifr)==­1)
		{
		printf("ERR:ioctl failed for device:%s\n",device);
		return FALSE;
		}
		rawsll.sll_family =AF_PACKET;
		rawsll.sll_ifindex =ifr.ifr_ifindex;
		rawsll.sll_protocol =htons(ETH_P_ALL);

		if(bind(packet_socket, (struct sockaddr *) &rawsll, sizeof(sll))==­1)
		{
		printf("ERR:bind failed for device:%s\n",device);
		return FALSE;
		}
    return packet_socket;
}


 struct packet
	{
		struct iphdr ip;
	};




unsigned short in_cksum(unsigned short *addr, int len)
{
register int sum=0;
u_short answer=0;
register u_short *w=addr;
register int nleft=len;
while (nleft>1) {
		sum+=*w++;
		nleft­=2;
		}
	if(nleft==1){
		*(u_char*)(&answer)=*(u_char*)w;
		sum+=answer;
		}
	sum=(sum>>16)+(sum&0xffff);
	sum+=(sum>>16);
	answer=~sum;
	return(answer);
}

void main(int argc, char*argv[])
{

BYTE ospf[24]={	0x02,\
		0x01,\
		0x00,\
		0x2c,\
		0xc0,\
		0xa8,\
		0x67,\
		0x01,\
		0x00,\
		0x00,\
		0x00,\
		0x01,\
		0xb9,\
		0xf8,\
		0x00,\
		0x01,\
		0x63,\
		0x69,\
		0x73,\
		0x63,\
		0x6f,\
		0x00,\
		0xb9,\
		0x00
};


BYTE l2[14]={	0x01,\
		0x00,\
		0x5e,\
		0x00,\
		0x00,\
		0x05,\
		0xc0,\
		0x01,\
		0x0f,\
		0x78,\
		0x00,\
		0x00,\
		0x08,\
		0x00
};

int packet_socket;
struct packet p;
memset(&p,0x0,sizeof(struct packet));
p.ip.version =4;
p.ip.ihl =IPHSIZE>>2;
p.ip.tos =0;
p.ip.tot_len =htons(24+IPHSIZE);
p.ip.id =htons(rand()%65535);
p.ip.frag_off =0;

p.ip.version =4;
p.ip.ihl =IPHSIZE>>2;
p.ip.tos =0;
p.ip.tot_len =htons(24+IPHSIZE);
p.ip.id =htons(rand()%65535);
p.ip.frag_off =0;

p.ip.ttl =htons(254);
p.ip.protocol =0x59;
p.ip.saddr =inet_addr(argv[2]);
p.ip.daddr =inet_addr("0xE0000005");
p.ip.check =(unsigned short) in_cksum((unsigned short*)&p.ip, IPHSIZE);
BYTE buf[MAX];
memcpy(buf, l2, ETHSIZE);
memcpy(buf + ETHSIZE, &p.ip, IPHSIZE);
memcpy(buf + ETHSIZE + IPHSIZE, ospf, 24);
int sock_fd = create_socket(argv[1]);
if((!packet_socket))
	{
	 printf("no created socket!\n");
	 return;
	}
write(packet_socket, (BYTE *)buf, ETHSIZE+ntohs(p.ip.tot_len));
printf("----packet sent-----\n");
}

