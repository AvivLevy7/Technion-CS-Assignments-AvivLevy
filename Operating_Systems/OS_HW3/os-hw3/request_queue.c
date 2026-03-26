#include "request_queue.h"
#include "segel.h"

struct RequestQueue {
    request_t* requests;
    sem_t add_sem, poll_sem;
    size_t start_index, end_index, max_size;
    pthread_mutex_t lock;
};

request_queue create_request_queue(int max_size) {
    request_queue queue = (request_queue)malloc(sizeof(struct RequestQueue));
    queue->requests = (request_t*)calloc(sizeof(struct Request), max_size);
    queue->start_index = queue->end_index = 0;
    queue->max_size = max_size;
    SemInit(&queue->add_sem, max_size);
    SemInit(&queue->poll_sem, 0);
    MutexInit(&queue->lock);
    return queue;
}

void destroy_request_queue(request_queue queue) {
    SemDestroy(&queue->add_sem);
    SemDestroy(&queue->poll_sem);
    MutexDestroy(&queue->lock);
    for (int i = 0; i < queue->max_size; i++) {
        if (queue->requests[i]) free(queue->requests[i]);
    }
    free(queue->requests);
    free(queue);
}
void add_request_to_queue(request_queue queue, request_getter_t request_getter, int socketfd) {
    SemWait(&queue->add_sem);
    request_t request = request_getter(socketfd);
    LOG("Add request, fd=%d\n", request->fd);
    MutexLock(&queue->lock);
    queue->requests[queue->end_index] = request;
    queue->end_index = (queue->end_index + 1) % queue->max_size;
    MutexUnlock(&queue->lock);
    SemPost(&queue->poll_sem);
}
request_t poll_request_from_queue(request_queue queue) {
    LOGLINE("Asking to poll request\n");
    SemWait(&queue->poll_sem);
    MutexLock(&queue->lock);
    request_t request = queue->requests[queue->start_index];
    queue->requests[queue->start_index] = NULL;
    queue->start_index = (queue->start_index + 1) % queue->max_size;
    MutexUnlock(&queue->lock);
    LOG("Polling request, fd=%d\n", request->fd);
    return request;
}
void update_request_finished(request_queue queue, request_t request) {
    LOG("Finished request, fd=%d\n", request->fd);
    Close(request->fd);
    free(request->stats);
    free(request);
    SemPost(&queue->add_sem);
}