#include "json/json.h"
#include <string>
#include <iostream>
using namespace std;
int main()
{
	Json::Value test;
	test["one"] = "one message";
	cout<<test["one"]<<endl;
	string out = test.toStyledString();
	cout<<out<<endl;
	return 0;
}
