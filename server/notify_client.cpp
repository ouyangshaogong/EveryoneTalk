#include "notify_client.h"

int client_connect(string ip_address, int port)
{
    struct sockaddr_in servaddr;
    
    int sockfd = Socket(AF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    Inet_pton(AF_INET, ip_address.c_str(), &servaddr.sin_addr);

    Connect(sockfd, (SA *) &servaddr, sizeof(servaddr));
    syslog(LOG_ERR, "server BBBBBBBBBBBBBBBB Connect");
    return sockfd;
}

int deal_notify(string ip, int port, char *pNotify)
{
    int sockfd = client_connect(ip.c_str(), port);

    syslog(LOG_ERR, "ip:%s, port:%d, pNotify:%s\n", ip.c_str(), port, pNotify);
    if(writen(sockfd, pNotify, strlen(pNotify)) != strlen(pNotify)) {
        syslog(LOG_ERR, "write to socket fail.\n");
        return FAIL;
    }

    char pRet[30] = {0};
    if (Readline(sockfd, pRet, sizeof(pRet)) == 0) {
        return FAIL;
    }

    Close(sockfd);
    
    return OK;
}

