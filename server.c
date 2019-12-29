/*
** server.c powered by epoll
*/

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <signal.h>

#include "./epoll.h"

void signal_handler(int sig) {
    pid_t pid = getpid();
    if (sig & SIGTERM) {
	printf("Got signal, pid:  %d\n", pid);
	// if (kill(pid, sig) == -1) {
	//     printf("kill\n");
	//     exit(1);
	// }
    }
}

static void read_cb(int fd) {
    int num_read;
    unsigned int buf[MAX_BUFFER_SIZE];
    num_read = read(fd, buf, sizeof(buf));
    if (num_read < 0)
	printf("Failed to read.\n");
    printf("read: %d\n", num_read);
}

int main(void) {
    struct sigaction sa;
    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGTERM, &sa, NULL) == -1) {
	printf("sigaction\n");
	exit(1);
    }

    struct addrinfo hints, *serverinfo, *p;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

#define PORT "2000"
    int code;
    code = getaddrinfo(NULL, PORT, &hints, &serverinfo);
    if (code != 0) {
	printf("getaddrinfo\n");
	exit(1);
    }
#undef PORT

    int fd, new_fd, optval = 1;
    /* Loop through all the results and bind to the first. */
    for (p = serverinfo; p != NULL; p = (*p).ai_next) {
	if ((fd = socket((*p).ai_family, (*p).ai_socktype, (*p).ai_protocol)) == -1) {
	    printf("socket\n");
	    continue;
	}

	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1) {
	    printf("setsockopt\n");
	    exit(1);
	}

	if (bind(fd, (*p).ai_addr, (*p).ai_addrlen) == -1) {
	    close(fd);
	    printf("bind\n");
	    continue;
	}

	break;
    }

    freeaddrinfo(serverinfo); /* Clean up the precious memory. */

    if (p == NULL) {
	printf("failed to bind\n");
	exit(1);
    }

#define BACKLOG 128
    if (listen(fd, BACKLOG) == -1) {
	printf("listen\n");
	exit(1);
    }
#undef BACKLOG

    printf("Server is up and running...\n");

    struct poll_data *data = new_poll();
    struct sockaddr_storage their_addr;
    socklen_t sin_size;
    for(;;) {
	sin_size = sizeof(their_addr);
	new_fd = accept4(fd, (struct sockaddr *)&their_addr, &sin_size, SOCK_NONBLOCK);
	if (new_fd == -1) {
	    printf("accpet\n");
	    continue;
	}
	register_client(data, new_fd);
	dispatch(data, read_cb);
	printf("Got new client\n");
	const char *msg = "12345678";
	if (send(new_fd, msg, sizeof(msg), 0) == -1)
	    printf("send\n");
	close(new_fd);
	// Kick out registered event here.
    }
    free(data);
    printf("clean\n");
    return 0;
}
