#ifndef USERMODEL_H
#define USERMODEL_H

#include "user.hpp"

class UserModel
{
public:
    // User表增加的方法
    bool insert(User &user);

    // User表查询的方法：根据用户号码查询用户信息
    User query(int id);

    // User表更新用户状态
    bool updateState(User user);

    // 重置用户的状态信息
    void resetState();
};

#endif