#include <muduo/base/Logging.h>
#include <muduo/base/TimeZone.h>

#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <iostream>
#include <functional>
#include <signal.h>

#include "json.hpp"
#include "chatserver.hpp"
#include "chatservice.hpp"

// 处理服务器CTRL + C的
void resetHandler(int)
{
    // 设置状态
    ChatService::instrance()->reset();

    exit(0);
}

int main(int argc, char **argv)
{

    /*
    if (argc < 3)
        {
            cerr << "command invalid! example: ./ChatServer 127.0.0.1 8000" << endl;
            exit(-1);
        }
    */

    // 设置时区为北京时间（UTC+8）
    muduo::TimeZone beijing(8 * 3600, "CST");
    muduo::Logger::setTimeZone(beijing);

    const char *ip = "127.0.0.1";
    uint16_t port = 6000;

    if (argc >= 3)
    {
        // 解析通过命令行参数传递的ip和port
        ip = argv[1];
        port = atoi(argv[2]);
    }

    signal(SIGINT, resetHandler);

    EventLoop loop;
    InetAddress addr(ip, port);
    ChatServer server(&loop, addr, "ChatServer");

    // 查看端口
    cout << "ChatServer started on port " << port << endl;

    server.start(); // epoll_ctl
    loop.loop();    // epoll_wait以阻塞方式等待新用户连接，已连接用户的读写事件

    return 0;
}