/*
 * OpenOS - Networking Implementation
 */

#include "network.h"
#include "console.h"
#include "string.h"

/* Global network device */
static net_device_t net_dev;
static int net_initialized = 0;

/* Socket storage */
#define MAX_SOCKETS 32
static socket_t sockets[MAX_SOCKETS];

/* Initialize networking subsystem */
void net_init(void) {
    if (net_initialized) return;
    
    console_write("NET: Initializing networking stack...\n");
    
    /* Initialize network device */
    strncpy(net_dev.name, "eth0", sizeof(net_dev.name));
    net_dev.is_up = 0;
    
    /* Set default MAC address (00:11:22:33:44:55) */
    net_dev.mac.addr[0] = 0x00;
    net_dev.mac.addr[1] = 0x11;
    net_dev.mac.addr[2] = 0x22;
    net_dev.mac.addr[3] = 0x33;
    net_dev.mac.addr[4] = 0x44;
    net_dev.mac.addr[5] = 0x55;
    
    /* Set default IP address (192.168.1.100) */
    net_dev.ip.addr[0] = 192;
    net_dev.ip.addr[1] = 168;
    net_dev.ip.addr[2] = 1;
    net_dev.ip.addr[3] = 100;
    
    /* Initialize sockets */
    for (int i = 0; i < MAX_SOCKETS; i++) {
        sockets[i].is_open = 0;
    }
    
    net_dev.is_up = 1;
    net_initialized = 1;
    
    console_write("NET: eth0 up at 192.168.1.100\n");
}

/* Set IP address */
void net_set_ip(ip_addr_t* ip) {
    if (ip) {
        memcpy(&net_dev.ip, ip, sizeof(ip_addr_t));
    }
}

/* Set MAC address */
void net_set_mac(mac_addr_t* mac) {
    if (mac) {
        memcpy(&net_dev.mac, mac, sizeof(mac_addr_t));
    }
}

/* Get network device */
net_device_t* net_get_device(void) {
    return &net_dev;
}

/* Send packet (stub) */
int net_send_packet(packet_t* packet) {
    if (!packet || !net_dev.is_up) return -1;
    
    /* TODO: Implement actual packet transmission */
    return packet->length;
}

/* Receive packet (stub) */
int net_receive_packet(packet_t* packet) {
    if (!packet || !net_dev.is_up) return -1;
    
    /* TODO: Implement actual packet reception */
    return 0;
}

/* Create a socket */
socket_t* net_socket_create(uint8_t protocol) {
    for (int i = 0; i < MAX_SOCKETS; i++) {
        if (!sockets[i].is_open) {
            sockets[i].id = i;
            sockets[i].protocol = protocol;
            sockets[i].local_port = 0;
            sockets[i].remote_port = 0;
            sockets[i].is_open = 1;
            return &sockets[i];
        }
    }
    return NULL;
}

/* Bind socket to port */
int net_socket_bind(socket_t* socket, uint16_t port) {
    if (!socket || !socket->is_open) return -1;
    
    socket->local_port = port;
    return 0;
}

/* Connect socket to remote host */
int net_socket_connect(socket_t* socket, ip_addr_t* ip, uint16_t port) {
    if (!socket || !socket->is_open || !ip) return -1;
    
    memcpy(&socket->remote_ip, ip, sizeof(ip_addr_t));
    socket->remote_port = port;
    return 0;
}

/* Send data through socket */
int net_socket_send(socket_t* socket, const void* data, size_t size) {
    if (!socket || !socket->is_open || !data) return -1;
    
    /* TODO: Implement actual socket send */
    return size;
}

/* Receive data from socket */
int net_socket_recv(socket_t* socket, void* buffer, size_t size) {
    if (!socket || !socket->is_open || !buffer) return -1;
    
    /* TODO: Implement actual socket receive */
    (void)size;  /* Parameter reserved for future implementation */
    return 0;
}

/* Close socket */
void net_socket_close(socket_t* socket) {
    if (socket) {
        socket->is_open = 0;
    }
}

/* Calculate Internet checksum */
uint16_t net_checksum(const void* data, size_t length) {
    const uint16_t* ptr = (const uint16_t*)data;
    uint32_t sum = 0;
    
    while (length > 1) {
        sum += *ptr++;
        length -= 2;
    }
    
    if (length > 0) {
        sum += *(uint8_t*)ptr;
    }
    
    while (sum >> 16) {
        sum = (sum & 0xFFFF) + (sum >> 16);
    }
    
    return ~sum;
}
