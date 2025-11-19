
/* Paket Sniffer
 * IPK 2. projekt 2019/2020
 * Tomáš Moravčík
 * xmorav41
 *
 *
 * Tento C program je založený na programe Tima Carstensa "sniffer.c" (http://www.tcpdump.org/sniffex.c), vydané nasledovne:
 *
 * sniffer.c
 * Copyright (c) 2002 Tim Carstens
 * 2002-01-07
 * Demonstration of using libpcap
 * timcarst -at- yahoo -dot- com
 *
 * "sniffer.c" is distributed under these terms:
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 4. The name "Tim Carstens" may not be used to endorse or promote
 *    products derived from this software without prior written permission
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 * <end of "sniffer.c" terms>
 *
 * This software, "sniffex.c", is a derivative work of "sniffer.c" and is
 * covered by the following terms:
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Because this is a derivative work, you must comply with the "sniffer.c"
 *    terms reproduced above.
 * 2. Redistributions of source code must retain the Tcpdump Group copyright
 *    notice at the top of this source file, this list of conditions and the
 *    following disclaimer.
 * 3. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 4. The names "tcpdump" or "libpcap" may not be used to endorse or promote
 *    products derived from this software without prior written permission.
 *
 * THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.
 * BECAUSE THE PROGRAM IS LICENSED FREE OF CHARGE, THERE IS NO WARRANTY
 * FOR THE PROGRAM, TO THE EXTENT PERMITTED BY APPLICABLE LAW.  EXCEPT WHEN
 * OTHERWISE STATED IN WRITING THE COPYRIGHT HOLDERS AND/OR OTHER PARTIES
 * PROVIDE THE PROGRAM "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED
 * OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  THE ENTIRE RISK AS
 * TO THE QUALITY AND PERFORMANCE OF THE PROGRAM IS WITH YOU.  SHOULD THE
 * PROGRAM PROVE DEFECTIVE, YOU ASSUME THE COST OF ALL NECESSARY SERVICING,
 * REPAIR OR CORRECTION.
 *
 * IN NO EVENT UNLESS REQUIRED BY APPLICABLE LAW OR AGREED TO IN WRITING
 * WILL ANY COPYRIGHT HOLDER, OR ANY OTHER PARTY WHO MAY MODIFY AND/OR
 * REDISTRIBUTE THE PROGRAM AS PERMITTED ABOVE, BE LIABLE TO YOU FOR DAMAGES,
 * INCLUDING ANY GENERAL, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES ARISING
 * OUT OF THE USE OR INABILITY TO USE THE PROGRAM (INCLUDING BUT NOT LIMITED
 * TO LOSS OF DATA OR DATA BEING RENDERED INACCURATE OR LOSSES SUSTAINED BY
 * YOU OR THIRD PARTIES OR A FAILURE OF THE PROGRAM TO OPERATE WITH ANY OTHER
 * PROGRAMS), EVEN IF SUCH HOLDER OR OTHER PARTY HAS BEEN ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGES.
 * <end of "sniffex.c" terms>
 *
 ****************************************************************************
 * */


#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <sys/time.h>

#include </usr/include/netinet/ip.h>    //ipv4 protocols
#include </usr/include/netinet/ip6.h>   //ipv6 protocols

#include <pcap.h>
#include <netinet/in.h>         //internet protocol family
#include <sys/socket.h>         //main sockets header
#include <arpa/inet.h>          //for inet_ntoa()
#include <net/ethernet.h>       //ethernet fundamental onstants
#include <netinet/if_ether.h>   //ethernet header declarations
#include <netinet/ether.h>      //ethernet header declarations
#include <netinet/udp.h>	    //Provides declarations for udp header
#include <netinet/tcp.h>	    //Provides declarations for tcp header
#include <netinet/ip.h>	        //Provides declarations for ip header

typedef unsigned char u_char;

void print_tcp_packet(const u_char * , int);
void print_udp_packet(const u_char * , int);

struct sockaddr_in source,dest;

////
// IPv6 zdrojové/cieľové adresy
char sourIP6[INET_ADDRSTRLEN];
char destIP6[INET_ADDRSTRLEN];
////
/***************************************************************************************
*    Následovné 3 funkcie (got_packet(), print_payload(), print_hex_ascii_line()) inšpirované/prevzaté
*
*    Title: sniffex.c
*    Author: Tim Carstens
*    Date: 2-5-2020
*    Code version: Version 0.1.1 (2005-07-05)
*    Availability: http://www.tcpdump.org/sniffex.c
*
***************************************************************************************/
void
got_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *packet);

void
print_payload(const u_char *payload, int len);

void
print_hex_ascii_line(const u_char *payload, int len, int offset);
///


void print_hex_ascii_line(const u_char *payload, int len, int offset)
{

    int i;
    int gap;
    const u_char *ch;

    /* offset */
    printf("0x%04x: ", offset);

    /* hex */
    ch = payload;
    for(i = 0; i < len; i++) {
        printf("%02x ", *ch);
        ch++;
        /* print extra space after 8th byte for visual aid */
        if (i == 7)
            printf(" ");
    }
    /* print space to handle line less than 8 bytes */
    if (len < 8)
        printf(" ");

    /* fill hex gap with spaces if not full line */
    if (len < 16) {
        gap = 16 - len;
        for (i = 0; i < gap; i++) {
            printf("   ");
        }
    }
    printf("   ");

    /* ascii (if printable) */
    ch = payload;
    for(i = 0; i < len; i++) {
        if (isprint(*ch))
            printf("%c", *ch);
        else
            printf(".");
        ch++;
    }

    printf("\n");

    return;
}

//print packet payload data (avoid printing binary data)
void print_payload(const u_char *payload, int len)
{

    int len_rem = len;
    int line_width = 16;			/* number of bytes per line */
    int line_len;
    int offset = 0;					/* zero-based offset counter */
    const u_char *ch = payload;

    if (len <= 0)
        return;

    /* data fits on one line */
    if (len <= line_width) {
        print_hex_ascii_line(ch, len, offset);
        return;
    }

    /* data spans multiple lines */
    for ( ;; ) {
        /* compute current line length */
        line_len = line_width % len_rem;
        /* print line */
        print_hex_ascii_line(ch, line_len, offset);
        /* compute total remaining */
        len_rem = len_rem - line_len;
        /* shift pointer to remaining bytes to print */
        ch = ch + line_len;
        /* add offset */
        offset = offset + line_width;
        /* check if we have line width chars or less */
        if (len_rem <= line_width) {
            /* print last line and get out */
            print_hex_ascii_line(ch, len_rem, offset);
            break;
        }
    }
    printf("\n");

    return;
}

void print_tcp_packet(const u_char * Buffer, int Size)
{
    //Odsadenie
    printf("\n");
    //Vytlačí hlavičku + dáta
    print_payload(Buffer, Size);
}

void print_udp_packet(const u_char *Buffer , int Size)
{
    //Odsadenie
    printf( "\n");
    //Vytlačí hlavičku + dáta
    print_payload(Buffer,Size);
}


/*
 * Identifikuj a spracuj paket
 */
void
got_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *packet)
{
    //Zisti a vytlač čas
    struct timeval curTime;
    gettimeofday(&curTime, NULL);
    int milli = curTime.tv_usec / 1000;

    char Tbuffer [80];
    strftime(Tbuffer, 80, "%H:%M:%S", localtime(&curTime.tv_sec));

    printf("%s.%06d ", Tbuffer, milli);
    //



    int size = header->len;
    unsigned short iphdrlen;
    struct iphdr *ip = (struct iphdr *)(packet + sizeof(struct ethhdr));
    iphdrlen = ip->ihl * 4;
    memset(&source, 0, sizeof(source));
    source.sin_addr.s_addr = ip->saddr;
    memset(&dest, 0, sizeof(dest));
    dest.sin_addr.s_addr = ip->daddr;

    //Rozhodni o ipv
    if (ip->version == 4)     //ivp4
    {
        struct sockaddr_in s;
        char hbuf[NI_MAXHOST];
        memset(&s, 0, sizeof(struct sockaddr_in));
        s.sin_family = AF_INET;
        s.sin_addr.s_addr = inet_addr(inet_ntoa(source.sin_addr));
        if (getnameinfo((struct sockaddr*) &s, sizeof(struct sockaddr_in), hbuf, sizeof(hbuf), NULL, 0, NI_NAMEREQD)) {
            printf("%s : ", inet_ntoa(source.sin_addr)); //Vypíše domain name zdroja
        }
        else {
            printf("%s : ", hbuf); //Vypíše ipv4 zdroja
        }

        //Rozhodni protokol
        if (ip->protocol == 6) //tcp
        {
            //hlavička
            struct tcphdr *tcph = (struct tcphdr *)(packet + iphdrlen + sizeof(struct ethhdr));
            //Zdrojový port
            printf("%d > ", ntohs(tcph->source));

            struct sockaddr_in sT;
            char hbuf1[NI_MAXHOST];
            memset(&sT, 0, sizeof(struct sockaddr_in));
            sT.sin_family = AF_INET;
            sT.sin_addr.s_addr = inet_addr(inet_ntoa(dest.sin_addr));
            if (getnameinfo((struct sockaddr*) &sT, sizeof(struct sockaddr_in), hbuf1, sizeof(hbuf1), NULL, 0, NI_NAMEREQD)) {
                printf("%s : ", inet_ntoa(dest.sin_addr)); //Vypíše domain name cieľa
            }
            else {
                printf("%s : ", hbuf1); //Vypíše ipv4 cieľa
            }
            //Cieľový port
            printf("%d", ntohs(tcph->dest));
            //Výpis paketu
            print_tcp_packet(packet,size);
        }
        else if (ip->protocol == 17) //udp
        {
            //Hlavička
            struct udphdr *udph = (struct udphdr *)(packet + iphdrlen + sizeof(struct ethhdr));
            //Zdrojový port
            printf("%d > ", ntohs(udph->source));

            struct sockaddr_in sU;
            char hbuf1[NI_MAXHOST];
            memset(&sU, 0, sizeof(struct sockaddr_in));
            sU.sin_family = AF_INET;
            sU.sin_addr.s_addr = inet_addr(inet_ntoa(dest.sin_addr));
            if (getnameinfo((struct sockaddr*) &sU, sizeof(struct sockaddr_in), hbuf1, sizeof(hbuf1), NULL, 0, NI_NAMEREQD)) {
                printf("%s : ", inet_ntoa(dest.sin_addr)); //Vypíše domain name cieľa
            }
            else {
                printf("%s : ", hbuf1); //Vypíše ipv4 cieľa
            }
            //Cieľový port
            printf("%d", ntohs(udph->dest));
            //Výpis paketu
            print_udp_packet(packet,size);
        }
    }
    else if (ip->version != 6)  //ipv6
    {
        //hlavička
        const struct ip6_hdr *ipv6_header;
        ipv6_header = (struct ip6_hdr*)(packet + size);

        //ipv6 adresa zdroja a cieľu
        inet_ntop(AF_INET6, &(ipv6_header->ip6_src), sourIP6, INET6_ADDRSTRLEN);
        inet_ntop(AF_INET6, &(ipv6_header->ip6_dst), destIP6, INET6_ADDRSTRLEN);

        //nasledujúca hlavička ipv6
        int nextheader = ipv6_header->ip6_nxt;

        size += sizeof(struct ip6_hdr);

        if (nextheader == 6){
            print_tcp_packet(packet,size);
        }
        else if (nextheader == 17){
            print_udp_packet(packet,size);
        }
        else{
            printf(" %s : 0 > %s : 0\n",sourIP6,destIP6);
            printf("Protocol: Unknown %d\n",nextheader);
         }
    }
    else
    {
        printf("Ether Type: Other \n");
        return;
    }
}

//Vypíš nápovedu
void help_me()
{
    printf("\n***********************************\n");
    printf("Sniffer paketov (xmorav41)\n");
    printf("Sieťový analyzátor v C schopný zachytávať a filtrovať pakety na určitom rozhraní\n");
    printf("\nVolanie programu:\n");
    printf("./ipk-sniffer -i rozhraní [-p port] [--tcp|-t] [--udp|-u] [-n num]\n");
    printf("\t./ipk-sniffer (Zobrazíť dostupné zariadenia)\n");
    printf("\t-i eth0 (rozhranie, na kterom se bude poslúchať. Nebude tento parameter uvedený, vypíše sa zoznam aktívnych rozhraní)\n");
    printf("\t-p 23 (bude filtrovať pakety na danom rozhraní podľa portu; nebude tento parameter uvedený, uvažujú sa všetky porty)\n");
    printf("\t-t alebo --tcp (bude zobrazovať len tcp pakety)\n");
    printf("\t-u alebo --udp (bude zobrazovať len udp pakety)\n");
    printf("\tAk nebude -tcp ani -udp špecifikované, uvažujú sa TCP a UDP pakety zároveň\n");
    printf("\t-n 10 (určuje počet paketov, ktoré se majú zobraziť; ak nieje uvedené, uvažujte zobrazenie len 1 paketu)\n");

    exit(0);
}

int main(int argc, char *argv[]) {

    //Argument flags
    int iflag = 0;
    int pflag = 0;
    int tflag = 0;
    int uflag = 0;
    int nflag = 0;
    char *dev = NULL;
    long port = 0;
    int n_packets;

    //Nápoveda
    if (argc == 2 && (strcmp(argv[1],"--help") == 0 || strcmp(argv[1],"-h") == 0)){
        help_me();
    }

    //Parsovanie arg
    for (int i = 1; i < argc; i++) {
        if (argv[i][0] != '-') {
            fprintf(stderr, "Wrong arg %s\n",argv[i]);
            help_me();
        }

        if (strcmp(argv[1],"-i") != 0){
            fprintf(stderr, "First arg must be'-i'\n");
            help_me();
        }

        switch (argv[i][1]) {
            case 'i':
                if (strlen(argv[i]) != 2) {
                    fprintf(stderr, "Wrong arg %s\n", argv[i]);
                    help_me();
                }
                if (iflag != 1 && i == 1) {
                    iflag = 1;
                    if (!argv[i + 1]) {
                        char error[PCAP_ERRBUF_SIZE];
                        pcap_if_t *interfaces, *temp;
                        int i = 0;
                        if (pcap_findalldevs(&interfaces, error) == -1) {
                            printf("chyba v pcap findall devs\n");
                            return -1;
                        }

                        printf("\nDostupné rozhrania:\n");
                        for (temp = interfaces; temp; temp = temp->next) {
                            printf("%d  :  %s\n", i++, temp->name);
                        }
                        exit(0);
                    }
                    dev = argv[i + 1];
                    i++;
                } else {
                    fprintf(stdout, "Devices list: \n");
                }

                break;
            case 'p':
                if (strlen(argv[i]) != 2) {
                    fprintf(stderr, "Wrong arg %s\n", argv[i]);
                    help_me();
                }
                if (argc == 2){
                    fprintf(stderr, "Missing -i\n");
                    help_me();
                }
                if (pflag != 1) {
                    char *ptr;
                    pflag = 1;
                    if (!argv[i + 1]) {
                        fprintf(stderr, "Missing par for %s\n", argv[i]);
                        help_me();
                    }
                    port = strtol(argv[i + 1], &ptr, 10);
                    i++;
                }
                break;
            case 't':
                if (strlen(argv[i]) != 2) {
                    fprintf(stderr, "Wrong arg %s\n", argv[i]);
                    help_me();
                }
                if (argc == 2){
                    fprintf(stderr, "Missing -i\n");
                    help_me();
                }
                if (tflag != 1) {
                    tflag = 1;
                } else {
                    fprintf(stderr, "Duplicate t arg\n");
                    help_me();
                }

                break;
            case 'u':
                if (strlen(argv[i]) != 2) {
                    fprintf(stderr, "Wrong arg %s\n", argv[i]);
                    help_me();
                }
                if (argc == 2){
                    fprintf(stderr, "Missing -i\n");
                    help_me();
                }
                if (uflag != 1) {
                    uflag = 1;
                } else {
                    fprintf(stderr, "Duplicate u arg\n");
                    help_me();
                }
                break;
            case 'n':
                if (strlen(argv[i]) != 2) {
                    fprintf(stderr, "Wrong arg %s\n", argv[i]);
                    help_me();
                }
                if (argc == 2 || argc == 3){
                    fprintf(stderr, "Missing -i\n");
                    help_me();
                }
                if (nflag != 1) {
                    nflag = 1;
                    if (!argv[i + 1]) {
                        fprintf(stderr, "Missing par for %s\n", argv[i]);
                        help_me();
                    }
                    n_packets = strtol(argv[i + 1], NULL, 10);
                    if (n_packets == 0){
                        fprintf(stderr, "Invalid number of packets %s\n",argv[i+1]);
                        exit(-1);
                    }
                    i++;
                } else {
                    fprintf(stderr, "Duplicate n arg\n");
                    help_me();
                }

                break;
            case '-':
                if (strlen(argv[i]) != 5) {
                    fprintf(stderr, "Wrong arg %s\n", argv[i]);
                    exit(1);
                }
                if (argc == 2){
                    fprintf(stderr, "Missing -i\n");
                    help_me();
                }
                if (!strcmp(argv[i],"--tcp")){
                    if (tflag != 1) {
                        tflag = 1;
                    } else {
                        fprintf(stderr, "Duplicate t arg\n");
                        help_me();
                    }
                } else if (strcmp(argv[i],"--udp") == 0) {
                    if (uflag != 1) {
                        uflag = 1;
                    } else {
                        fprintf(stderr, "Duplicate u arg\n");
                        help_me();
                    }
                } else {
                    fprintf(stderr, "Invalid par %s\n",argv[i]);
                    help_me();
                }
                break;

            default:
                fprintf(stderr, "Wrong par %s\n",argv[i]);
                help_me();
        }

    }

    //Výpis dostupných zariadení
    if (iflag == 0 || dev == NULL) {
        char error[PCAP_ERRBUF_SIZE];
        pcap_if_t *interfaces, *temp;
        int i = 0;
        if (pcap_findalldevs(&interfaces, error) == -1) {
            printf("chyba v pcap findall devs\n");
            return -1;
        }

        printf("\nDostupné rozhrania:\n");
        for (temp = interfaces; temp; temp = temp->next) {
            printf("%d  :  %s\n", i++, temp->name);
        }
        exit(0);
    }


    char errbuf[PCAP_ERRBUF_SIZE];        //Chybový buffer
    pcap_t *handle;                       //Paket handle


    struct bpf_program fp;                //Skompilovaný program s filtrom
    bpf_u_int32 mask;                     // subnet mask
    bpf_u_int32 net;                      // ip
    int num_packets;                      // pocet zobrazovanych paketov
    if (nflag == 0) {
        num_packets = 1;
    } else {
        num_packets = n_packets;
    }
    char filter_exp[20] = "";             // filter
    if (tflag == uflag){
        ;
    }
    else if (tflag){
        strcat(filter_exp, "tcp");    //tcp filter
    }
    else if (uflag){
        strcat(filter_exp, "udp");    //udp filter
    }

    if (pflag){
        char strport[10];
        sprintf(strport, " port %ld",port); //port filter
        strcat(filter_exp, strport);
    }

    //Získaj vlastnosti rozhrania
    if (pcap_lookupnet(dev, &net, &mask, errbuf) == -1) {
        fprintf(stderr, "Couldn't get netmask for device %s: %s\n", dev, errbuf);
        net = 0;
        mask = 0;
    }

    //Sniffuj v promiskuitnom móde
    handle = pcap_open_live(dev, 65536, 1, 1000, errbuf);
    if (handle == NULL) {
        fprintf(stderr, "Couldn't open device %s: %s\n", dev, errbuf);
        return (2);
    }

    //Zkompilovanie
    if (pcap_compile(handle, &fp, filter_exp, 0, net) == -1) {
        fprintf(stderr, "Couldn't parse filter %s: %s\n", filter_exp, pcap_geterr(handle));
        return (2);
    }
    //Aplikovanie filtru
    if (pcap_setfilter(handle, &fp) == -1) {
        fprintf(stderr, "Couldn't install filter %s: %s\n", filter_exp, pcap_geterr(handle));
        return (2);
    }

    // callback funkcia
    pcap_loop(handle, num_packets, got_packet, NULL);

    // uvoľnenie
    pcap_freecode(&fp);
    pcap_close(handle);

    printf("\n\n");

    return 0;

}
