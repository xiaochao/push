#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <errno.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>
#include "MysqlCDriver.h"
#include "OperateChar.h"
#include <vector>
#include <map>
#include <iostream>
#include <pthread.h>
#include <signal.h>
#include <list>
#include "json/json.h"
#include <arpa/inet.h>
using namespace std;

vector<string> all;
list<string> connectlog, sendlog;
map<string, string> config;
string control_ip = config["master_ip"], control_port = config["master_port"];
char ip[20], user[20], pass[20], dbname[20];
MYSQL *connectcons, *recordcons, *conn, *schedulecons;
vector<int> allsockets;
pthread_mutex_t mutex;
int load_average = 0;

int OpenSock();

//detail the Ctrl+C signal
//close mysql connection and all opened socket 
void MyClose(int sig)
{
	if(sig == SIGINT)
	{
		mysql_close(conn);
		mysql_close(connectcons);
		mysql_close(recordcons);
		mysql_close(schedulecons);
		vector<int>::iterator ite;
		for(ite=allsockets.begin(); ite!=allsockets.end(); ite++)
			close(*ite);

		int sockfd = OpenSock(), recv_bytes;
		char buf[10];
		if(sockfd != 0)
		{
			if(send(sockfd, "offline", sizeof("offline"), 0) < 0)
				perror("");
			recv_bytes=recv(sockfd, buf, sizeof(buf), 0);
			if(recv_bytes < 0)
				perror("");
			buf[recv_bytes] = '\0';
			if(strcmp(buf, "OK") != 0)
				cout<<RED<<"offline failed"<<NONE<<endl;
			else
				cout<<YELLOW<<"you can offline"<<NONE<<endl;
			if(send(sockfd, "6000", sizeof("6000"), 0) < 0)
				perror("");
			close(sockfd);
		}
		exit(0);
	}
}

//function: send "client unlink" message to control
int SendLoadAverage(int Add)
{
	int sockfd = OpenSock(), recv_bytes;
	char buf[20];
	while(sockfd <= 0)
	{
		sleep(1);
		sockfd = OpenSock();
	}
	if(send(sockfd, "load", sizeof("load"), 0) < 0)
		perror("");
	recv_bytes = recv(sockfd, buf, sizeof(buf), 0);
	if(recv_bytes <= 0)
		perror("");
	buf[recv_bytes] = '\0';
	if(strcmp(buf, "OK") != 0)
		cout<<RED<<"send load failed"<<NONE<<endl;
	else
		cout<<YELLOW<<"a client load"<<NONE<<endl;
	if(Add)
		load_average += 1;
	else
		load_average -= 1;
	strcpy(buf, "");
	itoa(load_average, buf);
	strcat(buf, "\t6000");
	if(send(sockfd, buf, sizeof(buf), 0) < 0)
		perror("");
	close(sockfd);
	return 1;
}

//thread: write the connect msg to mysql
void *WriteConnectMsg(void *arg)
{
	mysql_set_character_set(connectcons, "utf8");
	while(1)
	{
		sleep(1);
		list<string>::iterator ite = connectlog.begin();
		while(!connectlog.empty())
		{
			string statement = *ite;
			if(mysql_query(connectcons, statement.c_str()))
			{
				cout<<"mysql: "<<statement<<" run failed"<<endl;
				break;
			}
			ite++;
			connectlog.pop_front();
		}
	}
}
//thread: write the send msg to mysql
void *WriteSendMsg(void *arg)
{
	mysql_set_character_set(recordcons, "utf8");
	while(1)
	{
		sleep(1);
		list<string>::iterator ite = sendlog.begin();
		while(!sendlog.empty())
		{
			string statement = *ite;
			if(mysql_query(recordcons, statement.c_str()))
			{
				cout<<"mysql: "<<statement<<" run failed"<<endl;
				break;
			}
			ite++;
			sendlog.pop_front();
		}
	}
}

//thread: heartbeat to control and check control is alive
void *HeartBeat(void *arg)
{
	int heart_sock, recv_bytes;
	char buf[50];
	struct sockaddr_in heart_addr;
	while(1)
	{
		sleep(9);
		cout<<PURPLE<<"is is heart beat"<<NONE<<endl;
		close(heart_sock);
		heart_sock = socket(AF_INET, SOCK_STREAM, 0);
		if(heart_sock < 0)
			continue;
		heart_addr.sin_family = AF_INET;
		heart_addr.sin_port = htons(atoi(control_port.c_str()));
		if(inet_pton(AF_INET, control_ip.c_str(), &heart_addr.sin_addr) <= 0)
			continue;
		int times = 3;
		while(times)
		{
			if(connect(heart_sock, (struct sockaddr*)&heart_addr, sizeof(heart_addr)) == 0)
			{
				if(send(heart_sock, "beat", sizeof("beat"), 0) < 0)
					perror("send heart beat error");
				break;
			}
			times--;
			sleep(3);
		}
		if(times != 0)
			continue;

		cout<<RED<<"the master control down"<<NONE<<endl;
		close(heart_sock);
		int slive = socket(AF_INET, SOCK_STREAM, 0);
		control_ip = config["slive_ip"];
		control_port = config["slive_port"];
		memset(&heart_addr, 0, sizeof(heart_addr));
		heart_addr.sin_port = htons(atoi(control_port.c_str()));
		heart_addr.sin_family = AF_INET;
		if(inet_pton(AF_INET, control_ip.c_str(), &heart_addr.sin_addr) <= 0)
			continue;
		times = 3;
		cout<<control_ip<<"\t"<<control_port<<endl;
		while(times)
		{
			if(connect(slive, (struct sockaddr*)&heart_addr, sizeof(heart_addr)) == 0)
			{
				if(send(slive, "beat", sizeof("beat"), 0) < 0)
					perror("send heart beat error");
				break;
			}
			times--;
			sleep(3);
		}
		if(times == 0)
			cout<<RED<<"the slive control down"<<NONE<<endl;
		close(slive);
	}
}

//function: get the sended message's max id with device_id
int GetMaxSendId(char* deid)
{
	char select[100] = "select max(message_id) from record where device_id='";
	int reval;
	strcat(select, deid);
	strcat(select, "'");
	cout<<select<<endl;
	int ret = pthread_mutex_lock(&mutex);
	if(ret != 0)
		perror("lock failed");
	if(mysql_query(schedulecons, select))
	{
		cout<<"search database error"<<endl;
		reval = -1;
	}
	
	MYSQL_RES *result = mysql_store_result(schedulecons);
	MYSQL_ROW rows;
	rows = mysql_fetch_row(result);
	if(rows[0] != NULL)
		reval = atoi(rows[0])+1;
	else
		reval = 0;
	mysql_free_result(result);
	ret = pthread_mutex_unlock(&mutex);
	if(ret != 0)
		perror("unlock failed");
	return reval;
}

//thread: sync message from mysql
void *SyncMessge(void *arg)
{
	conn = Connect(ip, user, pass, dbname);
	mysql_set_character_set(conn, "utf8");
	char id[10] = "0";
	while(1)
	{
		sleep(1);
		char select[50] = "select * from message where id>";
		strcat(select, id);
		int res = mysql_query(conn, select);
		if(res)
		{
			perror("select mysql error");
			cout<<CR_OUT_OF_MEMORY<<endl;
			continue;
		}
		MYSQL_RES *result = mysql_store_result(conn);
		if(!result)
		{
			perror("store result error");
			continue;
		}
		int i=0;
		MYSQL_FIELD *fd;
		MYSQL_ROW sql_row;
		char buf[300];
		vector<char*> rows;
		int num = mysql_num_fields(result);
		for(; fd=mysql_fetch_field(result); i++)
		{
			bzero(buf, sizeof(buf));
			strcpy(buf, fd->name);
			rows.push_back(buf);
		}
		while(sql_row=mysql_fetch_row(result))
		{
			Json::Value msg;
			msg["title"] = sql_row[1];
			msg["content"] = sql_row[2];
			strcpy(id, sql_row[0]);
			all.push_back(msg.toStyledString());
		}

		mysql_free_result(result);
	}
}

//thread:hold a socket from client and send message to client
//at the same time, record the send log to mysql

void *Hold(void *arg)
{
	int clientfd = *(int*)arg, msgid = 0, recvbyte;
	char buf[200], temp[200], deviceid[20]="0";
	struct sockaddr_in device;
	socklen_t len = sizeof(device);
	int times = 3, circle = 0;
	cout<<"step in thread......"<<endl;

	//connect init:get device message from client send message 
	//if cannot get right message from client three times, exit
	while(times--)
	{
		cout<<"prepare recv client init string"<<endl;
		recvbyte = recv(clientfd, buf, sizeof(buf), 0);
		if(recvbyte <= 0)
		{
			perror("recv message from client error");
			continue;
		}

		SplitChar(buf, temp, 1);
		temp[recvbyte] = '\0';
		cout<<temp<<"\t"<<recvbyte<<endl;
		if(strcmp(temp, "READY") != 0)
		{
			cout<<"init send string error"<<endl;
			if(send(clientfd, "1", sizeof("1"), 0) < 0)
			{
				perror("send init again message error:exit");
				close(clientfd);
				pthread_exit(NULL);
			}
			continue;
		}
		cout<<"get the right init string"<<buf<<endl;
		SplitChar(buf, deviceid, 2);
		break;
	}
	if(times <= 0)
	{
		cout<<"init three error,exit"<<endl;
		close(clientfd);
		pthread_exit(NULL);
	}

	msgid = GetMaxSendId(deviceid);
	cout<<"max send id is:"<<msgid<<endl;
	char insert[100] = "insert into connect(device_id, status) values('";
	strcat(insert, deviceid);
	strcat(insert, "', 'connected')");
	cout<<"the insert string is"<<insert<<endl;
	connectlog.push_back(string(insert));
	/*if(mysql_query(connectcons, insert))
	{
		perror("insert connect record to mysql error, exit");
		close(clientfd);
		pthread_exit(NULL);
	}*/

	while(1)
	{
		sleep(1);
		while(msgid < all.size() && all[msgid].length() != 0)
		{
			//send message
			cout<<"send msg --> "<<all[msgid]<<endl;
			if(send(clientfd, all[msgid].c_str(), all[msgid].length(), 0) < 0)
			{
				strcpy(insert, "insert into record(device_id, message_id, status) values('");
				strcat(insert, deviceid); strcat(insert, "', ");
				char temp[10];
				sprintf(temp, "%d", msgid);
				strcat(insert, temp); strcat(insert, ", 'failed'");
				sendlog.push_back(string(insert));
				//if(mysql_query(connectcons, insert))
				//	perror("write failed msg to mysql error");
				perror("send message error");
				sleep(1);
				continue;
			}
			strcpy(insert, "");
			strcat(insert, "insert into record(device_id, message_id, status) values('");
			strcat(insert, deviceid);
			strcat(insert, "', ");
			char temp[10];
			sprintf(temp, "%d", msgid);
			strcat(insert, temp);
			strcat(insert, ", 'sended')");
			sendlog.push_back(string(insert));
			//if(mysql_query(connectcons, insert))
			//	perror("record send records error");
			msgid++;
			circle = 0;
		}

		//send keepalive if no activities in 15mins
		if(circle == 20)
		{
			if(send(clientfd, "2", sizeof("2"), 0) < 0)
			{
				cout<<"lost client connection"<<endl;
				SendLoadAverage(false);
				close(clientfd);
				pthread_exit(NULL);
			}
			recvbyte = recv(clientfd, buf, sizeof(buf), 0);
			buf[recvbyte] = '\0';
			if(recv<=0 || strcmp(buf, "Alive") != 0)
			{
				cout<<"lost client connection"<<endl;
				SendLoadAverage(false);
				close(clientfd);
				pthread_exit(NULL);
			}
			cout<<"clinet is active"<<endl;
			circle = 0;
		}
		circle++;
	}
	close(clientfd);
}

int OpenSock()
{
	int sockfd, recv_bytes;
	char buf[50];
	struct sockaddr_in client_addr;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd <= 0)
	{
		perror("socket function error:\t");
		return 0;
	}

	client_addr.sin_port = htons(atoi(control_port.c_str()));
	client_addr.sin_family = AF_INET;
	if(inet_pton(AF_INET, control_ip.c_str(), &client_addr.sin_addr) <= 0)
	{
		perror("inet_pton funciton error:\t");
		return 0;
	}

	if(connect(sockfd, (struct sockaddr*)&client_addr, sizeof(client_addr)) == -1)
	{
		close(sockfd);
		perror("connect error:\t");
		return 0;
	}
	return sockfd;
}

bool Init()
{
	int recv_bytes, sockfd = OpenSock();
	char buf[10];
	cout<<"start registe to control"<<endl;
	if(sockfd == 0)
		return false;

	if(send(sockfd, "register", sizeof("register"), 0) < 0)
	{
		close(sockfd);
		perror("send error:\t");
		return false;
	}

	if(send(sockfd, "6000", sizeof("6000"), 0) < 0)
	{
		close(sockfd);
		perror("send error:\t");
		return false;
	}

	recv_bytes = recv(sockfd, buf, sizeof(buf), 0);
	cout<<"get registe message from server:\t"<<recv_bytes<<endl;
	if(recv_bytes <= 0)
	{
		close(sockfd);
		perror("register failed:\t");
		return false;
	}
	buf[recv_bytes] = '\0';
	if(strcmp(buf, "Error") == 0)
	{
		close(sockfd);
		cout<<"register failed"<<endl;
		return false;
	}
	close(sockfd);
	cout<<"registe to control over"<<endl;

	return true;
}

int main(int argc, char* argv[])
{
	if(argc != 2)
	{
		cout<<"Usage: "<<argv[0]<<" config_path"<<endl;
		return 1;
	}

	config = LoadConfig(argv[1]);
	if(config.empty())
	{
		cout<<"error config file"<<endl;
		return 1;
	}

	strcpy(ip, config["ip"].c_str());
	strcpy(user, config["user"].c_str());
	strcpy(pass, config["password"].c_str());
	strcpy(dbname, config["dbname"].c_str());
	if(strlen(user) <= 0)
	{
		cout<<"配置文件格式错误"<<endl;
		return 1;
	}

	Init();

	signal(SIGINT, MyClose);

	connectcons = Connect(ip, user, pass, dbname);
	recordcons = Connect(ip, user, pass, dbname);
	schedulecons = Connect(ip, user, pass, dbname);
	mysql_set_character_set(connectcons, "utf8");
	if(connectcons == NULL)
	{
		printf("connect to mysql error: ip->%s,user->%s,pass->%s,db->%s\n", ip, user, pass, dbname);
		return 1;
	}
	int sockfd, clifd, links=0;
	ssize_t recvbytes;
	char buf[100];
	struct sockaddr_in servaddr, cliaddr;
	pthread_t pid, syncpid, connectpid, sendpid, heartpid;
	void* status;

	pthread_create(&syncpid, NULL, SyncMessge, NULL);
	cout<<GREEN<<"create sync thread finish"<<NONE<<endl;
	pthread_create(&connectpid, NULL, WriteConnectMsg, NULL);
	cout<<GREEN<<"create write connect msg thread finish"<<NONE<<endl;
	pthread_create(&sendpid, NULL, WriteSendMsg, NULL);
	cout<<GREEN<<"create write send msg thread finish"<<NONE<<endl;
	pthread_create(&heartpid, NULL, HeartBeat, NULL);
	cout<<GREEN<<"create heart beat thread finish"<<NONE<<endl;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd == -1)
	{
		perror("Error: init socket error----");
		mysql_close(connectcons);
		mysql_close(recordcons);
		mysql_close(schedulecons);
		return 1;
	}

	int on = 1;
	if((setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on))) < 0)
	{
		perror("Error: set socket paragram error---");
		mysql_close(connectcons);
		mysql_close(recordcons);
		mysql_close(schedulecons);
		return 1;
	}

	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(6000);
	if(bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1)
		perror("Error: bind error----");
	if(listen(sockfd, 10) == -1)
		perror("Error: listen error----");
	socklen_t len = sizeof(cliaddr);
	while(clifd = accept(sockfd, (struct sockaddr*)&cliaddr, &len))
	{  
		printf("accept one client connected\n");
		allsockets.push_back(clifd);
		SendLoadAverage(true);
		pthread_create(&pid, NULL, Hold, &clifd);
	}
	return 0;
}
