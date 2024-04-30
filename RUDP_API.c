#include <stdint.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>

#define MAX_DATA_SIZE 1024

typedef struct {
    uint16_t length;
    uint16_t checksum;
    uint8_t flags;
    char data[MAX_DATA_SIZE];
} RUDP_Packet;

unsigned short int calculate_checksum(void *data, unsigned int bytes) {
    unsigned short int *data_pointer = (unsigned short int *)data;
    unsigned int total_sum = 0;

    // Main summing loop
    while (bytes > 1) {
        total_sum += *data_pointer++;
        bytes -= 2;
    }

    // Add left-over byte, if any
    if (bytes > 0)
        total_sum += *((unsigned char *)data_pointer);

    // Fold 32-bit sum to 16 bits
    while (total_sum >> 16)
        total_sum = (total_sum & 0xFFFF) + (total_sum >> 16);

    return (~((unsigned short int)total_sum));
}

int rudp_socket() {
    int sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sockfd < 0) {
        perror("Failed to create UDP socket");
        return -1;
    }
    // Additional initialization code for RUDP protocol parameters
    return sockfd;
}

int rudp_send(int sockfd, const void *buf, short len, int flags, const char *ip, int port) {
    RUDP_Packet *packet;
    // memset(&packet, 0, sizeof(packet));
    *packet.header->len = (uint16_t)len;
    memcpy(packet->data, buf, len);
    packet->checksum = calculate_checksum(packet->data, packet->length);

    struct sockaddr_in dest_addr;
    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(port);
    dest_addr.sin_addr.s_addr = inet_addr(ip);

    if (sendto(sockfd, &packet, sizeof(packet), 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) < 0) {
        perror("Failed to send packet");
        return -1;
    }

    return 0;
}

int rudp_recv(int sockfd, void *buf, size_t len, int flags, int port) {
    struct sockaddr_in from_addr;
    memset(&from_addr, 0, sizeof(from_addr));
    // from_addr.sin_family = AF_INET;
    // from_addr.sin_port = htons(port);
    // from_addr.sin_addr.s_addr = htons(INADDR_ANY);
    socklen_t from_len = sizeof(from_addr);
    RUDP_Packet packet;

    // Initialize buffer to zero
    memset(&packet, 0, sizeof(packet));

    // Receive a packet
    ssize_t received = recvfrom(sockfd, &packet, sizeof(packet), flags, (struct sockaddr *)&from_addr, &from_len);
    if (received < 0) {
        perror("Failed to receive data");
        return -1;
    }

    // Calculate checksum for the received packet
    unsigned short original_checksum = packet.checksum;  // Store original checksum
    // packet.checksum = 0;  // Zero out checksum for calculation
    unsigned short calculated_checksum = calculate_checksum(&packet, sizeof(packet));

    // Check checksum validity
    if (original_checksum != calculated_checksum) {
        fprintf(stderr, "Checksum mismatch: expected %hu, got %hu\n", calculated_checksum, original_checksum);
        return -1;  // Return error for checksum mismatch
    }

    // Copy data to user buffer if valid and packet has content
    size_t data_length = packet.length < len ? packet.length : len;  // Ensure we do not overflow the buffer
    memcpy(buf, packet.data, data_length);

    return data_length;  // Return the number of bytes copied to the buffer
}

int rudp_close(int sockfd) {
	if(close(sockfd) == -1) {
		perror("Failed to close socket");
		return -1;
	}
	return 0;
}
