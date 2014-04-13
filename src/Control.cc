#include <cstdio>
#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <error.h>
#include <string.h>
#include <stdlib.h>
#include <map>
#include <sstream>
#include "MysqlCDriver.h"
#include "OperateChar.h"
using namespace std;

map<string, int> available_nodes;
map<string, string> config_items;
int temp_sock;

string IntToString(int tmp)
{
	stringstream ss;
	string s;
	ss << tmp;
	ss >> s;
	return s;
}

//function: get the nodes register message and add node to "available_nodes" list
int RegisterRequest(int sockfd, char *ip)
{
	char buf[20];
	int recv_bytes;
	recv_bytes = recv(sockfd, buf, sizeof(buf), 0);
	if(recv_bytes == 0)
		return 0;
	if(recv_bytes < 0)
	{
		perror("recieve bytes error:\t");
		return 0;
	}

	if(strcmp(ip, "127.0.0.1") == 0)
	{
		char buf[16];
		GetIp(buf, "eth0");
		strcpy(ip, buf);
	}
	buf[recv_bytes] = '\0';
	string key = string(ip) + '\t' + string(buf);

	available_nodes[key] = 0;
	cout<<"a new node registe:\t"<<available_nodes[key]<<endl;
	return 1;
}

//function: get the nodes offline message and delete it from available_nodes list
int OfflineRequest(int sockfd, char *ip)
{
	char buf[20];
	int recv_bytes;
	if(send(sockfd, "OK", sizeof("OK"), 0) < 0)
	{
		perror("");
		return 0;
	}
	recv_bytes = recv(sockfd, buf, sizeof(buf), 0);
	if(recv_bytes == 0)
		return 0;
	if(recv_bytes < 0)
	{
		perror("recieve bytes error:\t");
		return 0;
	}

	if(strcmp(ip, "127.0.0.1") == 0)
	{
		char buf[16];
		GetIp(buf, "eth0");
		strcpy(ip, buf);
	}
	buf[recv_bytes] = '\0';
	string key = string(ip) + '\t' + string(buf);
	cout<<RED"offline a node:"<<key<<NONE<<endl;

	map<string, int>::iterator ite = available_nodes.begin();
	for(; ite!=available_nodes.end(); ite++)
		if(ite->first.compare(key) == 0)
		{
			available_nodes.erase(ite++);
			break;
		}
	return 1;
}

//function: get the client connect and assign a server to this client
int LinkRequest(int sockfd)
{
	int recv_bytes;
	char buf[20];
	map<string, int>::iterator min = available_nodes.begin(), ite;

	if(available_nodes.empty())
	{
		send(sockfd, "no register client", sizeof("no register client"), 0);
		close(sockfd);
		return 0;
	}
	
	for(ite=available_nodes.begin(); ite!=available_nodes.end(); ite++)
		if(min->second > ite->second)
			min = ite;

	cout<<min->first.c_str()<<endl;
	//int npos = min->first.find("\t");
	//string ip = min->first.substr(0, npos);
	//string port = min->first.substr(npos+1; min->first.length()-npos-1);
	int ret = send(sockfd, min->first.c_str(), min->first.length(), 0);
	if(ret < 0)
	{
		perror("send server message to client error:\t");
		return 0;
	}
	return 1;
}

//function: a client is closed to node
int GetLoadRequest(int sockfd, char *ip)
{
	int recv_bytes;
	char buf[20];
	if(send(sockfd, "OK", sizeof("OK"), 0) < 0)
		return 0;
	recv_bytes = recv(sockfd, buf, sizeof(buf), 0);
	if(recv_bytes <= 0)
	{
		perror("");
		return 0;
	}
	buf[recv_bytes] = '\0';
	char *num = strtok(buf, "\t");
	char *port = strtok(NULL, "\t");

	string temp = string(ip) + "\t" + string(port);
	available_nodes[temp] = atoi(num);
	return 0;
}

//thread: master and slive sync
void *MasterSliveSync(void *argc)
{
	int sync_sock = *(int*)argc, recv_bytes;
	char buf[50];
	//this is a master
	map<string, int>::iterator ite;
	while(1)
	{
		if(send(sync_sock, "start", sizeof("start"), 0) < 0)
		{
			perror("start send sync message:\t");
			close(sync_sock);
			pthread_exit(NULL);
		}

		recv_bytes = recv(sync_sock, buf, sizeof(buf), 0);
		if(recv_bytes <= 0)
		{
			perror("");
			sleep(5);
			continue;
		}
		buf[recv_bytes] = '\0';
		if(strcmp(buf, "OK") != 0)
		{
			cout<<RED<<"not get OK from slive, get "<<buf<<" instead"<<NONE<<endl;
			sleep(5);
			continue;
		}

		cout<<GREEN<<"start send sync message to slive"<<NONE<<endl;

		for(ite=available_nodes.begin(); ite!=available_nodes.end(); ite++)
		{
			string temp = ite->first+"\t"+string(IntToString(ite->second));
			int re_val = send(sync_sock, temp.c_str(), temp.length(), 0);
			if(re_val < 0)
			{
				perror("send sync_message error:\t");
				close(sync_sock);
				pthread_exit(NULL);
			}
		}
		if(send(sync_sock, "end", sizeof("end"), 0) < 0)
		{
			perror("end send sync message:\t");
			close(sync_sock);
			pthread_exit(NULL);
		}
		sleep(10);
	}
}

void *SliveGetSync(void *argc)
{
	//this is a slave
	int sync_sock = temp_sock, recv_bytes;
	char buf[50];

	while(1)
	{
		if(config_items["master"].compare("1") == 0)
		{
			cout<<RED<<"mode changed"<<NONE<<endl;
			pthread_exit(NULL);
		}
		while(1)
		{
			recv_bytes = recv(sync_sock, buf, sizeof(buf), 0);
			if(recv_bytes == 0)
			{
				close(sync_sock);
				pthread_exit(NULL);
			}
			else if(recv_bytes < 0)
			{
				perror("recv data error:\t");
				close(sync_sock);
				pthread_exit(NULL);
			}
			buf[recv_bytes] = '\0';
			if(strcmp(buf, "start") == 0)
			{
				available_nodes.clear();
				if(send(sync_sock, "OK", sizeof("OK"), 0) < 0)
				{
					perror("send OK error line 221");
					break;
				}
				cout<<GREEN<<"a new sync start"<<NONE<<endl;
				continue;
			}
			else if(strcmp(buf, "end") == 0)
			{
				cout<<GREEN<<"sync finish"<<NONE<<endl;
				break;
			}
			string recv_chars = string(buf);
			int npos = recv_chars.rfind("\t");
			string val = recv_chars.substr(0, npos);
			string num = recv_chars.substr(npos+1, recv_bytes-npos-1);
			available_nodes[val] = atoi(num.c_str());
			cout<<GREEN<<"get a new client"<<val<<":"<<num<<NONE<<endl;
		}
		sleep(10);
	}
}

int AddSliveMode()
{
	cout<<GREEN<<"this is a slive node"<<NONE<<endl;
	temp_sock = socket(AF_INET, SOCK_STREAM, 0);
	pthread_t temp_pid;
	if(temp_sock < 0)
		return 0;
	struct sockaddr_in temp_addr;
	temp_addr.sin_port = htons(atoi(config_items["master_port"].c_str()));
	temp_addr.sin_family = AF_INET;
	if(inet_pton(AF_INET, config_items["master_ip"].c_str(), &temp_addr.sin_addr) <= 0)
		return 0;
	if(connect(temp_sock, (struct sockaddr*)&temp_addr, sizeof(temp_addr)) < 0)
		return 0;
	if(send(temp_sock, "slive", sizeof("slive"), 0) < 0)
		return 0;
	pthread_create(&temp_pid, NULL, SliveGetSync, NULL);
	cout<<YELLOW<<"slive thread create finish"<<NONE<<endl;
	return 1;
}

int main(int argc, char* argv[])
{
	if(argc != 2)
	{
		cout<<"Usage: "<<argv[0]<<" config_file_path"<<endl;
		return 1;
	}

	config_items = LoadConfig(argv[1]);
	if(config_items.empty())
		return 1;

	if(config_items["master"].compare("0") == 0)
	{
		if(AddSliveMode() == 0)
		{
			perror("");
			return 1;
		}
	}

	char buf[50];
	struct sockaddr_in server_addr, client_addr;
	int server_fd, client_fd, re_val, recv_bytes;
	pthread_t pid;
	socklen_t addr_len = sizeof(server_addr);
	int old_time = 0, new_time = 0;

	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(server_fd <= 0)
	{
		perror("init socket error:\t");
		return 1;
	}

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(9000);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	if(bind(server_fd, (struct sockaddr*)&server_addr, addr_len) == -1)
	{
		perror("bind socket error:\t");
		return 2;
	}

	if(listen(server_fd, 10) == -1)
	{
		perror("listen socket error:\t");
		return 3;
	}

	while(client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &addr_len))
	{
		getpeername(client_fd, (struct sockaddr*)&client_addr, &addr_len);
		cout<<"accept a client:\t"<<inet_ntoa(client_addr.sin_addr)<<endl;

		config_items = LoadConfig(argv[1]);

		recv_bytes = recv(client_fd, buf, sizeof(buf), 0);
		buf[recv_bytes] = '\0';
		cout<<"get message..."<<buf<<"\t"<<recv_bytes<<endl;
		if(strcmp(buf, "register") == 0)
		{
			if(RegisterRequest(client_fd, inet_ntoa(client_addr.sin_addr)) == 0)
			{
				if(send(client_fd, "Error", sizeof("Error"), 0) < 0)
					perror("send error message to client:\t");
			}
			else
			{
				if(send(client_fd, "OK", sizeof("OK"), 0) < 0)
					perror("send OK message to client:\t");
			}
			close(client_fd);
		}
		else if(strcmp(buf, "link") == 0)
		{
			if(LinkRequest(client_fd) == 0)
			{
				if(send(client_fd, "Error", sizeof("Error"), 0) < 0)
					perror("send error message to client:\t");
			}
			else
			{
				if(send(client_fd, "OK", sizeof("OK"), 0) < 0)
					perror("send OK message to client:\t");
			}
			close(client_fd);
		}
		else if(strcmp(buf, "offline") == 0)
		{
			if(OfflineRequest(client_fd, inet_ntoa(client_addr.sin_addr)) == 0)
			{
				if(send(client_fd, "Error", sizeof("Error"), 0) < 0)
					perror("send error message to client:\t");
			}
			else
			{
				if(send(client_fd, "OK", sizeof("OK"), 0) < 0)
					perror("send OK message to client:\t");
			}
			close(client_fd);
		}
		else if(strcmp(buf, "load") == 0)
		{
			if(GetLoadRequest(client_fd, inet_ntoa(client_addr.sin_addr)) == 0)
			{
				if(send(client_fd, "Error", sizeof("Error"), 0) < 0)
					perror("send error message to client:\t");
			}
			else
			{
				if(send(client_fd, "OK", sizeof("OK"), 0) < 0)
					perror("send OK message to client:\t");
			}
		}
		else if(strcmp(buf, "slive") == 0)
		{
			pthread_create(&pid, NULL, MasterSliveSync, &client_fd);
			cout<<YELLOW<<"create master thread finish"<<NONE<<endl;
		}
		cout<<YELLOW<<"++++++load average+++++++"<<endl;
		cout<<"server\t\tport\tloads"<<endl;
		map<string, int>::iterator ite;
		for(ite=available_nodes.begin(); ite!=available_nodes.end(); ite++)
			cout<<ite->first<<"\t"<<ite->second<<endl;
		cout<<NONE<<endl;
	}

	close(server_fd);
	close(client_fd);
	return 0;
}
