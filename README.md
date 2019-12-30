# TCP Server powered by Linux poll API
This is the advanced example of a tcp server implementation powered by poll API. Just writing socket programs is boring and not so exciting so I started writing an example program, spending boring new's year holidays.

In general, Poll API helps network-based server applications scale well. The key to a scale-well theory is to make file descriptors none-blocking, meaning putting descriptors into a queue and letting OSes read/write 'em later work when OSes have time for it. Epoll has been the main API used by many network-oriented programs/libraries for years. The example is intended to understand technologies and *hope* to utilize poll APIs for daily application developments.

# API
```c
struct poll_data *new_poll(void); // Make an instance of poll_data.
void register_client(struct poll_data *, int); // Register client file descriptor.
int dispatch(struct poll_data *, read_callback); // Dispatch with read callback.
```
