#ifndef GROUPMODEL_H
#define GROUPMODEL_H

#include <string>
#include <vector>

#include "group.hpp"

using namespace std;

class GroupModel
{
public:
    // 创建群聊
    bool createGroup(Group &group);
    // 加入群聊
    void addGroup(int userid, int groupid, string role);
    // 查询用户所在群聊
    vector<Group> queryGroups(int userid);
    // 发送信息到群聊
    vector<int> queryGroupUsers(int userid, int groupid);
};

#endif