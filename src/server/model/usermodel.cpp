#include "usermodel.hpp"
#include "db.h"

#include <iostream>

using namespace std;

// User表增加的方法
bool UserModel::insert(User &user)
{
    // 1.组装sql语句
    char sql[1024] = "";
    sprintf(sql, "INSERT INTO Users(name, password, state) VALUES('%s', '%s', '%s')",
            user.getName().c_str(), user.getPwd().c_str(), user.getState().c_str());

    MySQL mysql;
    if (mysql.connect())
    {
        if (mysql.update(sql))
        {
            // 获取成功插入的用户数据生成的主键id并设置
            user.setId(mysql_insert_id(mysql.getConnection()));

            return true;
        }
    }

    return false;
}

// 根据用户号码查询用户信息
User UserModel::query(int id)
{
    // 组装sql
    char sql[1024] = "";
    sprintf(sql, "SELECT * from Users WHERE id = %d", id);

    MySQL mysql;

    if (mysql.connect())
    {
        MYSQL_RES *res = mysql.query(sql);

        if (res != nullptr) // 查询有结果
        {
            MYSQL_ROW row = mysql_fetch_row(res);
            /*
                MYSQL_ROW mysql_fetch_row(MYSQL_RES *result)
                作用：从结果集 res 中取出下一行数据

                若结果有多行可循环调用取出，没有下一行数据会返回nullptr
                    while(MYSQL_ROW row = mysql_fetch_row(res))
                    {
                        //...
                    }
            */

            if (row != nullptr)
            {
                User user;
                user.setId(atoi(row[0])); // atoi,C标准函数，字符串转整型
                user.setName(row[1]);
                user.setPwd(row[2]);
                user.setState(row[3]);

                // 释放结果集res
                mysql_free_result(res);

                return user;
            }
        }
    }

    return User();
}

// User表更新用户状态
bool UserModel::updateState(User user)
{
    // 组装sql
    char sql[1024] = "";
    sprintf(sql, "UPDATE Users SET state = '%s' WHERE id = %d", user.getState().c_str(), user.getId());

    MySQL mysql;

    if (mysql.connect()) // 连接数据库
    {
        if (mysql.update(sql)) // 更新成功
        {
            return true;
        }
    }

    return false;
}

// 重置用户的状态信息
void UserModel::resetState()
{
    // 组装sql
    char sql[1024] = "";
    sprintf(sql, "UPDATE Users SET state = 'offline' WHERE state = 'online'");

    MySQL mysql;

    if (mysql.connect()) // 连接数据库
    {
        mysql.update(sql);
    }
}