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
	if(argc != 2)
	{
		cout<<"Usage: "<<argv[0]<<" device_id"<<endl;
		return 1;
	}

	map<string, string> config = LoadConfig("../../config/config.ini");
	int sockfd, times=0, recvbyte;
	char sendline[4096] = "Hello World", buf[20];
	struct sockaddr_in servaddr, cliaddr;
	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("init socket error----");
		return 1;
	}
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(atoi(config["master_port"].c_str()));
	if(inet_pton(AF_INET, config["master_ip"].c_str(), &servaddr.sin_addr) <= 0)
	{
		perror("inet_pton error----");
		return 1;
	}
	if(connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1)
	{
		perror("connect error-----");
		return 1;
	}
	if(send(sockfd, "link", sizeof("link"), 0) < 0)
	{
		perror("request a server to control error:\t");
		return 1;
	}
	recvbyte = recv(sockfd, buf, sizeof(buf), 0);
	if(recvbyte <= 0)
	{
		perror("request error:\t");
		return 1;
	}
	buf[recvbyte] = '\0';
	close(sockfd);

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd == -1)
	{
		perror("socket error:\t");
		return 1;
	}

	char *ip = strtok(buf, "\t");
	char *port = strtok(NULL, "\t");
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
			cout<<"now send string"<<endl;
			if(send(sockfd, sendline, strlen(sendline), 0) < 0)
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
