#include "offlinemessagemodel.hpp"
#include "db.h"

// 存储用户的离线消息
void OfflineMsgModel::insert(int userid, string msg)
{
    // 1.组装sql语句
    char sql[1024] = "";
    sprintf(sql, "INSERT INTO OfflineMessage(userid, message) VALUES(%d, '%s')",
            userid, msg.c_str());

    MySQL mysql;
    if (mysql.connect())
    {
        mysql.update(sql);
    }
}

// 删除用户的离线消息
void OfflineMsgModel::remove(int userid)
{
    // 组装sql
    char sql[1024] = "";
    sprintf(sql, "DELETE FROM OfflineMessage WHERE userid = %d", userid);

    MySQL mysql;

    if (mysql.connect()) // 连接数据库
    {
        mysql.update(sql);
    }
}

// 查询目标用户的离线消息
vector<string> OfflineMsgModel::query(int userid)
{
    // 组装sql
    char sql[1024] = "";
    sprintf(sql, "SELECT message from OfflineMessage WHERE userid = %d", userid);

    MySQL mysql;
    vector<string> retvec;

    if (mysql.connect())
    {
        MYSQL_RES *res = mysql.query(sql);

        if (res != nullptr) // 查询有结果
        {
            MYSQL_ROW row;

            while ((row = mysql_fetch_row(res)) != nullptr)
            {
                retvec.push_back(row[0]);
                /*
                    MYSQL_ROW mysql_fetch_row(MYSQL_RES *result)
                    作用：从结果集 res 中取出下一行数据

                    若结果有多行可循环调用取出，没有下一行数据会返回nullptr
                        while(MYSQL_ROW row = mysql_fetch_row(res))
                        {
                            //...
                        }
                */
            }

            // 释放结果集res
            mysql_free_result(res);
        }
    }
    return retvec;
}