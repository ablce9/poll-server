#include <stdio.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#include "./epoll.h"

#define MAX_EVENTS 1024
struct poll_data {
    int fd;
    struct epoll_event event_list[MAX_EVENTS];
    //read_callback read_callback;
};

static void die(int code, const char *message) {
    printf("%s", message);
    exit(code);
}

/* @return *struct poll_data* */
struct poll_data *new_poll() {
    int epfd = epoll_create1(EPOLL_CLOEXEC);
    if (epfd == -1)
	die(epfd, "epll_create1");

    struct poll_data *po;
    memset(&po, 0, sizeof(po));
    po = malloc(sizeof(struct poll_data*));
    po->fd = epfd;
    return po;
}

void register_client(struct poll_data *data, int fd) {
    struct epoll_event event = { data: { fd: fd }, events: EPOLLIN };
    int code;
    if ((code = epoll_ctl(data->fd, EPOLL_CTL_ADD, fd, &event)) == -1)
	die(code, "epoll_ctl");
}

int dispatch(struct poll_data *data, read_callback read_cb) {
    int num_event;
    num_event = epoll_wait(data->fd, data->event_list, MAX_EVENTS, -1);
    if (num_event == -1 && errno != EINTR)
	die(num_event, "epoll_wait");

#define DEBUG_EVENT(evt) evt & EPOLLOUT ? "EPOLLOUT" : "" \
	|| evt & EPOLLIN ? "EPOLLIN" : ""		  \
	|| evt & EPOLLHUP ? "EPOLLHUP" : ""		  \
	|| evt & EPOLLERR ? "EPOLLERR" : ""
    for (int evt = 0; evt < num_event; evt++) {
	printf("event: %s\n", DEBUG_EVENT(data->event_list[evt].events));
	if (data->event_list[evt].events & EPOLLIN) {
	    read_cb(data->event_list[evt].data.fd);
	} else if (data->event_list[evt].events & EPOLLOUT) {
	    // write callback
	} else if (data->event_list[evt].events & (EPOLLHUP | EPOLLERR)) {
	    // If EPOLLHUP and EPOLLIN were both set, buffer will be consumed in
	    // the next round.
	    int code;
	    code = close(data->event_list[evt].data.fd);
	    if (code == -1)
		die(code, "close");
	    printf("closing file descriptor: %d\n", data->event_list[evt].data.fd);
	}
    }
#undef DEBUG_EVENT
    return num_event;
}

#undef MAX_EVENTS
