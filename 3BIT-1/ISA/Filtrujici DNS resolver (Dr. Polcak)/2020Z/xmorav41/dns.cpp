#include <iostream>
#include <arpa/inet.h>
#include <string.h>
#include <netinet/in.h> 
#include <cstdlib> 
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <vector>
#include <fstream>
#include <iostream>
#include <unistd.h>  
#include <algorithm> 

using namespace std;

#define v(f) if(f)

#define BUFFER	(512)   
#define MAX (2048)
#define DNSPORT 53


typedef struct {
    unsigned short id;       
    unsigned char rd :1;     
    unsigned char tc :1;     
    unsigned char aa :1;     
    unsigned char opcode :4; // purpose of message
    unsigned char qr :1;     // query/response flag
    unsigned char rcode :4;  
    unsigned char cd :1;    
    unsigned char ad :1;     
    unsigned char z :1;      
    unsigned char ra :1;     
    unsigned short q_count;  
    unsigned short ans_count; 
    unsigned short auth_count; 
    unsigned short add_count; 
} dns_header;

struct  Args {
    char * server;
    int port = 53;
    char *port_c;
    char *handle;

    bool sFlag = false, pFlag = false, fFlag = false, v = false;

} Arguments;

Args getArguments(int argc, char* argv[]){

    if (argc == 1 || (argc == 2 && ( strcmp(argv[1],"-h") || strcmp(argv[1],"--help"))) ){
        printf("### DNS RESOLVER \n");
        printf("### ISA \n");
        printf("### Author: Tomáš Moravčík xmorav41\n");
        printf("##  Program filtring A type queries and forwarding them to specified resolver.\n    Answer is sent back to the sender. \n");
        printf("# Usage: \n");
        printf("# ---------- \n");
        printf("# Example: ./dns -s ::fffff:8.8.8.8 -p 5454 -f filter.txt -v \n");
        printf("# ---------- \n");
        printf("#   -s [DNS SERVER]     req\n");
        printf("#   -p [LOCALHOST PORT] opt (default 53)\n");
        printf("#   -f [FILTER FILE]    req\n");
        printf("#   -v verbose          opt\n\n");
        exit(0);
    }

    if ( (argc > 8 && argc < 5) ){
        std::cerr << "Error: Incorrect argument count" << '\n';
        exit(-1);        
    }

    for (int i = 1; i < argc; ++i){
        std::string string = argv[i];
        if (0 == string.find("-s")){    //server
           if (!(strcmp(argv[i+1],"-p")) || !(strcmp(argv[i+1],"-f")) || !(strcmp(argv[i+1],"-v")) ){
               std::cerr << "Error: Missing -s argument" << '\n';
               exit(-1);
           }          
            i++;
            Arguments.server = argv[i];
            Arguments.sFlag = true;
        }
        else if (0 == string.find("-p")){   //port
            try{
                Arguments.port = std::stoi(argv[i+1]);   
            }
            catch(std::invalid_argument& e){
                std::cerr << "Error: Wrong port argument" << '\n';
                exit(-1);
            }
            catch(std::out_of_range& e){
                std::cerr << "Error: Int out of bound (port)" << '\n';
                exit(-1);
            }
            catch(...){
                std::cerr << "Error: Wrong port argument" << '\n';
                exit(-1);
            }
            if (Arguments.port < 0 || Arguments.port > 65535){
                std::cerr << "Error: Wrong port range " << Arguments.port << '\n';
                exit(-1);
            }
            i++;
            Arguments.port = std::stoi(argv[i]);
            Arguments.port_c = (argv[i]);
            Arguments.pFlag = true;
        }        
        else if (0 == string.find("-f")){   // file
            if (!(strcmp(argv[i+1],"-p")) || !(strcmp(argv[i+1],"-s")) || !(strcmp(argv[i+1],"-v"))){
               std::cerr << "Error: Missing -f argument" << '\n';
               exit(-1);
           }
            i++;
            Arguments.handle = argv[i];
            Arguments.fFlag = true;
        }
        else if (0 == string.find("-v")){   // verbose
            i++;
            Arguments.v = true;
        }
        else{
            std::cerr << "Error: Argument error" << '\n';
        }
    }
    if ((!Arguments.sFlag) || (!Arguments.fFlag)){
        std::cerr << "Error: Missing arguments -s || -f" << '\n';
            exit(-1);
    }
    return Arguments;
}


void err(int i, const char*mssg);

vector<string> parse_filter(char *ffile);



int main(int argc, char* argv[]){
    
    // Parse arguments
    struct Args args;
    args = getArguments(argc, argv);

    // Get DNS server informations
    struct addrinfo hints, *result;
    memset(&hints, 0,sizeof(hints));
    if((getaddrinfo(args.server, "53", &hints, &result)) != 0) err(-1,"Couldnt validate server\n");

    // Set DNS socket
    struct sockaddr_in6 address;
    memset (&address, 0, sizeof(struct sockaddr_in6));
    inet_pton(AF_INET6, args.server, &address.sin6_addr);
    address.sin6_family = AF_INET6;
    address.sin6_port = htons (DNSPORT);
    
    int dnsSocket;
    if ((dnsSocket = socket(AF_INET6, SOCK_DGRAM, 0)) == -1) printf("Cannot create server socket!\n");
    int mode = 0;
    setsockopt(dnsSocket, IPPROTO_IPV6, IPV6_V6ONLY, (char*)&mode, sizeof(mode));   // Accept both ipv4 and ipv6


    // Parse filter data
    vector<string> data = parse_filter(args.handle);
    std::cout << data.max_size();
 
    // Set local socket
    struct sockaddr_in6 loc_socket6;
    memset (&loc_socket6, 0, sizeof(struct sockaddr_in6));
    socklen_t l_length;
    l_length = sizeof loc_socket6;
    loc_socket6.sin6_addr = in6addr_any;
    loc_socket6.sin6_family = AF_INET6;
    loc_socket6.sin6_port = htons (args.port);
    
    int s;
    v(args.v) printf("\n# Opening a local UDP socket\n");
    if ((s = socket(AF_INET6, SOCK_DGRAM, 0)) == -1) printf("Cannot create server socket!\n");
    setsockopt(s, IPPROTO_IPV6, IPV6_V6ONLY, (char*)&mode, sizeof(mode));
    v(args.v) printf("# Binding to the port %d (%d)\n\n",5454, loc_socket6.sin6_port);
    if (bind(s, (struct sockaddr *)&loc_socket6, sizeof(loc_socket6)) == -1) err(1, "bind() failed");


    // Set request buffer
    unsigned char buffer[BUFFER];
    memset(buffer,0,BUFFER);

    // Timeout
    timeval timeout;
    timeout.tv_sec = 5;
    setsockopt(dnsSocket, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout));

    // Listen 
    while(true){

        v(args.v) printf("\n# Waiting on port %d\n", args.port);
        int recvlen = recvfrom(s, buffer, BUFFER, 0, (struct sockaddr *)&loc_socket6, &l_length);      // get request from client
        v(args.v) printf("# Request received from port %d, bytes %d\n",ntohs(loc_socket6.sin6_port),recvlen);

        // Non-standard query
        dns_header *dns = NULL;
        dns = (dns_header *)&buffer;
        if (dns->qr != 0){
            v(args.v) printf("Not standard query\n");
            continue;
        }

        // Set variables for query check
        unsigned char *qname;
        qname =(unsigned char*)&buffer[sizeof(dns_header)];
        unsigned char *domain = (unsigned char*)calloc(strlen((char*)qname),sizeof(qname));     //init
        int k = strlen((char*)qname);
        memcpy(domain,qname, sizeof(qname)*k);
        unsigned int p=0;
    
        int i;
        for(i = 0; i < (int)strlen((const char*)domain); i++) {
            p = domain[i];
            for(int j = 0; j<(int)p; j++) {
                domain[i] = domain[i+1];
                i = i + 1;
            }
            domain[i] = '.';
        }
        domain[i-1] = '\0'; 

        string req_name;        // Requested 
        req_name = (char *)domain;
        free(domain);


        // "NOT IMPLEMENTED" response
        if (dns->opcode != 0 ){          
            dns->qr  = 1;
            dns->rcode = 4;                     // send unimplemented response and continue
            if(sendto(s,(unsigned char*)buffer,sizeof(buffer),0,(struct sockaddr*)&loc_socket6, l_length) == -1) perror("sendto client failed");
            v(args.v) printf("Done\n");
            continue;
        }

        // "REFUSED" response
        bool blacklist = false;
        for(int i = 0; i < (int)data.size() ; i++ ) {      
            if (req_name.find(data[i]) != string::npos ) {      // Check that domain in vector is requested domain substring
                if (req_name.length() > data[i].length())  {    // Subdomain and domain
                    if (0 == req_name.compare (req_name.length() - data[i].length(), data[i].length(), data[i])     // Next compare vector domain from END
                        && ((req_name[req_name.length() - data[i].length()-1] == '.' ))){  ;     // Make sure that is subdomain and not just 
                    }
                    else break;
                }
                // Else equal strings
                v(args.v) printf("%s blacklisted\n", req_name.c_str());
                blacklist = true;
                dns->qr  = 1;
                dns->rcode = 5;                // send refused response and continue
                if(sendto(s,(unsigned char*)buffer,sizeof(buffer),0,(struct sockaddr*)&loc_socket6, l_length) == -1) perror("sendto client failed");
                v(args.v) printf("Done\n");
                break;
            }
        }
        if (blacklist) continue;
        
        
        // Set response buffer
        unsigned char res_buffer[BUFFER];
        memset(res_buffer,0,BUFFER);

        // Forward query to DNS server and return answer to client
        //if (connect(dnsSocket, result->ai_addr, result->ai_addrlen) == -1) err(1, "Could not connect to server");
        v(args.v) printf("Forwarding Packet...\n");
        if(sendto(dnsSocket,(unsigned char*)buffer,sizeof(buffer),0,result->ai_addr, result->ai_addrlen) == -1) perror("sendto failed");
        v(args.v) printf("Receiving Packet...\n");  
        if ((recvfrom (dnsSocket, res_buffer, BUFFER, 0, result->ai_addr, &result->ai_addrlen)) == -1 ){
            v(args.v)printf("Server timeout\n");
            dns->qr  = 1;
            dns->rcode = 2;                // server failure
            if(sendto(s,(unsigned char*)buffer,sizeof(buffer),0,(struct sockaddr*)&loc_socket6, l_length) == -1) perror("sendto client failed");
            continue;
        } 
        v(args.v) printf("Sending Answer...\n");
        if(sendto(s,(unsigned char*)res_buffer,sizeof(res_buffer),0,(struct sockaddr*)&loc_socket6, l_length) == -1) perror("sendto client failed");
        v(args.v) printf("Done...\n");

    }                  
    return 0;
}


// Error message
void err(int i, const char*mssg){
   std::cerr << i << ": " << mssg << '\n';
   exit(i); 
}

// Fill vector with blacklisted domains
vector<string> parse_filter(char *ffile){
    vector<string> v1;
    fstream newfile;
    newfile.open(ffile, ios::in);
    if (newfile.is_open()){
        string tmp;
        while(getline(newfile, tmp)){
            if (tmp[0] == '#' || tmp=="")   // ignore comment or empty line  
                continue;
            v1.push_back(tmp);
        }
    }
    newfile.close();
    return v1;
}

