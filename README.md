# ChatServer

基于 [muduo](https://github.com/chenshuo/muduo) 网络库实现的集群聊天服务器与客户端，支持 nginx TCP 负载均衡，可水平扩展多台 ChatServer 实例。

## 架构概览

```
┌──────────────┐   ┌──────────────┐   ┌──────────────┐
│  ChatClient  │   │  ChatClient  │   │  ChatClient  │
└──────┬───────┘   └──────┬───────┘   └──────┬───────┘
       │                  │                  │
       └──────────────────┼──────────────────┘
                          │
                    ┌─────┴─────┐
                    │   nginx   │  ← TCP stream 负载均衡 (port 8000)
                    └─────┬─────┘
                          │
              ┌───────────┼───────────┐
              │           │           │
       ┌──────┴──────┐ ┌──┴─────┐ ┌──┴─────┐
       │ ChatServer  │ │  ...   │ │  ...   │  ← 多实例 (6000, 6002, ...)
       └──────┬──────┘ └────────┘ └────────┘
              │
       ┌──────┼──────┐
       │      │      │
  ┌────┴──┐ ┌─┴──┐ ┌─┴─────┐
  │ MySQL │ │Redis│ │Redis  │  ← 持久化 + 跨服务器消息
  └───────┘ └────┘ └───────┘
```

- **Client** 通过原始 TCP socket 连接 nginx（默认 8000），nginx 将流量分发到后端 ChatServer 实例
- **Server** 之间通过 **Redis Pub/Sub** 共享在线用户连接信息，实现跨服务器的消息转发
- 数据持久化使用 **MySQL**（用户、好友、群组、离线消息）

## 功能

| 功能 | 命令 |
|------|------|
| 用户注册 | `2`（首页菜单） |
| 用户登录 | `1`（首页菜单） |
| 一对一聊天 | `chat:friendid:message` |
| 添加好友 | `addfriend:friendid` |
| 创建群组 | `creategroup:groupname:groupdesc` |
| 加入群组 | `addgroup:groupid` |
| 群聊 | `groupchat:groupid:message` |
| 注销登录 | `loginout` |
| 查看帮助 | `help` |

### 特性

- **离线消息**：消息接收方不在线时，存入 MySQL，上线后自动推送
- **用户状态同步**：登录 / 注销 / 异常断线 自动更新在线状态
- **集群扩展**：通过 nginx stream 模块做 TCP 四层代理，后端可挂多个 ChatServer 实例
- **跨服务器消息**：Redis 订阅/发布机制确保不同 ChatServer 实例上的用户能够互相通信

## 依赖

| 组件 | 用途 |
|------|------|
| [muduo](https://github.com/chenshuo/muduo) | 网络库（Reactor + 线程池） |
| [nlohmann/json](https://github.com/nlohmann/json) | JSON 序列化/反序列化 |
| MySQL 5.7+ | 数据持久化 |
| [hiredis](https://github.com/redis/hiredis) | Redis C 客户端 |
| Redis 5.0+ | 跨服务器消息发布/订阅 |
| nginx (with stream module) | TCP 四层负载均衡 |
| CMake ≥ 3.0 | 构建系统 |
| g++ (C++11+) | 编译器 |

## 快速开始

### 1. 安装依赖

```bash
# Ubuntu / Debian
sudo apt install -y build-essential cmake libmysqlclient-dev libhiredis-dev nginx redis-server mysql-server

# 安装 muduo（需从源码编译）
git clone https://github.com/chenshuo/muduo.git
cd muduo && ./build.sh && sudo ./build.sh install
```

### 2. 初始化数据库

```sql
-- 连接 MySQL
mysql -u root -p

-- 创建数据库
CREATE DATABASE chat;

-- 选择数据库
USE chat;

-- 用户表
CREATE TABLE user (
    id INT PRIMARY KEY AUTO_INCREMENT,
    name VARCHAR(50) NOT NULL UNIQUE,
    password VARCHAR(50) NOT NULL,
    state ENUM('online', 'offline') DEFAULT 'offline'
);

-- 好友表
CREATE TABLE friend (
    userid INT NOT NULL,
    friendid INT NOT NULL,
    PRIMARY KEY (userid, friendid)
);

-- 离线消息表
CREATE TABLE offlinemessage (
    userid INT NOT NULL,
    message TEXT NOT NULL
);

-- 群组表
CREATE TABLE allgroup (
    id INT PRIMARY KEY AUTO_INCREMENT,
    groupname VARCHAR(50) NOT NULL,
    groupdesc VARCHAR(200) DEFAULT ''
);

-- 群组成员表
CREATE TABLE groupuser (
    groupid INT NOT NULL,
    userid INT NOT NULL,
    grouprole ENUM('creator', 'normal') DEFAULT 'normal',
    PRIMARY KEY (groupid, userid)
);
```

数据库连接配置在 [`src/server/db/db.cpp`](src/server/db/db.cpp#L4-L7)，默认值：

```cpp
server   = "127.0.0.1"
user     = "root"
password = "123456"
dbname   = "chat"
```

### 3. 启动 Redis

```bash
sudo systemctl start redis-server
```

### 4. 编译项目

```bash
git clone https://github.com/yourname/ChatServer.git
cd ChatServer
bash autobuild.sh
```

编译产物输出到 `bin/` 目录：

```
bin/
├── ChatServer   # 服务端
└── ChatClient   # 客户端
```

### 5. 配置 nginx（集群模式）

创建 `/etc/nginx/stream.conf`：

```nginx
stream {
    upstream chatserver_backend {
        server 127.0.0.1:6000;
        server 127.0.0.1:6002;   # 第二个实例（可选）
    }

    server {
        listen 8000;
        proxy_pass chatserver_backend;
    }
}
```

在 `/etc/nginx/nginx.conf` 的 `http { }` 块**外面**加入：

```nginx
include /etc/nginx/stream.conf;
```

重载 nginx：

```bash
sudo nginx -t && sudo nginx -s reload
```

### 6. 运行

**启动服务端：**

```bash
# 默认监听 127.0.0.1:6000
./bin/ChatServer

# 或指定 IP 和端口
./bin/ChatServer 127.0.0.1 6002
```

**启动客户端：**

```bash
# 默认连接 127.0.0.1:8000（走 nginx 负载均衡）
./bin/ChatClient

# 或直连指定服务端（调试用）
./bin/ChatClient 127.0.0.1 6000
```

## 通信协议

客户端与服务端之间使用 **JSON over TCP**，每条消息包含 `msgid` 字段标识消息类型：

| msgid | 类型 | 说明 |
|-------|------|------|
| 1 | `LOGIN_MSG` | 登录请求 |
| 2 | `LOGIN_MSG_ACK` | 登录响应 |
| 3 | `LOGINOUT_MSG` | 注销请求 |
| 4 | `REG_MSG` | 注册请求 |
| 5 | `REG_MSG_ACK` | 注册响应 |
| 6 | `ONE_CHAT_MSG` | 一对一聊天 |
| 7 | `ADD_FRIEND_MSG` | 添加好友 |
| 8 | `CREATE_GROUP_MSG` | 创建群组 |
| 9 | `ADD_GROUP_MSG` | 加入群组 |
| 10 | `GROUP_CHAT_MSG` | 群聊消息 |

协议定义见 [`include/public.hpp`](include/public.hpp)。

## 项目结构

```
ChatServer/
├── autobuild.sh                  # 一键编译脚本
├── CMakeLists.txt                # 顶层 CMake
├── bin/                          # 编译输出
├── build/                        # 构建中间文件
├── include/
│   ├── public.hpp                # 公共消息类型枚举
│   └── server/
│       ├── chatserver.hpp        # 服务器主类
│       ├── chatservice.hpp       # 业务逻辑层
│       ├── db/db.h               # MySQL 封装
│       ├── redis/redis.hpp       # Redis 封装
│       └── model/
│           ├── user.hpp           # 用户实体
│           ├── group.hpp          # 群组实体
│           ├── groupuser.hpp      # 群组成员实体
│           ├── usermodel.hpp      # 用户数据操作
│           ├── friendmodel.hpp    # 好友数据操作
│           ├── groupmodel.hpp     # 群组数据操作
│           └── offlinemessagemodel.hpp  # 离线消息数据操作
├── src/
│   ├── CMakeLists.txt
│   ├── client/
│   │   ├── CMakeLists.txt
│   │   └── main.cpp              # 客户端主程序
│   └── server/
│       ├── CMakeLists.txt
│       ├── main.cpp              # 服务端入口
│       ├── chatserver.cpp        # 网络层（muduo TcpServer）
│       ├── chatservice.cpp       # 业务逻辑层
│       ├── db/db.cpp             # MySQL 封装实现
│       └── redis/redis.cpp       # Redis 封装实现
├── thirdparty/
│   └── json.hpp                  # nlohmann/json (header-only)
└── test/
    ├── testjson/                 # JSON 序列化测试
    └── testmuduo/                # muduo 基础测试
```

## 技术要点

- **网络层**：muduo Reactor 模式，1 个 I/O 线程 + 3 个 worker 线程，支持高并发连接
- **业务解耦**：通过消息 ID 注册回调函数，网络模块与业务模块完全分离
- **线程安全**：`userConnMap` 使用 `std::mutex` 保护，客户端读写线程通过 `sem_t` 信号量同步
- **单例模式**：`ChatService` 使用单例模式，全局唯一业务实例
- **Redis 集群支持**：每个 ChatServer 实例向 Redis 订阅已登录用户频道，收到消息后检查本地连接并转发，实现跨服务器通信
