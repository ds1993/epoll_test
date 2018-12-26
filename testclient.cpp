#include "testcommon.h"
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

int main()
{
    int fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd == -1) {
        printf("create socket fail, error: %d, %s\n", errno, strerror(errno));
        return 0;
    }
    struct sockaddr_un socketAddr;
    socketAddr.sun_family = AF_UNIX;
    memset(socketAddr.sun_path, 0, sizeof(socketAddr.sun_path));
    strncpy(socketAddr.sun_path, SOCKET_PATH, sizeof(SOCKET_PATH));
    int res = connect(fd, (struct sockaddr*)&socketAddr, sizeof(socketAddr));
    if (res == -1) {
        printf("connect fail, error: %d, %s\n", errno, strerror(errno));
        close(fd);
        return 0;
    }
    send(fd, "fuck!", sizeof("fuck!"), 0);
    close(fd);
    return 0;
}
