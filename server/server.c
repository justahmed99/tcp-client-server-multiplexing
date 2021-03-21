#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>

#define PORT 8080

int main(int argc, char *argv[])
{
    int connfd = 0;
    char request[100];
    char *fname;

    struct sockaddr_in address;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(PORT);
    address.sin_family = AF_INET;

    int address_len = sizeof(address);

    // socket()
    int sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sockfd < 0)
    {
        printf("[-] Socket creation error!\n");
        return 1;
    }
    else
        printf("[+] Socket creation success!\n");

    // bind()
    int server_bind = bind(sockfd, (struct sockaddr *)&address, sizeof(address));
    if (server_bind)
    {
        printf("[-] Server binding failed!\n");
        return 1;
    }
    else
        printf("[+] Server binding success!\n");

    // listen()
    int listening = listen(sockfd, 5);
    if (listening < 0)
    {
        printf("[-] Failed to listen!\n");
        return 1;
    }

    while (1)
    {
        printf("Waiting ...\n");
        // accept()
        connfd = accept(sockfd, (struct sockaddr *)&address, (socklen_t *)&address_len);
        if (connfd < 0)
        {
            printf("[-] Error in accept!\n");
            continue;
        }

        int val_read = read(connfd, request, 1024);
        int choice = atoi(request);
        int not_found = 0;
        printf("[*] Request number: %d\n", choice);
        switch (choice)
        {
            case 1:
            {
                printf("[*] File 1 is requested!\n");
                fname = "1.jpg";
                break;
            }
            case 2:
            {
                printf("[*] File 2 is requested!\n");
                fname = "2.jpg";
                break;
            }

            default:
            {
                not_found = 1;
                printf("[-] File not found!\n");
                send(connfd, "File not found!", strlen("File not found!"), 0);
                break;
            }
        }

        if (not_found == 0)
        {
            FILE *fp = fopen(fname, "rb");
            if (fp == NULL)
            {
                printf("[-] Error opening file!\n");
            }
            send(connfd, fname, strlen(fname), 0);
            while (1)
            {
                unsigned char buff[1024] = {0};
                int nread = fread(buff, 1, 1024, fp);
                printf("%d bytes\n", nread);

                if (nread > 0)
                {
                    printf("sending\n");
                    write(connfd, buff, nread);
                }
                if (nread < 1024)
                {
                    if (feof(fp))
                        printf("EOF\n");

                    if (ferror(fp))
                        printf("Error loading");

                    break;
                }
            }
            fclose(fp);
            close(connfd);
        }
    }

    return 0;
}