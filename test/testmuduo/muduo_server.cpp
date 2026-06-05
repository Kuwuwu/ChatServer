#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <iostream>
#include <functional>

using namespace std;
using namespace muduo;
using namespace muduo::net;
using namespace placeholders;

/*基于muduo网络库开发服务器程序
组合TcpServer对象
创建EventLoop事件循环对象的指针
明确TcpServer构造函数需要什么参数，输出ChatServer的构造函数
在当前服务器类的构造函数当中，注册处理连接的回调函数和处理读写事件的回调函数
设置合适的服务端线程数量，muduo库会自己分配I/O线程和worker线程
*/
class ChatServer
{
public:
    ChatServer(EventLoop *loop,
               const InetAddress &listenAddr,
               const string &nameArg)
        : _server(loop, listenAddr, nameArg),
          _loop(loop)
    {
        // 给服务器用户创建连接和断开的回调
        _server.setConnectionCallback(std::bind(&ChatServer::onConnection, this, _1));

        // 给服务器用户读写事件的回调
        _server.setMessageCallback(std::bind(&ChatServer::onMessage, this, _1, _2, _3));

        // 设置服务器端的线程数 1个I/O线程 3个worker线程
        _server.setThreadNum(4);
    }

    // 开启事件循环
    void start()
    {
        _server.start();
    }

private:
    // 处理用户连接创建和断开
    void onConnection(const TcpConnectionPtr &conn)
    {
        if (conn->connected())
        {
            cout << conn->peerAddress().toIpPort() << "->" << conn->localAddress().toIpPort() << "state:online" << endl;
        }
        else
        {
            cout << conn->peerAddress().toIpPort() << "->" << conn->localAddress().toIpPort() << "state:offline" << endl;
            conn->shutdown(); // close(fd);
            // _loop->quit();
        }
    }

    // 处理用户读写事件
    void onMessage(const TcpConnectionPtr &conn, // 连接
                   Buffer *buffer,               // 缓冲区
                   Timestamp time)               // 时间
    {
        string buf = buffer->retrieveAllAsString();
        cout << "recv data:" << buf << "time:" << time.toString() << endl;
        conn->send(buf);
    }

    TcpServer _server;
    EventLoop *_loop;
};

int main()
{
    EventLoop loop;
    InetAddress addr("127.0.0.1", 8000);
    ChatServer server(&loop, addr, "ChatServer");

    server.start(); // epoll_ctl
    loop.loop();    // epoll_wait以阻塞方式等待新用户连接，已连接用户的读写事件

    return 0;
}