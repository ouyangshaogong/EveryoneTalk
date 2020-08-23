#include "connect_server.h"

extern int g_sockfd;
extern char pname[MAXLINE];



int client_connect(string ip_address, int port)
{
    struct sockaddr_in servaddr;
    
    g_sockfd = Socket(AF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    Inet_pton(AF_INET, ip_address.c_str(), &servaddr.sin_addr);
    syslog(LOG_ERR, "client BBBBBBBBBBBBBBBB Connect");
    return Connect(g_sockfd, (SA *)&servaddr, sizeof(servaddr));
}

    


int deal_msg(char *pMsg, char *pRet, int nlength)
{
    if(writen(g_sockfd, pMsg, strlen(pMsg)) != strlen(pMsg)) 
    {
        syslog(LOG_ERR, "deal_msg>>writen fail");
        return FAIL;
    }

    if (Readline(g_sockfd, pRet, nlength) == 0)
    {
        syslog(LOG_ERR, "deal_msg>>Readline fail");
        return FAIL;
    }

    return OK;
}
