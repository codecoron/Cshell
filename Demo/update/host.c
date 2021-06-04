#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include<sys/socket.h>
#include <arpa/inet.h>
void error_handling(char *message);
int main(int argc, char *argv[])
{
    char hostname[512];

    if (gethostname(hostname, sizeof(hostname)) != 0)
        error_handling("gethostename() error\n");

    printf("Local hostname :%s\n", hostname);

    struct hostent *pHostent;
    pHostent = gethostbyname(hostname);
    if (pHostent == NULL)
        error_handling("gethostbyname() error\n");

    printf("name:%s\n", pHostent->h_name);
    printf("aliases:%s\n", *(pHostent->h_aliases));

    struct sockaddr_in addr;
    for (int i = 0; pHostent->h_addr_list[i]; i++)
    {
        memcpy(&addr.sin_addr.s_addr, pHostent->h_addr_list[i], pHostent->h_length);
        printf("Address:%s\n", inet_ntoa(addr.sin_addr));
    }
    return 0;
}

void error_handling(char *message)
{
    printf("%s", message);
    exit(0);
}
