#ifndef _MYSQLCDRIVER_H
#define _MYSQLCDRIVER__

#include <stdlib.h>
#include <stdio.h>
#include "mysql.h"
#include "errmsg.h"
#include <map>
#include <fstream>
#include <string>
using namespace std;

MYSQL *Connect(char* ip, char* username, char* password, char* dbname);
void CloseConnect(MYSQL *conn_ptr);
bool CreateTable(MYSQL *conn_ptr, char* tablename);
bool InsertTable(MYSQL *conn_ptr, char* tablename, char* id, char* msg);
map<string, string> LoadConfig(const char* path);
void GetIp(char *buf, const char *eth);
long GetTimeSecond();
long GetTimeDiff(long start, long end);
bool InsertLocationTable(MYSQL *conn_ptr, char* token, char* lon, char* lat);
bool GetSeedFromTable(MYSQL *conn, char *userid, char *seed, char *seconds);

#endif
