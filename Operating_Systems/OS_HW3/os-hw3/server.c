#include "segel.h"
#include "request.h"
#include "log.h"

typedef struct {
    int port;
    int threads;
    int queue_size;
} command_line_args;

// Parses command-line arguments
command_line_args getargs(int argc, char *argv[])
{
    if (argc < 4) {
        fprintf(stderr, "Usage: %s <port> <threads> <queue_size>\n", argv[0]);
        exit(1);
    }

    command_line_args args = { atoi(argv[1]), atoi(argv[2]), atoi(argv[3]) };
    return args;
}

request_t get_request(int socketfd) {
    struct sockaddr_in clientaddr;
    int clientlen = sizeof(clientaddr);
    int connfd = Accept(socketfd, (SA *)&clientaddr, (socklen_t *) &clientlen);
    request_t request = (request_t)malloc(sizeof(struct Request));
    request->fd = connfd;
    request->stats = malloc(sizeof(struct Threads_stats));
    gettimeofday(&request->arrival, NULL);
    return request;
}

int main(int argc, char *argv[])
{
    // if (-1 == signal(SIGINT, sigint_handler)) unix_error("Ctrl+C not handled");
    command_line_args args = getargs(argc, argv);
    // Create the global server log
    server_log log = create_log();
    request_queue queue = create_request_queue(args.queue_size);

    int listenfd;

    listenfd = Open_listenfd(args.port);
    ThreadName(pthread_self(), "master");

    pthread_t* threadpool = (pthread_t*)malloc(sizeof(pthread_t) * args.threads);
    for (int i = 0; i < args.threads; i++) {
        request_listener_data threadpool_data = (request_listener_data)malloc(sizeof(struct RequestListenerData));
        memset(threadpool_data, 0, sizeof(struct RequestListenerData));
        threadpool_data->log = log;
        threadpool_data->queue = queue;
        threadpool_data->id = i + 1;
        threadpool[i] = ThreadCreate(request_listener, threadpool_data);
    }

    while (1) {
        add_request_to_queue(queue, get_request, listenfd);
    }

    for (int i = 0; i < args.threads; i++) {
        ThreadCancel(threadpool[i]);
    }
    // Clean up the server log before exiting
    destroy_log(log);
    destroy_request_queue(queue);
}
