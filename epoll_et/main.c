#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <libgen.h>
#include <assert.h>
#include <stdbool.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <pthread.h>

#define MAX_EVENT_NUMBER 1024
#define BUFFER_SIZE 1024

#define ONE_SHOT true
#define NOT_ONE_SHOT false

struct data {
	int epollfd;
	int sockfd;
};

void setnonblockling(int fd)
{
	int fd_option = fcntl(fd, F_GETFL);
	fd_option |= O_NONBLOCK;
	fcntl(fd, F_SETFL, fd_option);
}

void add_fd(int epollfd, int fd, bool flag)
{
	struct epoll_event event;
	event.data.fd = fd;
	event.events = EPOLLIN | EPOLLET;	//监听可读事件,ET模式
	if (flag)
		event.events |= EPOLLONESHOT;

	epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event);
	setnonblockling(fd);
}

void reset_oneshot(int epollfd, int fd)
{
	struct epoll_event event;
	event.data.fd = fd;
	event.events = EPOLLIN | EPOLLET | EPOLLONESHOT;
	epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &event);
}

void *threadMain(void *instance)
{
	struct data *data = (struct data *)instance;
	int epollfd = data->epollfd;
	int sockfd = data->sockfd;
	pthread_t pid = pthread_self();	//pid or tid, confused

	printf("start new thread %u to recv data on fd: %d\n", pid, sockfd);
	char buf[BUFFER_SIZE];
	memset(buf, 0, sizeof(buf));

	for(;;) {
		int n = recv(sockfd, buf, BUFFER_SIZE-1, 0);
		if (n == 0) {
			close(sockfd);
			printf("foreiner closed the connection\n");
			break;
		}
		else if (n < 0) {
			if (errno == EAGAIN) {
				reset_oneshot(epollfd, sockfd);
				printf("EAGAIN. Read later\n");
				break;
			}
		}
		else {
			buf[n] = '\0';
			printf("thread %u get content: %s\n", pid, buf);
			printf("thread %u about to sleep\n", pid);
			sleep(5);
			printf("thread %u back from sleep\n", pid);
		}
	}

	printf("end thread %u receiving data on fd: %d\n", pid, sockfd);
}

int main(int argc, char **argv)
{
	if (argc <= 1) {
		printf("usage: %s port_number [ip_address]\n", basename(argv[0]));

	}

	int ret;
	int listenfd;
	int port = atoi(argv[1]);
	struct sockaddr_in address;
	bzero(&address, sizeof(address));
	address.sin_family = AF_INET;
	if (argc == 3) {
		const char *ip = argv[2];
		inet_pton(AF_INET, ip, &address.sin_addr);
	}
	else {
		address.sin_addr.s_addr = INADDR_ANY;
	}
	address.sin_port = htons(port);
	printf("port = %d\n", port);
	listenfd = socket(PF_INET, SOCK_STREAM, 0);
	assert(listenfd >= 0);

	ret = bind(listenfd, (struct sockaddr *)&address, sizeof(address));
	assert(ret != -1);

	ret = listen(listenfd, 5);
	assert(ret != -1);

	struct epoll_event events[MAX_EVENT_NUMBER];
	int epollfd = epoll_create(5);
	assert(epollfd != -1);

	/* 需要反复监听listenfd */
	add_fd(epollfd, listenfd, NOT_ONE_SHOT);

	while(1) {
		printf("1\n");
		int fd_number = epoll_wait(epollfd, events, MAX_EVENT_NUMBER, -1);	//设置为永久阻塞
		if (fd_number < 0) {
			perror("epoll wait failed\n");
			break;
		}
		printf("2\n");
		for (int i = 0; i < fd_number; i++) {
			int sockfd = events[i].data.fd;
			/* listenfd上有新事件，则创建新的fd，并加入监听队列 */
			if (sockfd == listenfd) {
				printf("new data incoming\n");
				struct sockaddr_in client_addr;
				socklen_t client_socklen = sizeof(client_addr);
				int connfd = accept(sockfd, (struct sockaddr*)&client_addr, &client_socklen);
				add_fd(epollfd, connfd, ONE_SHOT);
			}
			else if (events[i].events & EPOLLIN) {
				pthread_t tid;
				struct data instance;
				instance.epollfd = epollfd;
				instance.sockfd = sockfd;
				/* 一个连接对应一个线程，最好写成threadpool-BlockingQueue模式 */
				pthread_create(&tid, NULL, threadMain, (void *)&instance);
			}
			else {
				printf("something else happened\n");
			}
		}
	}

	close(listenfd);
	return 0;
}
