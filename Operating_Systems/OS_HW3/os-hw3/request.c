//
// request.c: Does the bulk of the work for the web server.
// 

#include "segel.h"
#include "request.h"

#define MAXSTATS 256

int append_stats(char* buf, threads_stats t_stats, struct timeval arrival, struct timeval dispatch){
    int offset = strlen(buf);  // Start after what's already written to buf

    offset += sprintf(buf + offset, "Stat-Req-Arrival:: %ld.%06ld\r\n",
                      arrival.tv_sec, arrival.tv_usec);

    offset += sprintf(buf + offset, "Stat-Req-Dispatch:: %ld.%06ld\r\n",
                      dispatch.tv_sec, dispatch.tv_usec);

    offset += sprintf(buf + offset, "Stat-Thread-Id:: %d\r\n",
                      t_stats->id);

    offset += sprintf(buf + offset, "Stat-Thread-Count:: %d\r\n",
                      t_stats->total_req);

    offset += sprintf(buf + offset, "Stat-Thread-Static:: %d\r\n",
                      t_stats->stat_req);

    offset += sprintf(buf + offset, "Stat-Thread-Dynamic:: %d\r\n",
                      t_stats->dynm_req);
    offset += sprintf(buf + offset, "Stat-Thread-Post:: %d\r\n\r\n",
                      t_stats->post_req);
    return offset;
}

// requestError(      fd,    filename,        "404",    "Not found", "OS-HW3 Server could not find this file");
void requestError(int fd, char *cause, char *errnum, char *shortmsg, char *longmsg, struct timeval arrival, struct timeval dispatch, threads_stats t_stats)
{
	char buf[MAXLINE], body[MAXBUF];

	// Create the body of the error message
	sprintf(body, "<html><title>OS-HW3 Error</title>");
	sprintf(body, "%s<body bgcolor=""fffff"">\r\n", body);
	sprintf(body, "%s%s: %s\r\n", body, errnum, shortmsg);
	sprintf(body, "%s<p>%s: %s\r\n", body, longmsg, cause);
	sprintf(body, "%s<hr>OS-HW3 Web Server\r\n", body);

	// Write out the header information for this response
	sprintf(buf, "HTTP/1.0 %s %s\r\n", errnum, shortmsg);
	Rio_writen(fd, buf, strlen(buf));
	printf("%s", buf);

	sprintf(buf, "Content-Type: text/html\r\n");
	Rio_writen(fd, buf, strlen(buf));
	printf("%s", buf);

	sprintf(buf, "Content-Length: %lu\r\n", strlen(body));

    int buf_len = append_stats(buf, t_stats, arrival, dispatch);

	Rio_writen(fd, buf, buf_len);
	printf("%s", buf);
	Rio_writen(fd, body, strlen(body));
	printf("%s", body);

}


void requestReadhdrs(rio_t *rp)
{
	char buf[MAXLINE];

	Rio_readlineb(rp, buf, MAXLINE);
	while (strcmp(buf, "\r\n")) {
		Rio_readlineb(rp, buf, MAXLINE);
	}
	return;
}

//
// Return 1 if static, 0 if dynamic content
// Calculates filename (and cgiargs, for dynamic) from uri
//
int requestParseURI(char *uri, char *filename, char *cgiargs)
{
	char *ptr;
	if (strstr(uri, "..")) {
		sprintf(filename, "./public/home.html");
		return 1;
	}
	if (!strstr(uri, "cgi")) {
		// static
		strcpy(cgiargs, "");
		sprintf(filename, "./public/%s", uri);
		if (uri[strlen(uri)-1] == '/') {
			strcat(filename, "home.html");
		}
		return 1;
	} else {
		// dynamic
		ptr = index(uri, '?');
		if (ptr) {
			strcpy(cgiargs, ptr+1);
			*ptr = '\0';
		} else {
			strcpy(cgiargs, "");
		}
		sprintf(filename, "./public/%s", uri);
		return 0;
	}
}

//
// Fills in the filetype given the filename
//
void requestGetFiletype(char *filename, char *filetype)
{
	if (strstr(filename, ".html"))
		strcpy(filetype, "text/html");
	else if (strstr(filename, ".gif"))
		strcpy(filetype, "image/gif");
	else if (strstr(filename, ".jpg"))
		strcpy(filetype, "image/jpeg");
	else
		strcpy(filetype, "text/plain");
}

void requestServeDynamic(int fd, char *filename, char *cgiargs, struct timeval arrival, struct timeval dispatch, threads_stats t_stats)
{
	char buf[MAXLINE], *emptylist[] = {NULL};

	// The server does only a little bit of the header.
	// The CGI script has to finish writing out the header.
	sprintf(buf, "HTTP/1.0 200 OK\r\n");
	sprintf(buf, "%sServer: OS-HW3 Web Server\r\n", buf);
    int buf_len = append_stats(buf, t_stats, arrival, dispatch);

    Rio_writen(fd, buf, buf_len);
   	int pid = 0;
   	if ((pid = Fork()) == 0) {
     	 /* Child process */
     	 Setenv("QUERY_STRING", cgiargs, 1);
     	 /* When the CGI process writes to stdout, it will instead go to the socket */
     	 Dup2(fd, STDOUT_FILENO);
     	 Execve(filename, emptylist, environ);
   	}
  	WaitPid(pid, NULL, WUNTRACED);
}


void requestServeStatic(int fd, char *filename, int filesize, struct timeval arrival, struct timeval dispatch, threads_stats t_stats)
{
	int srcfd;
	char *srcp, filetype[MAXLINE], buf[MAXBUF];

	requestGetFiletype(filename, filetype);

	srcfd = Open(filename, O_RDONLY, 0);

	// Rather than call read() to read the file into memory,
	// which would require that we allocate a buffer, we memory-map the file
	srcp = Mmap(0, filesize, PROT_READ, MAP_PRIVATE, srcfd, 0);
	Close(srcfd);

	// put together response
	sprintf(buf, "HTTP/1.0 200 OK\r\n");
	sprintf(buf, "%sServer: OS-HW3 Web Server\r\n", buf);
	sprintf(buf, "%sContent-Length: %d\r\n", buf, filesize);
	sprintf(buf, "%sContent-Type: %s\r\n", buf, filetype);
    int buf_len = append_stats(buf, t_stats, arrival, dispatch);
    Rio_writen(fd, buf, buf_len);

	//  Writes out to the client socket the memory-mapped file
	Rio_writen(fd, srcp, filesize);
	Munmap(srcp, filesize);
}

void requestServePost(int fd,  struct timeval arrival, struct timeval dispatch, threads_stats t_stats, server_log log)
{
    char header[MAXBUF], *body = NULL;
    int body_len = get_log(log, &body);
    // put together response
    sprintf(header, "HTTP/1.0 200 OK\r\n");
    sprintf(header, "%sServer: OS-HW3 Web Server\r\n", header);
    sprintf(header, "%sContent-Length: %d\r\n", header, body_len);
    sprintf(header, "%sContent-Type: %s\r\n", header, "text/plain");
    int header_len = append_stats(header, t_stats, arrival, dispatch);
    Rio_writen(fd, header, header_len);
    Rio_writen(fd, body, body_len);
    free(body);
}

// handle a request
void requestHandle(request_t request, request_listener_data data)
{
	request->stats->total_req = ++(data->totalRequests);
    int is_static;
    struct stat sbuf;
    char buf[MAXLINE], method[MAXLINE], uri[MAXLINE], version[MAXLINE];
    char filename[MAXLINE], cgiargs[MAXLINE];
    rio_t rio;

    Rio_readinitb(&rio, request->fd);
    Rio_readlineb(&rio, buf, MAXLINE);
    sscanf(buf, "%s %s %s", method, uri, version);

    if (!strcasecmp(method, "GET")) {
        requestReadhdrs(&rio);

        is_static = requestParseURI(uri, filename, cgiargs);
        if (stat(filename, &sbuf) < 0) {
            requestError(request->fd, filename, "404", "Not found",
                         "OS-HW3 Server could not find this file",
                         request->arrival, request->dispatch, request->stats);
            return;
        }

        if (is_static) {
            if (!(S_ISREG(sbuf.st_mode)) || !(S_IRUSR & sbuf.st_mode)) {
                requestError(request->fd, filename, "403", "Forbidden",
                             "OS-HW3 Server could not read this file",
                             request->arrival, request->dispatch, request->stats);
                return;
            }

			request->stats->stat_req = ++(data->staticRequests);
            requestServeStatic(request->fd, filename, sbuf.st_size, request->arrival, request->dispatch, request->stats);

        } else {
            if (!(S_ISREG(sbuf.st_mode)) || !(S_IXUSR & sbuf.st_mode)) {
                requestError(request->fd, filename, "403", "Forbidden",
                             "OS-HW3 Server could not run this CGI program",
                             request->arrival, request->dispatch, request->stats);
                return;
            }

			request->stats->dynm_req = ++(data->dynamicRequests);
            requestServeDynamic(request->fd, filename, cgiargs, request->arrival, request->dispatch, request->stats);
        }
        char LOG[MAXSTATS] = { 0 };
		int len = append_stats(LOG, request->stats, request->arrival, request->dispatch);
		add_to_log(data->log, LOG, len);
    } else if (!strcasecmp(method, "POST")) {
		request->stats->post_req = ++(data->postRequests);
        requestServePost(request->fd, request->arrival, request->dispatch, request->stats, data->log);

    } else {
        requestError(request->fd, method, "501", "Not Implemented",
                     "OS-HW3 Server does not implement this method",
                     request->arrival, request->dispatch, request->stats);
        return;
    }
}

void timeval_subtract(struct timeval* end, struct timeval* start) {
    end->tv_sec -= start->tv_sec;
    end->tv_usec -= start->tv_usec;

    if (end->tv_usec < 0) {
        end->tv_sec -= 1;
        end->tv_usec += 1000000;
    }
}

void* request_listener(void* ptr) {
	request_listener_data data = (request_listener_data)ptr;
	ThreadWorkerName(pthread_self(), "worker", data->id);
	while (1) {
		request_t request = poll_request_from_queue(data->queue);
		gettimeofday(&request->dispatch, NULL);
		timeval_subtract(&request->dispatch, &request->arrival);
		request->stats->id = data->id;
		request->stats->dynm_req = data->dynamicRequests;
		request->stats->stat_req = data->staticRequests;
		request->stats->total_req = data->totalRequests;
		request->stats->post_req = data->postRequests;
        requestHandle(request, data);
		update_request_finished(data->queue, request);
	}
	free(data);
}