#include "testcommon.h"
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

int main()
{
    if (access(SOCKET_PATH, F_OK) == 0) {
        remove(SOCKET_PATH);
    }
    int fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd == -1) {
        printf("create socket fail, error: %d, %s\n", errno, strerror(errno));
        return 0;
    }
    struct sockaddr_un socketAddr;
    socketAddr.sun_family = AF_UNIX;
    memset(socketAddr.sun_path, 0, sizeof(socketAddr.sun_path));
    strncpy(socketAddr.sun_path, SOCKET_PATH, sizeof(SOCKET_PATH));
    socketAddr.sun_path[strlen(SOCKET_PATH)] = 0;
    if (bind(fd, (struct sockaddr*)&socketAddr, sizeof(socketAddr)) == -1) {
        printf("bind fail, error: %d, %s\n", errno, strerror(errno));
        close(fd);
        return 0;
    }
    if (listen(fd, 10) == -1) {
        printf("listen fail, error: %d, %s\n", errno, strerror(errno));
        close(fd);
        remove(SOCKET_PATH);
        return 0;
    }
    int clientFd = accept(fd, NULL, NULL);
    if (clientFd == -1) {
        printf("accept fail, error: %d, %s\n", errno, strerror(errno));
        close(fd);
        remove(SOCKET_PATH);
        return 0;
    }
    int epollFd = epoll_create1(EPOLL_CLOEXEC);
    if (epollFd == -1) {
        printf("create epoll fd fail, error: %d, %s\n", errno, strerror(errno));
        close(clientFd);
        close(fd);
        remove(SOCKET_PATH);
        return 0;
    }
    epoll_event ev;
    ev.data.fd = clientFd;
    ev.events = EPOLLIN;
    if (epoll_ctl(epollFd, EPOLL_CTL_ADD, clientFd, &ev) == -1) {
        printf("epoll_ctl EPOLL_CTL_ADD fail, error: %d, %s\n", errno, strerror(errno));
        close(clientFd);
        close(fd);
        remove(SOCKET_PATH);
        return 0;
    }
    epoll_event recEvt;
    int res = epoll_wait(epollFd, &recEvt, 1, -1);
    if (res == -1) {
        printf("epoll_wait fail, error: %d, %s\n", errno, strerror(errno));
        close(clientFd);
        close(fd);
        remove(SOCKET_PATH);
        return 0;
    }
    char data[1024] = {0};
    read(clientFd, data, sizeof(data));
    printf("data: %s\n", data);
    close(clientFd);
    close(fd);
    remove(SOCKET_PATH);
    return 0;
}
