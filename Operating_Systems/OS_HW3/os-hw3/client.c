/*
 * client.c: A very, very primitive HTTP client.
 * 
 * Example usage:
 *      ./client www.example.com 80 /
 *
 * This client sends a single HTTP request to a server and prints the response.
 *
 * HW3: For testing your server, you will likely want to modify this client:
 *  - Add multi-threading support to test concurrency.
 *  - Add ability to send different request types and URIs.
 *  - Consider reading URIs or request data from a file or stdin.
 *
 * NOTE: We will use a modified version of this client to test your server.
 */

#include "segel.h"

// Sends an HTTP request to the server using the given socket
void clientSend(int fd, char *filename, char* method)
{
    char buf[MAXLINE];
    char hostname[MAXLINE];

    Gethostname(hostname, MAXLINE); // Get the client's hostname

    // Form the HTTP request line and headers
    sprintf(buf, "%s %s HTTP/1.1\n", method, filename);
    sprintf(buf, "%shost: %s\n\r\n", buf, hostname);

    LOG("Request: %s %s HTTP/1.1\n", method, filename); // Display the request for debugging

    // Send the request to the server
    Rio_writen(fd, buf, strlen(buf));
}

// Reads and prints the server's HTTP response
void clientPrint(int fd)
{
    rio_t rio;
    char buf[MAXBUF];
    int length = 0;
    int n;

    // Initialize buffered input
    Rio_readinitb(&rio, fd);

    // --- Read and print HTTP headers ---
    n = Rio_readlineb(&rio, buf, MAXBUF);
    while (strcmp(buf, "\r\n") && (n > 0)) {
        // printf("Header: %s", buf);

        n = Rio_readlineb(&rio, buf, MAXBUF);
    }

    // --- Read and print HTTP body ---
    n = Rio_readlineb(&rio, buf, MAXBUF);
    while (n > 0) {
        // Try to parse Content-Length header
        if (sscanf(buf, "Content-Length: %d", &length) == 1) {
            LOG("Length = %d\n", length);
        }
        // printf("%s", buf);
        n = Rio_readlineb(&rio, buf, MAXBUF);
    }
}

typedef struct {
    char* filename;
    int port;
    char* method;
} ThreadData;

void* routine(void* ptr) {
    ThreadData* data = (ThreadData*)ptr;
    
    // Open a connection to the specified server
    int clientfd = Open_clientfd("localhost", data->port);
    if (clientfd < 0) {
        fprintf(stderr, "Error connecting to port %d\n", data->port);
        exit(1);
    }

    LOG("Connected to server. clientfd = %d\n", clientfd);

    // Send HTTP request and read response
    clientSend(clientfd, data->filename, data->method);
    clientPrint(clientfd);

    LOG("Finished session: %d\n", clientfd);

    // Clean up
    Close(clientfd);

    return NULL;
}

int main(int argc, char *argv[])
{
    char *filename, *method;
    int port;
    int clientfd;
    int connections;

    // Validate input arguments
    if (argc != 5) {
        fprintf(stderr, "Usage: %s <port> <filename> <method> <connections>\n", argv[0]);
        exit(1);
    }

    // Parse command-line arguments
    port = atoi(argv[1]);
    filename = argv[2];
    method = argv[3];
    connections = atoi(argv[4]);

    ThreadData data = { filename, port, method };
    pthread_t* threads = (pthread_t*)malloc(sizeof(pthread_t) * connections);
    for (int i = 0; i < connections; i++) {
        threads[i] = ThreadCreate(routine, &data);
        ThreadWorkerName(threads[i], "client", i + 1);
    }
    for (int i = 0; i < connections; i++) {
        ThreadJoin(threads[i]);
    }

    return 0;
}
