#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

#define PORT 8080
#define DATA_SIZE 1073741824           // 1 GB
#define SMALL_PACKET_SIZE 64           // Small packet size
#define MEDIUM_PACKET_SIZE 512         // Medium packet size
#define LARGE_PACKET_SIZE 4096         // Large packet size
#define EXTRA_LARGE_PACKET_SIZE 65536  // Extra large packet size

void benchmark(int client_socket, size_t packet_size) {
    char *data = malloc(packet_size);
    if (data == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    clock_t start = clock();
    for (size_t i = 0; i < DATA_SIZE / packet_size; ++i) {
        if (recv(client_socket, data, packet_size, 0) < 0) {
            perror("recv");
            exit(EXIT_FAILURE);
        }
    }
    clock_t end = clock();
    free(data);

    double elapsed = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Time taken for %ld packets of size %ld bytes: %.6f seconds\n",
           DATA_SIZE / packet_size, packet_size, elapsed);
}

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr;

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_socket, (struct sockaddr *)&server_addr,
             sizeof(server_addr)) < 0) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    if (listen(server_socket, 5) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("Server waiting for connection...\n");
    client_socket = accept(server_socket, NULL, NULL);
    if (client_socket < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    benchmark(client_socket, SMALL_PACKET_SIZE);
    benchmark(client_socket, MEDIUM_PACKET_SIZE);
    benchmark(client_socket, LARGE_PACKET_SIZE);
    benchmark(client_socket, EXTRA_LARGE_PACKET_SIZE);

    close(client_socket);
    close(server_socket);
    return 0;
}
