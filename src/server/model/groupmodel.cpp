#include "groupmodel.hpp"
#include "db.h"

// 创建群聊
bool GroupModel::createGroup(Group &group)
{
    // 1.组装sql语句
    char sql[1024] = "";
    sprintf(sql, "INSERT INTO AllGroup(groupname,groupdesc) VALUES('%s', '%s')",
            group.getName().c_str(), group.getDesc().c_str());

    MySQL mysql;
    if (mysql.connect())
    {
        if (mysql.update(sql))
        {
            // 获取成功插入的用户数据生成的主键id并设置
            group.setId(mysql_insert_id(mysql.getConnection()));
            return true;
        }
    }

    return false;
}

// 加入群聊
void GroupModel::addGroup(int userid, int groupid, string role)
{
    // 组装sql语句
    char sql[1024] = "";
    sprintf(sql, "INSERT INTO GroupUser(userid,groupid,grouprole) VALUES(%d, %d, '%s')",
            userid, groupid, role.c_str());

    MySQL mysql;
    if (mysql.connect())
    {
        mysql.update(sql);
    }
}

// 查询用户所在群聊
vector<Group> GroupModel::queryGroups(int userid)
{
    /*
        1.根据userid查询用户所在的群聊信息
        2.查询群聊的所有用户信息
    */

    vector<Group> retvec;
    {
        // 1.根据userid查询用户所在的群聊信息

        // 组装sql
        char sql[1024] = "";
        sprintf(sql, "SELECT a.id, a.groupname, a.groupdesc "
                     "FROM AllGroup a JOIN GroupUser g "
                     "ON g.groupid = a.id "
                     "WHERE g.userid = %d",
                userid);

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

                    // 查询userid所有的群聊信息
                    Group group;
                    group.setId(atoi(row[0])); // atoi,C标准函数，字符串转整型
                    group.setName(row[1]);
                    group.setDesc(row[2]);

                    retvec.push_back(group);
                }
                // 释放结果集res
                mysql_free_result(res);
            }
        }
    }

    {
        // 2.查询群聊的所有用户信息

        for (auto &g : retvec)
        {
            // 组装sql
            char sql[1024] = "";
            sprintf(sql, "SELECT u.id, u.name, u.state, g.grouprole "
                         "FROM Users u JOIN GroupUser g "
                         "ON u.id = g.userid "
                         "WHERE g.groupid = %d",
                    g.getId());

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

                        GroupUser guser;
                        guser.setId(atoi(row[0])); // atoi,C标准函数，字符串转整型
                        guser.setName(row[1]);
                        guser.setState(row[2]);
                        guser.setRole(row[3]);

                        g.getUsers().push_back(guser);
                    }

                    // 释放结果集res
                    mysql_free_result(res);
                }
            }
        }
    }

    return retvec;
}

// 根据指定的groupid查询群组用户id列表，除userid自己，主要用户群聊业务给群组其它成员群发消息
vector<int> GroupModel::queryGroupUsers(int userid, int groupid)
{
    // 组装sql
    char sql[1024] = "";
    sprintf(sql, "SELECT userid FROM GroupUser WHERE groupid = %d AND userid != %d",
            groupid, userid);

    vector<int> retvec;
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

                retvec.push_back(atoi(row[0]));
            }
            // 释放结果集res
            mysql_free_result(res);
        }
    }

    return retvec;
}