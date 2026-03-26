#include "segel.h"
#include "log.h"

typedef struct String_Node {
    const char* string;
    size_t len;
    struct String_Node* next;
}* string_node;

// Opaque struct definition
struct Server_Log {
    int current_readers, current_writers, waiting_writers;
    pthread_cond_t read_cond, write_cond;
    pthread_mutex_t global_lock;

    size_t size, amount; //size is in bytes for serialization
    string_node logs, final_log;
};

// Creates a new server log instance (stub)
server_log create_log() {
    server_log log = (server_log)malloc(sizeof(struct Server_Log));
    log->current_readers = log->current_writers = log->waiting_writers = 0;
    CondInit(&log->read_cond);
    CondInit(&log->write_cond);
    MutexInit(&log->global_lock);

    log->size = log->amount = 0;
    log->logs = log->final_log = NULL;
    return log;
}

// Destroys and frees the log (stub)
void destroy_log(server_log log) {
    CondDestroy(&log->read_cond);
    CondDestroy(&log->write_cond);
    MutexDestroy(&log->global_lock);
    
    string_node node = log->logs;
    while (node) {
        if (node->string) free((void*)node->string);
        string_node next = node->next;
        free(node);
        node = next;
    }
    free(log);
}

// Returns dummy log content as string (stub)
int get_log(server_log log, char** dst) {
    MutexLock(&log->global_lock);
    while (log->current_writers > 0 || log->waiting_writers > 0) CondWait(&log->read_cond, &log->global_lock);
    log->current_readers++;
    MutexUnlock(&log->global_lock);

    if (!log->logs) {
        *dst = (char*)malloc(sizeof(char));
        **dst = '\0';
        return 1;
    }

    *dst = (char*)malloc((log->size + 1) * sizeof(char));
    char* str = *dst;
    string_node node = log->logs;
    while (node) {
        strcpy(str, node->string);
        str += node->len;
    }
    str[-1] = '\0';

    MutexLock(&log->global_lock);
    if (0 == --(log->current_readers)) CondSignal(&log->write_cond);
    MutexUnlock(&log->global_lock);

    return log->size;
}

// Appends a new entry to the log (no-op stub)
void add_to_log(server_log log, const char* data, int data_len) {
    MutexLock(&log->global_lock);
    log->waiting_writers++;
    while (log->current_writers + log->current_readers > 0) CondWait(&log->write_cond, &log->global_lock);
    log->waiting_writers--;
    log->current_writers++;
    MutexUnlock(&log->global_lock);

    string_node new_node = (string_node)malloc(sizeof(string_node));
    new_node->len = data_len;
    new_node->next = NULL;
    char* new_string = (char*)malloc(sizeof(char) * (data_len + 1));
    strcpy(new_string, data);
    new_node->string = new_string;

    if (!log->logs) {
        log->logs = log->final_log = new_node;
    } else {
        log->final_log->next = new_node;
        log->final_log = new_node;
    }

    log->amount++;
    log->size += data_len;
    
    MutexLock(&log->global_lock);
    if (0 == --(log->current_writers)) {
        CondBroacast(&log->read_cond);
        CondSignal(&log->write_cond);
    }
    MutexUnlock(&log->global_lock);
}
