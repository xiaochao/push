/*************************************************************************
	> File Name: Points.h
	> Author: xiaochao
	> Mail: cschy.2008@163.com 
	> Created Time: 2014年04月10日 星期四 14时12分56秒
 ************************************************************************/

#include <iostream>
#include <map>
#include <cstring>
#include "MysqlCDriver.h"
#include <stdlib.h>
using namespace std;

struct Points
{
	char number[16];
	char lave[16];
	char seed[16];
	char gettime[16];
};

//get users from mysql and store all in memory
bool InitUsers(MYSQL *conn, map<string, struct Points> &users);
bool GetOneUser(MYSQL *conn, char* userid, struct Points &people, map<string, struct Points> &users);
bool GetRow(map<string, struct Points> &users);
int GetRandom(int start, int end);
int GetPoints(MYSQL *conn, char* userid, map<string, struct Points> &users);
bool AuthPeople(char *userid, struct Points &node);
bool UpdateLavePoints(MYSQL *conn, char *userid, int points);
