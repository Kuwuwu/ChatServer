#ifndef GROUPUSER_H
#define GROUPUSER_H

#include "user.hpp"
#include <string>

using namespace std;

class GroupUser : public User
{
public:
    // 返回角色
    string getRole() { return this->role; }

    // 设置角色
    void setRole(string role) { this->role = role; }

private:
    string role;
};

#endif