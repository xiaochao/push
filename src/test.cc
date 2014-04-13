#include <iostream>
#include "MysqlCDriver.h"
using namespace std;

int main(void) 
{
	MYSQL *conn = Connect((char*)"127.0.0.1", (char*)"root", (char*)"123456", (char*)"push");
	InsertLocationTable(conn, (char*)"123456", (char*)"77.1", (char*)"88.1");
	return 0;
}
