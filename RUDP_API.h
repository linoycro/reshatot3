#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <netinet/udp.h>
#include <stdbool.h>
#include <sys/time.h>
#include <time.h>
#include <errno.h>


#define TIMES_TO_SEND 100
#define BITS_TO_BYTES 8

#define TIMEOUT_MICROSECS 700000
#define BUFFER_SIZE 65000
#define SERVER_IP_ADDRESS "127.0.0.1"

typedef struct _RUDP_Header
{
    unsigned short len : BITS_TO_BYTES + BITS_TO_BYTES;
    unsigned short checksum : BITS_TO_BYTES + BITS_TO_BYTES;
    unsigned char ack : BITS_TO_BYTES;
    unsigned char fin : BITS_TO_BYTES;
    unsigned char syn : BITS_TO_BYTES;
    unsigned int seq;
} RUDP_Header;

typedef struct _RUDP_Packet
{
    // Header for RUDP
    RUDP_Header *header;

    // Message to deliver
    char mes[BUFFER_SIZE];

} RUDP_Packet;

// [[ RUDP HRADER ] [ PAYLOAD ]]



// A struct that represents RUDP Socket
typedef struct _rudp_socket
{
    int socket_fd;                // UDP socket file descriptor
    bool isServer;                // True if the RUDP socket acts like a server, false for client.
    bool isConnected;             // True if there is an active connection, false otherwise.
    struct sockaddr_in dest_addr; // Destination address. Client fills it when it connects via rudp_connect(), server fills it when it accepts a connection via rudp_accept().
} RUDP_Socket;

// A struct that represents a packet
//RUDP_Packet *create_Packet(void);

// Allocates a new structure for the RUDP socket (contains basic information about the socket itself). Also creates a UDP socket as a baseline for the RUDP.
// isServer means that this socket acts like a server. If set to server socket, it also binds the socket to a specific port.
int rudp_socket();

// Tries to connect to the other side via RUDP to given IP and port. Returns 0 on failure and 1 on success. Fails if called when the socket is connected/set to server.
//int rudp_connect(RUDP_Socket *sockfd, const char *dest_ip, unsigned short int dest_port);

// Accepts incoming connection request and completes the handshake, returns 0 on failure and 1 on success. Fails if called when the socket is connected/set to client.
//int rudp_accept(RUDP_Socket *sockfd);

// Receives data from the other side and put it into the buffer. Returns the number of received bytes on success, 0 if got FIN packet (disconnect), and -1 on error.
// Fails if called when the socket is disconnected.
int rudp_recv(int sockfd, void *buf, size_t len, int flags, int port);

// Sends data stores in buffer to the other side. Returns the number of sent bytes on success, 0 if got FIN packet (disconnect), and -1 on error. Fails if called when the socket is disconnected.
int rudp_send(int sockfd, const void *buf, size_t len, int flags, const char *ip, int port);
// Disconnects from an actively connected socket. Returns 1 on success, 0 when the socket is already disconnected (failure).
//int rudp_disconnect(RUDP_Socket *sockfd);

// This function releases all the memory allocation and resources of the socket.
int rudp_close(int sockfd);

//this function calculates the checksum
unsigned short int calculate_checksum(void *data, unsigned int bytes);

// our functions//

// This function Sends fyn 
//int send_fin(RUDP_Socket *sockfd);

// This function Sends ack
//int send_ack(RUDP_Socket *sockfd, unsigned int seq);


//this function creates a packet
//RUDP_Packet *create_Packet(void);

//this function sets the packet values
//void set_Packet(RUDP_Packet *packet, char ack, char fin, char syn, unsigned int seq, char *mes);

//this function free the packet
//void free_packet(RUDP_Packet *p);
