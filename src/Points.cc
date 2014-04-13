#include "Points.h"

bool InitUsers(MYSQL *conn, map<char*, struct Points> &users)
{
	char buf[] = "select * from points";
	char temp[256];
	struct Points node;
	int res = mysql_query(conn, buf);
	if(res)
		return false;

	MYSQL_RES *result = mysql_store_result(conn);
	if(result == NULL)
		return false;

	MYSQL_ROW row;
	while((row=mysql_fetch_row(result)))
	{
		strcpy(temp, row[1]);
		strcpy(node.number, row[2]);
		strcpy(node.lave, row[3]);
		strcpy(node.seed, row[4]);
		strcpy(node.gettime, row[5]);
		users[temp] = node;
	}

	mysql_free_result(result);
	return true;
}
bool GetOneUser(MYSQL *conn, char userid, struct Points &people, map<char*, struct Points> &users)
{
	char buf[128] = "select * from points where userid=";
	strcat(buf, userid);
	char temp[256];
	struct Points node;
	int res = mysql_query(conn, buf);
	if(res)
		return false;

	MYSQL_RES *result = mysql_store_result(conn);
	if(result == NULL)
		return false;

	MYSQL_ROW row = mysql_fetch_row(result);
	strcpy(temp, row[1]);
	strcpy(node.number, row[2]);
	strcpy(node.lave, row[3]);
	strcpy(node.seed, row[4]);
	strcpy(node.gettime, row[5]);
	users[temp] = node;
	people = node;
	return true;
}
bool GetRow(map<char*, struct Points> &users);

int GetRandom(int start, int end)
{
	int i;
	srand((unsigned)time(0));
	return rand()%(end-start+1)+start;
}

bool AuthPeople(char *userid, struct Points &node)
{
	int formor = node.gettime, later=time(NULL);
	char buf[16];
	struct tm *ftm, *stm;
	ftm = localtime(&formor);
	stm = localtime(&later);
	sprintf(buf, "%d", later);

	int fday = ftm->tm_mday, sday = stm->tm_mday;
	if(fday == sday)
		return true;

	char seed[16], update[256];
	MYSQL *conn = Connect("127.0.0.1", "root", "123456", "push");
	bool g = GetSeedFromTable(conn, userid, seed, update);
	int updatetime = atoi(update);
	struct tm *utm = localtime(&updatetime);
	int uday = utm->tm_mday;
	if(uday == fday)
		return false;

	int iseed = atoi(seed);
	char sql[256]="update points set seed=";
	strcat(sql, seed);
	strcat(sql, ",lave=seed*10,number=0,time=");
	strcat(sql, buf);
	strcat(sql, " where userid=");
	strcat(sql, userid);
	int res = mysql_query(conn, sql);
	if(res)
		return false;

	strcpy(node.gettime, buf);
	sprintf(buf, "%d", iseed*10);
	strcpy(node.lave, buf);
	strcpy(node.number, "0");
	strcpy(node.seed, seed);
	return true;
}

int GetPoints(char* userid, map<char*, struct Points> &users)
{
	struct Points people = users[userid];
	char buf1[128], buf2[128];
	int integr = GetRandom(1, atoi(people.lave));	
	int temp = atoi(people.lave) - integr;
	sprintf(buf1, "%d", temp);
	strcpy(users[userid].lave, buf1);
	sprintf(buf2, "%ld", GetTimeSecond());
	strcpy(users[userid].gettime, buf2);

	return integr;
}