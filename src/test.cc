#include <iostream>
#include <map>
#include "Points.h"
using namespace std;

int main(void) 
{
	char userid[] = "12345";
	map<string, struct Points> temp;
	MYSQL *conn = Connect((char*)"127.0.0.1", (char*)"root", (char*)"123456", (char*)"push");
	int res = InitUsers(conn, temp);
	res = GetPoints(conn, userid, temp);
	cout<<res<<"\t"<<temp.size()<<endl;
	res = UpdateLavePoints(conn, userid, res);
	cout<<res<<"\t"<<temp.size()<<endl;
	return 0;
}
