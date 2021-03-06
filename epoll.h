#ifndef __m_epoll_h

#include <sys/epoll.h>

#define MAX_BUFFER_SIZE 65536
#define MAX_EVENTS 1024

struct poll_data {
    int fd;
    struct epoll_event event_list[MAX_EVENTS];
    //read_callback read_callback;
};

typedef void(*read_callback)(int fd); // , unsigned int *buffer, size_t size);
struct poll_data *new_poll(void);
void register_client(struct poll_data *, int);
int dispatch(struct poll_data *, read_callback);
#endif /* __m_epoll_h */
