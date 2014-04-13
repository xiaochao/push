#include "MysqlCDriver.h"
#include <iostream>
#include <string.h>
using namespace std;

MYSQL *Connect(char* ip, char* username, char* password, char* dbname)
{
	MYSQL *conn_ptr;
	conn_ptr = mysql_init(NULL);

	if( !conn_ptr )
	{
		printf("mysql_init failed\n");
		return NULL;
	}

//	printf("%s\t%s\t%s\t%s\n", ip, username, password, dbname);
	conn_ptr = mysql_real_connect(conn_ptr, ip, username, password, dbname, 0, NULL, 0);
	if(!conn_ptr)
	{
		printf("Connection failed\n");
		return NULL;
	}

	return conn_ptr;
}

bool InsertLocationTable(MYSQL *conn_ptr, char* token, char* lon, char* lat)
{
	char buf[20];
	sprintf(buf, "%ld", GetTimeSecond());
	char select[256] = "select * from location where userid=";
	strcat(select, token);
	int res = mysql_query(conn_ptr, select);
	if(res)
		return false;
	MYSQL_RES *result = mysql_store_result(conn_ptr);
	long nums = mysql_num_rows(result);
	if(nums == 0) 
	{
		char cc[256] = "insert into location(userid, longitude, latitude, time) values(";
		strcat(cc, token);
		strcat(cc, ", ");
		strcat(cc, lon);
		strcat(cc, ", ");
		strcat(cc, lat);
		strcat(cc, ", ");
		strcat(cc, buf);
		strcat(cc, ")");
		int res = mysql_query(conn_ptr, cc);
		cout<<cc<<endl;
		if(res)
			return false;
		return true;
	}
	else
	{
		char cc[256] = "update location set longitude=";
		strcat(cc, lon);
		strcat(cc, ",latitude=");
		strcat(cc, lat);
		strcat(cc, ",time=");
		strcat(cc, buf);
		strcat(cc, " where userid=");
		strcat(cc, token);
		int res = mysql_query(conn_ptr, cc);
		cout<<cc<<endl;
		if(res)
			return false;
		return true;
	}
}

bool UpdatePointsTable(MYSQL *conn, char *userid, char *lave, char *gtime)
{
	char update[256] = "update points set number=number+1,lave=";
	strcat(update, lave);
	strcat(update, ",gettime=");
	strcat(update, gtime);
	strcat(update, " where userid=");
	strcat(update, userid);
	int res = mysql_query(conn, update);
	if(res)
		return false;
	return true;
}

bool GetSeedFromTable(MYSQL *conn, char *userid, char *seed, char *seconds)
{
	char select[256] = "select * from m_user_points where userid=";
	strcat(select, userid);
	int res = mysql_query(conn, select);
	if(res)
		return false;

	MYSQL_RES *result = mysql_store_result(conn);
	if(result == NULL)
		return false;

	MYSQL_ROW row = mysql_fetch_row(result);
	strcpy(seed, row[2]);
	strcpy(seconds, row[4]);
	mysql_free_result(result);

	return true;
}

void CloseConnect(MYSQL *conn_ptr)
{
	mysql_close(conn_ptr);
}

string trim(string strr)
{
	string str = strr;
	if(str.empty())
		return str;
	int i = 0;
	for(i=0; i<str.length(); i++)
		if(str[i] != ' ')
			break;
	str = str.substr(i, str.length()-i);
	for(i=str.length()-1; i>=0;  i--)
		if(str[i] != ' ')
			break;
	str = str.substr(0, i+1);
	return str;
}

map<string, string> LoadConfig(const char *path)
{
	map<string, string> config;
	ifstream file(path);
	string str;
	if(!file)
	{
		cout<<"配置文件不存在"<<endl;
		return config;
	}
	while(file >> str)
	{
		str = trim(str);
		if(str[0] == '#')
			continue;
		int npos = str.find('=');
		string key = trim(str.substr(0, npos));
		string value = trim(str.substr(npos+1, str.length()-npos-1));
		config[key] = value;
	}
	file.close();
	return config;
}

void GetIp(char *buf, const char *eth)
{
	char ip[20], command[100] = "ifconfig ";
	strcat(command, eth);
	strcat(command,"|grep 'inet '|awk -F' ' '{print $2}'|awk -F':' '{print $2}'");
	FILE *pp;
	if((pp = popen(command, "r")) == NULL)
	{
		cout<<"run command error ---"<<command<<endl;
		return;
	}

	fgets(buf, 16, pp);
	buf[strlen(buf)-1] = '\0';
	pclose(pp);
}

long GetTimeSecond()
{
	time_t now;
	time(&now);
	return (long)now;
}

long GetTimeDiff(long start, long end) {
	return (long)difftime((time_t)end, (time_t)start);
}
