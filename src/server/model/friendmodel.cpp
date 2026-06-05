#include "friendmodel.hpp"
#include "db.h"

// 添加好友关系
void FriendModel::insert(int userid, int friendid)
{
    // 1.组装sql语句
    char sql[1024] = "";
    sprintf(sql, "INSERT INTO Friend(userid,friendid) VALUES(%d, %d)",
            userid, friendid);

    MySQL mysql;
    if (mysql.connect())
    {
        mysql.update(sql);
    }
}

// 返回好友关系列表
vector<User> FriendModel::query(int userid)
{
    // 组装sql
    char sql[1024] = "";
    sprintf(sql, "SELECT u.id, u.name, u.state "
                 "FROM Friend f JOIN Users u "
                 "ON f.friendid = u.id "
                 "WHERE userid = %d",
            userid);

    vector<User> retvec;
    MySQL mysql;

    if (mysql.connect())
    {
        MYSQL_RES *res = mysql.query(sql);

        if (res != nullptr)
        {
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(res)) != nullptr) // 查询有结果
            {
                /*
                    MYSQL_ROW mysql_fetch_row(MYSQL_RES *result)
                    作用：从结果集 res 中取出下一行数据

                    若结果有多行可循环调用取出，没有下一行数据会返回nullptr
                        while(MYSQL_ROW row = mysql_fetch_row(res))
                        {
                            //...
                        }
                */

                User user;
                user.setId(atoi(row[0])); // atoi,C标准函数，字符串转整型
                user.setName(row[1]);
                user.setState(row[2]);

                retvec.push_back(user);
            }
            // 释放结果集res
            mysql_free_result(res);
        }
    }

    return retvec;
}