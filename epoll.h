#ifndef __m_epoll_h

#define MAX_BUFFER_SIZE 65536

struct poll_data;

typedef void(*read_callback)(int fd); // , unsigned int *buffer, size_t size);
struct poll_data *new_poll(void);
void register_client(struct poll_data *, int);
int dispatch(struct poll_data *, read_callback);
#endif /* __m_epoll_h */
