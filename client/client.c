#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

#define PORT 8080

int check_if_file_existed(const char *filename);
int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        printf("[!] input command : ./client [ip address] [filename]\n");
        return 1;
    }
    struct sockaddr_in server;
    server.sin_addr.s_addr = inet_addr(argv[1]);
    server.sin_port = htons(PORT);
    server.sin_family = AF_INET;

    char request[100];
    char response[1024];
    char recv_buff[1024];

    memset(recv_buff, '0', sizeof(recv_buff));
    
    if (argc < 2)
    {
        printf("[-] Please input the argument!\n");
        return 1;
    }
    else
        strcpy(request, argv[2]);
    
    int sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sockfd < 0)
    {
        printf("[-] Socket creation error!\n");
        return 1;
    }
    else
        printf("[+] Socket creation success!\n");

    int conn = connect(sockfd, (struct sockaddr *)&server, sizeof(server));
    if (conn < 0)
    {
        printf("[-] Connection failed!\n");
    }
    else
        printf("[+] Successfully connected to %s:%d\n", inet_ntoa(server.sin_addr), ntohs(server.sin_port));

    send(sockfd, request, strlen(request), 0);

    int val_read = read(sockfd, response, 1024);
    printf("[*] Response : %s\n", response);

    if (strcmp(response, "File not found!") == 0) {
        printf("[-] %s\n", response);
        return 1;
    }

    int if_file_existed = check_if_file_existed(response);

    // if file already existed
    if(if_file_existed == 1)
    {
        while(1)
        {
            char choice[2];
            printf("[?] Want to overwrite? (y/n)");
            scanf("%s", choice);
            if (strcmp(choice, "y") == 0)
            {
                break;
            } else if (strcmp(choice, "n") == 0)
            {
                printf("[*] Request cancelled.\n");
                return 0;
            }
            else
            {
                printf("[!] Please input a correct arguement!\n");
            }
        }
    }

    FILE *fp = fopen(response, "ab");
    if (fp == NULL)
    {
        printf("[-] Error opening file!\n");
        return 1;
    }

    int byte_recv = 0;
    while((byte_recv = recv(sockfd, recv_buff, 1024, 0)) > 0)
    {
        fwrite(recv_buff, 1, byte_recv, fp);
    }

    if (byte_recv < 0)
    {
        printf("[-] Read error!\n");
        return 1;
    }

    fclose(fp);
    close(sockfd);

    return 0;
}

int check_if_file_existed(const char *filename)
{
    FILE *file;
    file = fopen(filename, "r");
    if (file != NULL)
    {
        fclose(file);
        return 1;
    }
    return 0;
}