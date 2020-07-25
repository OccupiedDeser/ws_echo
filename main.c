/**************************************************************** 
    > Author: Deser
    > e-Mail: yongk202@163.com
    > Date: 2020-07-24 Fri 11:51:13
    > LastEditors: Deser
    > LastEditTime: 2020-07-25 Sat 18:06:18
 ****************************************************************/
#include "head.h"

void make_non_block(int fd)
{
    //设置非堵塞
    // if (fcntl(fd, F_SETFL, fcntl(fd, F_GETFL, 0) | O_NONBLOCK) == -1) {
    //     perror("fcntl");
    //     exit(EXIT_FAILURE);
    // }

    unsigned long ul = 1;
    ioctl(fd, FIONBIO, &ul);
}

int main()
{
    int listenfd;
    struct sockaddr_in server, client;
    bzero(&server, sizeof(server));
    bzero(&client, sizeof(client));
    int client_len = sizeof(client);

    // 创建套接字
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd < 0) {
        perror("socket()");
        exit(1);
    }
    int opt = 1;
    // setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    make_non_block(listenfd);

    // 绑定监听端口
    bzero(&server, sizeof(server));
    server.sin_port = htons(PORT);
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(listenfd, (struct sockaddr*)&server, sizeof(server)) < 0) {
        perror("bind()");
        exit(1);
    }

    if (listen(listenfd, 20) < 0) {
        perror("listen()");
        exit(1);
    }

    // int flag = 1;
    // bzero(&client, sizeof(client));
    // int newfd = accept(listenfd, (struct sockaddr*)&client, &client_len);
    // char buf[BUFLENGTH];
    // char data[BUFLENGTH];
    // char request[BUFLENGTH];
    // unsigned long long datalen, packlen;
    // while (1) {
    //     memset(buf, 0, BUFLENGTH);
    //     // int n = recv(newfd, buf, BUFLENGTH, 0);
    //     // if (n < 0) {
    //     //     perror("recv():");
    //     //     exit(1);
    //     // }
    //     if (flag) {
    //         // printf("request:\n%s\n", buf);
    //         // char key[GUIDLEN], request[BUFLENGTH];
    //         // getKey(buf, key);
    //         // parsestr(request, key);
    //         // int ret = send(newfd, request, BUFLENGTH, 0);
    //         // printf("reponse:\n%s\n", request);
    //         // if (ret < 0) {
    //         //     perror("send");
    //         //     exit(1);
    //         // }
    //         ShakeHand(newfd);
    //         flag = 0;
    //         continue;
    //     }
    //     // write(newfd, buf, BUFLENGTH);
    //     // datalen = ParsePack(buf, data);
    //     // if(!datalen){
    //     //     printf("文本过长\n");
    //     //     continue;
    //     // }
    //     // printf("data(%llu个字符): %s\n", datalen, data);
    //     // packlen = PackData(data, datalen, buf);
    //     // if(!packlen){
    //     //     printf("文本过长\n");
    //     //     continue;
    //     // }
    //     // send(newfd, buf, packlen, 0);
    //     // printf("%s\n", buf);
    //     ws_echo(newfd);
    // }

    int epollfd = epoll_create(MAX_CLIENT);
    if (epollfd < 0) {
        perror("epoll_create()");
        exit(1);
    }

    struct epoll_event ev, events[MAX_CLIENT];
    memset(&ev, 0, sizeof(ev));
    ev.data.fd = listenfd;
    ev.events = EPOLLIN;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, listenfd, &ev) < 0) {
        perror("epoll_ctl()");
        exit(1);
    }

    int fds[MAX_CLIENT] = { 0 };
    int nfds = -1;
    char buf[BUFLENGTH];
    char data[BUFLENGTH];
    unsigned long long datalen, packlen;

    while (1) {
        int nfds = epoll_wait(epollfd, events, MAX_CLIENT, -1);
        if (nfds < 0) {
            perror("epoll_wait()");
            exit(1);
        } else if (nfds == 0) {
            printf("waiting for connecting...\n");
            continue;
        }
        for (int i = 0; i < nfds; i++) {
            if (events[i].data.fd == listenfd) {
                int new_fd = accept(listenfd, (struct sockaddr*)&client, &client_len);
                if (new_fd < 0) {
                    perror("accept()");
                    exit(1);
                }
                // make_non_block(new_fd);
                if (!ShakeHand(new_fd)) {
                    continue;
                }
                printf("%s login\n", inet_ntoa(client.sin_addr));
                struct epoll_event new_ev;
                memset(&new_ev, 0, sizeof(new_ev));
                new_ev.data.fd = new_fd;
                new_ev.events = EPOLLIN;
                if (epoll_ctl(epollfd, EPOLL_CTL_ADD, new_fd, &new_ev) < 0) {
                    perror("epoll_ctl()");
                    exit(1);
                }
                // while (1)
                // {
                //     ws_echo(new_fd);
                // }
            } else {
                if (!ws_echo(events[i].data.fd)) {
                    struct epoll_event new_ev;
                    memset(&new_ev, 0, sizeof(new_ev));
                    new_ev.data.fd = events[i].data.fd;
                    new_ev.events = EPOLLIN;
                    if (epoll_ctl(epollfd, EPOLL_CTL_DEL, events[i].data.fd, &new_ev) < 0) {
                        perror("epoll_ctl()");
                        exit(1);
                    }
                    close(events[i].data.fd);
                    printf("logout\n");
                }
            }
        }
        // int new_fd = accept(listenfd, (struct sockaddr*)&client, &client_len);
        // if (new_fd > 0) {
        //     nfds++;
        //     fds[nfds] = new_fd;
        //     ShakeHand(new_fd);
        // }
        // for (int i = 0; i <= nfds; i++) {
        //     // ws_echo(fds[i]);
        //     memset(buf, 0, BUFLENGTH);
        //     int newfd = fds[i];
        //     int n = recv(newfd, buf, BUFLENGTH, 0);
        //     if (n < 0) {
        //         perror("recv():");
        //         exit(1);
        //     }
        //
        //     // write(newfd, buf, BUFLENGTH);
        //     datalen = ParsePack(buf, data);
        //     if (!datalen) {
        //         printf("文本过长\n");
        //         continue;
        //     }
        //     printf("data(%llu个字符): %s\n", datalen, data);
        //     packlen = PackData(data, datalen, buf);
        //     if (!packlen) {
        //         printf("文本过长\n");
        //         continue;
        //     }
        //     send(newfd, buf, packlen, 0);
        //     printf("%s\n", buf);
        // }
    }
    return 0;
}