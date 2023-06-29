/**
 * @brief socket 客户端
 * @author zhangzhiyu
 * @date 2022-11-26
 */

#include <stdio.h>
#include <sys/types.h> /* See NOTES */
#include <sys/socket.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
int main()
{
    int c_fd; // 客户侧socket句柄
    struct sockaddr_in c_addr; // 地址信息（ip + 端口）
    char readBuf[128];
    char *msg = (char*)"msg from client";

    // void *memset(void *s, int c, size_t n);
    memset(&c_addr, 0, sizeof(struct sockaddr_in));

    // 1.scoket
    // int socket(int domain, int type, int protocol);
    c_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (c_fd == -1) {
        perror("socket");
        exit(-1);
    }

    c_addr.sin_family = AF_INET;
    c_addr.sin_port = htons(1088); // htons： 主机字节序转网络字节序
    inet_aton("127.0.0.1", &c_addr.sin_addr); // 将char*格式十进制ip转换成二进制，并存放在struct in_addr中

    // 2.connect
    // int connect(int sockfd, const struct sockaddr *addr,socklen_t addrlen);
    if (connect(c_fd, (struct sockaddr *)&c_addr, sizeof(struct sockaddr)) == -1) {
        perror("connect");
        exit(-1);
    }

    // 3.write
    // ssize_t write(int fd, const void *buf, size_t count);
    write(c_fd, msg, 128);

    // 4.read
    // ssize_t read(int fd, void *buf, size_t count);
    int n_read = read(c_fd, readBuf, 128);
    if (n_read == -1) {
        perror("read");
    } else {
        printf("msg:%d,%s\n", n_read, readBuf);
    }

    return 0;
}

// msg:23,Server:msg from client