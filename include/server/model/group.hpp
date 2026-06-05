#ifndef GROUP_H
#define GROUP_H

#include <string>
#include <vector>

#include "groupuser.hpp"

using namespace std;

class Group
{
public:
    Group(int id = -1, string groupName = "", string groupDesc = "")
        : id(id), name(groupName), desc(groupDesc) {}

    int getId() { return this->id; }
    string getName() { return this->name; }
    string getDesc() { return this->desc; }
    vector<GroupUser> &getUsers() { return this->users; }

    void setId(int id) { this->id = id; }
    void setName(string groupName) { this->name = groupName; }
    void setDesc(string groupDesc) { this->desc = groupDesc; }

private:
    int id;
    string name;
    string desc;
    vector<GroupUser> users;
};

#endif