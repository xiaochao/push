#include <stdio.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <iostream>
#include <cstdlib>
#include "MysqlCDriver.h"
#include <map>

using namespace std;

int main(int argc, char* argv[])
{
	if(argc != 3)
	{
		cout<<"Usage: "<<argv[0]<<" device_id server_ip"<<endl;
		return 1;
	}

	map<string, string> config = LoadConfig("../../config/config.ini");
	int sockfd, times=0, recvbyte;
	char sendline[4096] = "Hello World", buf[20];
	struct sockaddr_in servaddr, cliaddr;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd == -1)
	{
		perror("socket error:\t");
		return 1;
	}

	char ip[20];
	strcpy(ip, argv[2]);
	char port[] = "6000";
	cout<<"the usable node ip and port is:  "<<ip<<":"<<port<<endl;

	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(atoi(port));
	if(inet_pton(AF_INET, ip, &servaddr.sin_addr) <= 0)
	{
		perror("inet_pton error----");
		return 1;
	}
	if(connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1)
	{
		perror("connect error-----");
		return 1;
	}

	while(1)
	{
		if(times < 1)
		{
			strcpy(sendline, "READY\t");
			strcat(sendline, argv[1]);
			cout<<"now send string"<<sendline<<endl;
			if(send(sockfd, sendline, sizeof(sendline), 0) < 0)
			{
				perror("send error----");
				return 1;
			}
			times++;
		}
		else
		{
			recvbyte = recv(sockfd, sendline, sizeof(sendline), 0);
			if(recvbyte == 0)
			{
				printf("connection breakout\n");
				break;
			}
			else if(recvbyte < 0)
			{
				perror("recv data error-->");
				break;
			}
			sendline[recvbyte] = '\0';
			cout<<"recv data from server-->"<<sendline<<endl;
			if(strcmp(sendline, "2") == 0)
			{
				if(send(sockfd, "Alive", sizeof("Alive"), 0) < 0)
				{
					perror("send alive msg error");
					return 0;
				}
			}
			else if(strcmp(sendline, "1") == 0)
				times--;
		}
	}
	close(sockfd);
	return 0;
}
