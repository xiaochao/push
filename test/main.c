#include "MysqlCDriver.h"

int main(void)
{
	MYSQL *conn = Connect("localhost", "root", "123456", "test");
	if(!CreateTable(conn, "message"))
	{
		printf("create table error");
		return EXIT_FAILURE;
	}

	if(!InsertTable(conn, "message", "1", "Hello World"))
	{
		printf("insert table error");
		return EXIT_FAILURE;
	}

	return 0;
}
