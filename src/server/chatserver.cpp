#include "chatserver.hpp"
#include "json.hpp"
#include "chatservice.hpp"

#include <functional>
#include <string>
#include <muduo/base/Logging.h>

using namespace std;
using namespace placeholders;
using json = nlohmann::json;

ChatServer::ChatServer(EventLoop *loop,
                       const InetAddress &listenAddr,
                       const string &nameArg)
    : _loop(loop), _server(loop, listenAddr, nameArg)
{
    // 给服务器用户连接和断开注册回调
    _server.setConnectionCallback(std::bind(&ChatServer::onConnection, this, _1));

    // 给服务器用户消息发送注册回调
    _server.setMessageCallback(std::bind(&ChatServer::onMessage, this, _1, _2, _3));

    // 设置服务器端的线程数 1个I/O，3个worker线程
    _server.setThreadNum(4);
}

// 启动服务
void ChatServer::start()
{
    _server.start();
}

// 上报链接相关信息的回调函数
void ChatServer::onConnection(const TcpConnectionPtr &conn)
{
    if (!conn->connected())
    {
        conn->shutdown();
        ChatService::instrance()->clientCloseException(conn);
    }
}

// 上报读写事件相关的回调函数
void ChatServer::onMessage(const TcpConnectionPtr &conn,
                           Buffer *buffer,
                           Timestamp time)
{
    while (buffer->readableBytes() >= sizeof(uint32_t))
    {
        // 完全解耦网络模块和业务模块代码
        // 通过js[msgid]获取业务handler

        uint32_t len = buffer->peekInt32(); // 窥探4字节长度头，大端序
        if (buffer->readableBytes() < len + sizeof(uint32_t))
        {
            break; // 半包，数据不够一个完整报文，退出循环等待下次回调
        }

        buffer->retrieve(sizeof(uint32_t)); // 跳过长度头
        string buf = buffer->retrieveAsString(len);

        json js;
        try
        {
            // 数据的反序列化
            js = json::parse(buf);
        }
        catch (const nlohmann::json::parse_error &e)
        {
            LOG_ERROR << "JSON parse error: " << e.what();
            conn->shutdown();
            return;
        }

        auto msgHandler = ChatService::instrance()->getHandler(js["msgid"].get<int>());
        // 回调绑定好的业务处理器
        msgHandler(conn, js, time);
    }
}
