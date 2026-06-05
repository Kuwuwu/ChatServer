#include "json.hpp"
using json = nlohmann::json;

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <muduo/net/TcpServer.h>

using namespace std;

string func1()
{
    json js;
    js["msg_type"] = 2;
    js["from"] = "zhangsan";
    js["to"] = "lisi";
    js["msg"] = "hello, Are you ok?";

    string buf;
    buf = js.dump();

    return buf;
}

string func2()
{
    json js;
    // 添加数组
    js["id"] = {1, 2, 3, 4, 5};
    // 添加key-value
    js["name"] = "zhang san";
    // 添加对象
    js["msg"]["zhang san"] = "hello world";
    js["msg"]["liu shuo"] = "hello china";
    // 上面等同于下面这句一次性添加数组对象
    js["msg"] = {{"zhang san", "hello world"}, {"liu shuo", "hello china"}};

    return js.dump();
}

void func3()
{
    json js;
    // 直接序列化一个vector容器
    vector<int> vec;
    vec.push_back(1);
    vec.push_back(2);
    vec.push_back(5);
    js["list"] = vec;
    // 直接序列化一个map容器
    map<int, string> m;
    m.insert({1, "黄山"});
    m.insert({2, "华山"});
    m.insert({3, "泰山"});
    js["path"] = m;
    cout << js << endl;
}

void reverse_func1()
{
    string ret = func1();
    json buf = json::parse(ret);

    cout << buf["msg_type"] << endl;
    cout << buf["from"] << endl;
    cout << buf["to"] << endl;
    cout << buf["msg"] << endl;
}

void reverse_func2()
{
    string ret = func2();
    json buf = json::parse(ret);

    cout << buf["id"] << endl;
    auto arr = buf["id"];
    cout << arr[2] << endl;
}

int main()
{
    reverse_func2();

    return 0;
}