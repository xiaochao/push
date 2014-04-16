#include "Points.h"

string CharToString(char *buf)
{
	string temp = buf;
	return temp;
}

bool InitUsers(MYSQL *conn, map<string, struct Points> &users)
{
	char buf[] = "select * from points";
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
		char temp[256];
		strcpy(temp, row[1]);
		strcpy(node.number, row[2]);
		strcpy(node.lave, row[3]);
		strcpy(node.seed, row[4]);
		strcpy(node.gettime, row[5]);
		users[CharToString(temp)] = node;
	}

	mysql_free_result(result);
	return true;
}
bool GetOneUser(MYSQL *conn, char* userid, struct Points &people, map<string, struct Points> &users)
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
	int num_rows = mysql_num_rows(result);
	if(num_rows <= 0)
		return false;

	MYSQL_ROW row = mysql_fetch_row(result);
	strcpy(temp, row[1]);
	strcpy(node.number, row[2]);
	strcpy(node.lave, row[3]);
	strcpy(node.seed, row[4]);
	strcpy(node.gettime, row[5]);
	users[CharToString(temp)] = node;
	people = node;
	return true;
}
bool GetRow(map<char*, struct Points> &users);

int GetRandom(int start, int end)
{
	int i;
	//cout<<"GetRandom-->"<<start<<"\t"<<end<<endl;
	srand((unsigned)time(0));
	return rand()%(end-start+1)+start;
}

bool AuthPeople(char *userid, struct Points &node)
{
	time_t formor = (time_t)(atoi(node.gettime));
	time_t later=time(NULL);
	char buf[16];
	sprintf(buf, "%d", (int)later);
	struct tm *ftm, *stm;
	int fday, sday;
	ftm = localtime(&formor);

	if(ftm == NULL)
		fday = 0;
	else
		fday = ftm->tm_mday;
	stm = localtime(&later);
	sday = stm->tm_mday;

	char seed[16], update[256];
	MYSQL *conn = Connect((char*)"127.0.0.1", (char*)"root", (char*)"123456", (char*)"push");
	bool g = GetSeedFromTable(conn, userid, seed, update);
	time_t updatetime =(time_t)(atoi(update));
	struct tm *utm = localtime(&updatetime);
	int uday = utm->tm_mday;
	//cout<<uday<<"\t"<<sday<<endl;
	if(uday != sday)
		return false;
	int iseed = atoi(seed);
	if(formor == 0)
	{
		char sql[256] = "insert into points(userid, number, lave, seed, time) values(";
		char buff[128];
		strcat(sql, userid);
		strcat(sql, ", 0, ");
		sprintf(buff, "%d", atoi(seed)*10);
		strcat(sql, buff);
		strcat(sql, ", ");
		strcat(sql, seed);
		strcat(sql, ", ");
		sprintf(buff, "%d", (int)later);
		strcat(sql, buff);
		strcat(sql, ")");
		cout<<"sql:"<<sql<<endl;
		if(mysql_query(conn, sql))
			return false;

		strcpy(node.gettime, buf);
		sprintf(buf, "%d", iseed*10);
		strcpy(node.lave, buf);
		strcpy(node.number, "0");
		strcpy(node.seed, seed);
	}
	else
	{
		if(fday == sday)
			return true;

		char sql[256]="update points set seed=";
		strcat(sql, seed);
		strcat(sql, ",lave=seed*10,number=0,time=");
		strcat(sql, buf);
		strcat(sql, " where userid=");
		strcat(sql, userid);
		int res = mysql_query(conn, sql);
		cout<<sql<<endl;
		if(res)
			return false;

		strcpy(node.gettime, buf);
		sprintf(buf, "%d", iseed*10);
		strcpy(node.lave, buf);
		strcpy(node.number, "0");
		strcpy(node.seed, seed);
	}
	mysql_close(conn);
	return true;
}

int GetPoints(MYSQL *conn, char* userid, map<string, struct Points> &users)
{
	map<string, struct Points>::iterator ite = users.find(userid);
	struct Points temp;
	if(ite == users.end())
	{
		bool re = GetOneUser(conn, userid, temp, users);
		if(!re)
		{
			strcpy(temp.gettime, "0");
			users[CharToString(userid)] = temp;
		}
	}

	bool re = AuthPeople(userid, users[CharToString(userid)]);
	//cout<<"***************"<<re<<endl;
	if(!re)
		return 0;
	
	struct Points people = users[CharToString(userid)];
	char buf1[128], buf2[128];
	int integr = GetRandom(1, atoi(people.lave));	
	int tempp = atoi(people.lave) - integr;
	sprintf(buf1, "%d", tempp);
	strcpy(users[CharToString(userid)].lave, buf1);
	sprintf(buf2, "%ld", GetTimeSecond());
	strcpy(users[CharToString(userid)].gettime, buf2);

	return integr;
}

bool UpdateLavePoints(MYSQL *conn, char *userid, int points)
{
	time_t now = time(NULL);
	char buf[128];
	char update[256] = "update points set number=number+1,lave=lave-";

	sprintf(buf, "%d", points);
	strcat(update, buf);
	sprintf(buf, "%d", (int)now);
	strcat(update, ",time=");
	strcat(update, buf);
	strcat(update, " where userid=");
	strcat(update, userid);
	//cout<<update<<endl;

	int res = mysql_query(conn, update);
	if(res)
		return false;
	return true;
}
