#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define SOCKET_PATH "/tmp/benchmark_socket"
#define DATA_SIZE 1073741824           // 1 GB
#define SMALL_PACKET_SIZE 64           // Small packet size
#define MEDIUM_PACKET_SIZE 512         // Medium packet size
#define LARGE_PACKET_SIZE 4096         // Large packet size
#define EXTRA_LARGE_PACKET_SIZE 65536  // Extra large packet size

void send_packets(int server_socket, size_t packet_size) {
    char *data = malloc(packet_size);
    if (data == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    for (size_t i = 0; i < DATA_SIZE / packet_size; ++i) {
        if (send(server_socket, data, packet_size, 0) < 0) {
            perror("send");
            exit(EXIT_FAILURE);
        }
    }
    free(data);
}

int main() {
    int server_socket;
    struct sockaddr_un server_addr;

    server_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(struct sockaddr_un));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SOCKET_PATH,
            sizeof(server_addr.sun_path) - 1);

    if (connect(server_socket, (struct sockaddr *)&server_addr,
                sizeof(struct sockaddr_un)) < 0) {
        perror("connect");
        exit(EXIT_FAILURE);
    }

    send_packets(server_socket, SMALL_PACKET_SIZE);
    send_packets(server_socket, MEDIUM_PACKET_SIZE);
    send_packets(server_socket, LARGE_PACKET_SIZE);
    send_packets(server_socket, EXTRA_LARGE_PACKET_SIZE);

    close(server_socket);
    return 0;
}
