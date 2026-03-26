#ifndef _REQUEST_QUEUE_H
#define _REQUEST_QUEUE_H

#include "segel.h"

typedef struct Threads_stats* threads_stats;
typedef struct RequestQueue* request_queue;
typedef struct Request {
    int fd;
    threads_stats stats;
    struct timeval arrival, dispatch;
}* request_t;
typedef request_t(*request_getter_t)(int socketfd);

request_queue create_request_queue(int max_size);
void destroy_request_queue(request_queue queue);
void add_request_to_queue(request_queue queue, request_getter_t request_getter, int socketfd);
request_t poll_request_from_queue(request_queue queue);
void update_request_finished(request_queue queue, request_t request);

#endif