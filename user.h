#include <string>
#include <vector>
#include "blocklist.h"

using namespace std;

#ifndef BOOKSTORE_USER_H
#define BOOKSTORE_USER_H

class User
{
public:
    char ID[31] = "root";
    char name[31] = "root";
    char password[31] = "sjtu";
    int privilege = -1;

    User();

    User(string &id, string &_name, string &_password, int _privilege);

};

class UserManager
{
private:
    string fileName;
    fstream in, out;

public:
    int userNumber;
    vector<User> userLog;
    BlockList userID;

    UserManager();

    bool checkPrivilege(int _privilege);

    void login(string &id, string &_password);

    void logout();

    void useradd(string &id, string &_password, int _privilege, string &_name);

    void Register(string &id, string &_password, string &_name);

    void Delete(string &id);

    void changePassword(string &id, string &password1, string &password2);

};

#endif //BOOKSTORE_USER_H
