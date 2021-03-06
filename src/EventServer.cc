#include <stdio.h>
#include <time.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <errno.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>
#include "MysqlCDriver.h"
#include "Points.h"
#include "OperateChar.h"
#include <vector>
#include <map>
#include <utility>
#include <iostream>
#include <pthread.h>
#include <signal.h>
#include <list>
#include "json/json.h"
#include <arpa/inet.h>
#include <event.h>
#include <algorithm>
using namespace std;

vector<string> all;
vector< vector<string> > send_to;
list<string> connect_log, send_log;
map<string, string> config;
map<int, string> client_deviceid;
map<int, pair<int, int> > send_num;
string control_ip = config["master_ip"], control_port = config["master_port"];
char ip[20], user[20], pass[20], dbname[20];
MYSQL *connect_cons, *record_cons, *conn, *schedule_cons, *location_cons;
vector<int> all_sockets;
pthread_mutex_t mutex;
int load_average = 0;

struct event_base *base;
struct sock_ev
{
	struct event* read_ev;
	struct event* write_ev;
	char *buffer;
};

int OpenSock();

//detail the Ctrl+C signal
//close mysql connection and all opened socket 
void MyClose(int sig)
{
	if(sig == SIGINT)
	{
		mysql_close(conn);
		mysql_close(connect_cons);
		mysql_close(record_cons);
		mysql_close(schedule_cons);
		vector<int>::iterator ite;
		for(ite=all_sockets.begin(); ite!=all_sockets.end(); ite++)
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
	/*int sockfd = OpenSock(), recv_bytes;
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
	close(sockfd);*/
	return 1;
}

//thread: write the connect msg to mysql
void *WriteConnectMsg(void *arg)
{
	mysql_set_character_set(connect_cons, "utf8");
	while(1)
	{
		sleep(1);
		list<string>::iterator ite = connect_log.begin();
		while(!connect_log.empty())
		{
			string statement = *ite;
			if(mysql_query(connect_cons, statement.c_str()))
			{
				cout<<"mysql: "<<statement<<" run failed"<<endl;
				break;
			}
			ite++;
			connect_log.pop_front();
		}
	}
}
//thread: write the send msg to mysql
void *WriteSendMsg(void *arg)
{
	mysql_set_character_set(record_cons, "utf8");
	while(1)
	{
		sleep(1);
		list<string>::iterator ite = send_log.begin();
		while(!send_log.empty())
		{
			string statement = *ite;
			if(mysql_query(record_cons, statement.c_str()))
			{
				cout<<"mysql: "<<statement<<" run failed"<<endl;
				break;
			}
			ite++;
			send_log.pop_front();
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
		//cout<<PURPLE<<"is is heart beat"<<NONE<<endl;
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
	if(mysql_query(schedule_cons, select))
	{
		cout<<"search database error"<<endl;
		reval = -1;
	}
	
	MYSQL_RES *result = mysql_store_result(schedule_cons);
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

			vector<string> devices = SplitChar(sql_row[3], ";");
			send_to.push_back(devices);
		}

		mysql_free_result(result);
	}
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
	if(inet_pton(AF_INET, control_ip.c_str(), &client_addr.sin_addr) == 0)
	{
		perror("inet_pton funciton error:\t");
		close(sockfd);
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

void DeleteClient(int fd)
{
	map<int, string>::iterator ite = client_deviceid.find(fd);
	if(ite != client_deviceid.end())
		client_deviceid.erase(ite);
	close(fd);
}

void release_sock_event(struct sock_ev* ev)
{
    event_del(ev->read_ev);
    free(ev->read_ev);
	event_del(ev->write_ev);
    free(ev->write_ev);
    free(ev);
}

void on_read(int sock, short event, void *arg);
void on_write(int sock, short event, void *arg);
void on_accept(int sock, short event, void *arg)
{
	int clifd;
	struct sockaddr_in cliaddr;
	struct sock_ev* ev = (struct sock_ev*)malloc(sizeof(struct sock_ev));
	ev->read_ev = (struct event*)malloc(sizeof(struct event));
	ev->write_ev = (struct event*)malloc(sizeof(struct event));
	
	socklen_t size = sizeof(struct sockaddr);
	clifd = accept(sock, (struct sockaddr*)&cliaddr, &size);
	if(clifd < 0)
	{
		perror("");
		cout<<RED<<"accept error"<<NONE<<endl;
		return;
	}
	getpeername(clifd, (struct sockaddr*)&cliaddr, (socklen_t*)sizeof(cliaddr));
	cout<<GREEN<<"get a new client:"<<inet_ntoa(cliaddr.sin_addr)<<NONE<<endl;
	//get a new client, push it to queue and send loadaverage msg to control
	all_sockets.push_back(clifd);
	SendLoadAverage(true);
	
	//add read event
	event_set(ev->read_ev, clifd, EV_READ|EV_PERSIST, on_read, ev);
	event_base_set(base, ev->read_ev);
	event_add(ev->read_ev, NULL);
}

void on_read(int sock, short event, void *arg)
{
	int client_fd = sock, msgid = 0, recvbyte, times = 3;
	char buf[200], temp[200]="", deviceid[20]="0";
	struct event *write_ev;
	struct sock_ev *ev = (struct sock_ev*)arg;
	
	//connect init:get device message from client send message 
	//if cannot get right message from client three times, exit
	recvbyte = recv(client_fd, buf, sizeof(buf), 0);
	if(recvbyte <= 0)
	{
		cout<<"******************"<<endl;
		release_sock_event(ev);
		DeleteClient(client_fd);
		return;
	}

	buf[recvbyte] = '\0';
	SplitChar(buf, temp, 1);
	cout<<"get string "<<buf<<endl;
	if(strcmp(temp, "GPS") == 0)
	{
		char userid[256], loni[256], latitu[256];
		SplitChar(buf, userid, 2);
		SplitChar(buf, loni, 3);
		SplitChar(buf, latitu, 4);
		bool lo = InsertLocationTable(location_cons, userid, loni, latitu);
		if(lo)
			cout<<"record user location successfully"<<endl;
		else
			cout<<"can not record user location"<<endl;
		send_num[client_fd].second = time(NULL);
		return;
	}
	if(strcmp(temp, "getpoints") == 0)
	{
		map<string, struct Points> mtemp;
		char buf[128]="成功领取", ctemp[128], userid[128];
		SplitChar(buf, userid, 2);
		MYSQL *tempconn = Connect((char*)"127.0.0.1", (char*)"root", (char*)"123456", (char*)"push");
		int res = InitUsers(tempconn, mtemp);
		res = GetPoints(tempconn, userid, mtemp);
		sprintf(ctemp, "%d", res);
		res = UpdateLavePoints(tempconn, userid, res);
		mysql_close(tempconn);
		if(!res)
			return;
		strcat(buf, ctemp);
		strcat(buf, "积分");
		Json::Value msg;
		msg["title"] = "领取成功";
		msg["content"] = buf;
		string content = msg.toStyledString();
		if(send(client_fd, content.c_str(), content.length(), 0)<0)
			cout<<"*****************send getpoints failed"<<endl;
		return;

	}
	if(strcmp(temp, "READY") != 0)
		return;

	cout<<"get the right init string:"<<buf<<endl;
	SplitChar(buf, deviceid, 2);
	deviceid[strlen(deviceid)] = '\0';

	client_deviceid[client_fd] = string(deviceid);
	
	char insert[100] = "insert into connect(device_id, status) values('";
	strcat(insert, deviceid);
	strcat(insert, "', 'connected')");
	connect_log.push_back(string(insert));
	
	send_num[client_fd].first = GetMaxSendId(deviceid);
	send_num[client_fd].second = 0;
	cout<<PURPLE<<"the max msgid sended last time"<<send_num[client_fd].first<<NONE<<endl;

	event_set(ev->write_ev, sock, EV_WRITE|EV_PERSIST, on_write, NULL);
	event_base_set(base, ev->write_ev);
	event_add(ev->write_ev, NULL);
}

void on_write(int sock, short event, void *arg)
{
	int clientfd = sock, recvbyte;
	int msgid, now_time=0;
	char buf[200],  deviceid[20]="0", insert[200] = "";
	strcpy(deviceid, client_deviceid[clientfd].c_str());
	msgid = send_num[clientfd].first;
	
	while(msgid < all.size() && all[msgid].length() != 0)
	{
		//send message
		if(send_to[msgid][0].compare(string("all")) == 0 or find(send_to[msgid].begin(), send_to[msgid].end(), client_deviceid[clientfd]) != send_to[msgid].end())
		{
			cout<<all[msgid].c_str()<<endl;
			if(send(clientfd, all[msgid].c_str(), all[msgid].length(), 0) < 0)
			{
				strcpy(insert, "insert into record(device_id, message_id, status) values('");
				strcat(insert, deviceid); strcat(insert, "', ");
				char temp[10];
				sprintf(temp, "%d", msgid);
				strcat(insert, temp); strcat(insert, ", 'failed'");
				send_log.push_back(string(insert));
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
			send_log.push_back(string(insert));
			send_num[clientfd].second = time(NULL);
		}
		msgid++;
	}
	send_num[clientfd].first = msgid;
	now_time = time(NULL);

	//send keepalive if no activities in 15mins
	if((now_time - send_num[clientfd].second) > 900)
	{
		if(send(clientfd, "2", sizeof("2"), 0) < 0)
		{
			perror("send-->lost client connection");
			SendLoadAverage(false);
			DeleteClient(clientfd);
			return;
		}

		while(1)
		{
			recvbyte = recv(clientfd, buf, sizeof(buf), 0);
			buf[recvbyte] = '\0';
			if(recvbyte == 0)
			{
				cout<<YELLOW<<"client close the connection"<<NONE<<endl;
				SendLoadAverage(false);
				DeleteClient(clientfd);
				return;
			}

			if(recv<0 && (errno==EINTR||errno==EWOULDBLOCK||errno==EAGAIN))
			{
				continue;
			}
			else if(recv < 0)
			{
				SendLoadAverage(false);
				DeleteClient(clientfd);
				return;
			}
			break;
		}
		cout<<"clinet is active"<<endl;
		send_num[clientfd].second= time(NULL);
	}
	//sleep(1);
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

	//read the config file
	strcpy(ip, config["ip"].c_str());
	strcpy(user, config["user"].c_str());
	strcpy(pass, config["password"].c_str());
	strcpy(dbname, config["dbname"].c_str());
	if(strlen(user) <= 0)
	{
		cout<<"配置文件格式错误"<<endl;
		return 1;
	}

	//start init, register to control
	//Init();

	//process the Ctrl+C singal
	signal(SIGINT, MyClose);

	//create connections to mysql
	connect_cons = Connect(ip, user, pass, dbname);
	record_cons = Connect(ip, user, pass, dbname);
	schedule_cons = Connect(ip, user, pass, dbname);
	location_cons = Connect(ip, user, pass, dbname);
	mysql_set_character_set(connect_cons, "utf8");
	if(connect_cons == NULL)
	{
		printf("connect to mysql error: ip->%s,user->%s,pass->%s,db->%s\n", ip, user, pass, dbname);
		return 1;
	}

	//some variables
	int sockfd, links=0;
	struct sockaddr_in servaddr;
	pthread_t pid, syncpid, connectpid, sendpid, heartpid;

	//create threads
	pthread_create(&syncpid, NULL, SyncMessge, NULL);
	cout<<GREEN<<"create sync thread finish"<<NONE<<endl;
	pthread_create(&connectpid, NULL, WriteConnectMsg, NULL);
	cout<<GREEN<<"create write connect msg thread finish"<<NONE<<endl;
	pthread_create(&sendpid, NULL, WriteSendMsg, NULL);
	cout<<GREEN<<"create write send msg thread finish"<<NONE<<endl;
	pthread_create(&heartpid, NULL, HeartBeat, NULL);
	cout<<GREEN<<"create heart beat thread finish"<<NONE<<endl;

	//start socket process
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd == -1)
	{
		perror("Error: init socket error----");
		mysql_close(connect_cons);
		mysql_close(record_cons);
		mysql_close(schedule_cons);
		return 1;
	}

	/*int on = 1;
	if((setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on))) < 0)
	{
		perror("Error: set socket paragram error---");
		mysql_close(connect_cons);
		mysql_close(record_cons);
		mysql_close(schedule_cons);
		return 1;
	}*/

	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(6000);
	evutil_make_socket_nonblocking(sockfd);
	if(bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1)
		perror("Error: bind error----");
	if(listen(sockfd, 10000) == -1)
		perror("Error: listen error----");

	//init event and start event loop
	struct event listen_ev;
	base = event_base_new();
	event_set(&listen_ev, sockfd, EV_READ|EV_PERSIST, on_accept, NULL);
	event_base_set(base, &listen_ev);
	event_add(&listen_ev, NULL);
	event_base_dispatch(base);
	
	return 0;
}
