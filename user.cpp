#include "user.h"
#include <cstring>
#include <iostream>
#include "program.h"
#include "error.h"
#include <vector>

using namespace std;

extern BookManager bookManager;
extern UserManager userManager;
extern Account account;


User::User() = default;

User::User(string &id, string &_name, string &_password, int _privilege)
{
    privilege = _privilege;
    strcpy(ID, id.c_str());
    strcpy(name, _name.c_str());
    strcpy(password, _password.c_str());
}

UserManager::UserManager() : userID(USERID_INDEXFILE)
{
    userNumber = 1;
    User nowUser;
    nowUser.privilege = 0;
    userLog.clear();
    userLog.push_back(nowUser);

    fileName = "user.dat";
    in.open(fileName, ios::in | ios::binary);
    out.open(fileName, ios::in | ios::out | ios::binary);

    if (!in.is_open())
    {
        nowUser.privilege = 7;
        char ch1[5] = "root", ch2[5] = "sjtu";
        strcpy(nowUser.ID, ch1);
        strcpy(nowUser.name, ch1);
        strcpy(nowUser.password, ch2);
        out.write(reinterpret_cast<char *>(&nowUser), sizeof(User));
        userID.addElement(Element(0, "root"));
    }

    in.close();
    out.close();
}

bool UserManager::checkPrivilege(int _privilege)
{
 //   auto debug = userLog[userNumber - 1];
    return _privilege <= userLog[userNumber - 1].privilege;
}

void UserManager::login(string &id, string &_password)
{
    vector<int> Ans;
    userID.findElement(id.c_str(), Ans);
    if (Ans.empty())
    {
        error("Invalid");
    }

    User tempUser(readOtherData<User>(USER, Ans[0]));

    if (_password.empty())
    {
        if (!checkPrivilege(tempUser.privilege + 1)) error("Invalid");
        userLog.push_back(tempUser);
        bookManager.selectedBook.push_back(-1);
        userNumber++;
    } else
    {
        if (_password != tempUser.password) error("Invalid");
        userLog.push_back(tempUser);
        bookManager.selectedBook.push_back(-1);
        userNumber++;
    }
 //   cout<<userManager.userLog.size()<<endl;
}

void UserManager::logout()
{
    if(!checkPrivilege(1)) error("Invalid");
    userNumber--;
    userLog.erase(userLog.end() - 1);

    bookManager.selectedBook.erase(bookManager.selectedBook.end() - 1);

//    for(int i=0;i<userManager.userNumber;++i) cout<<userManager.userLog[i].privilege<<endl;
}

void UserManager::useradd(string &id, string &_password, int _privilege, string &_name)
{
    if (!checkPrivilege(_privilege * 2 + 1)) error("Invalid");
    if (id.length() > 30 || _password.length() > 30 || _name.length() > 30) error("Invalid");

    vector<int> Ans;
    userID.findElement(id.c_str(), Ans);
    if (!Ans.empty()) error("Invalid");

    User newUser;
    newUser.privilege = _privilege;
    strcpy(newUser.ID, id.c_str());
    strcpy(newUser.password, _password.c_str());
    strcpy(newUser.name, _name.c_str());

    int offset=writeOtherData<User>(USER,newUser);
    Element newElement(offset, id.c_str());
    userID.addElement(newElement);
}

void UserManager::Register(string &id, string &_password, string &_name)
{
//    useradd(id, _password, 1, _name);
    if (id.length() > 30 || _password.length() > 30 || _name.length() > 30) error("Invalid");

    vector<int> Ans;
    userID.findElement(id.c_str(), Ans);
    if (!Ans.empty()) error("Invalid");

    User newUser;
    newUser.privilege = 1;
    strcpy(newUser.ID, id.c_str());
    strcpy(newUser.password, _password.c_str());
    strcpy(newUser.name, _name.c_str());

    int offset=writeOtherData<User>(USER,newUser);
    Element newElement(offset, id.c_str());
    userID.addElement(newElement);
}

void UserManager::Delete(string &id)
{
    if (!checkPrivilege(7) || id == "root") error("Invalid");
    if (id.length() > 30) error("Invalid");

    vector<int> Ans;
    userID.findElement(id.c_str(), Ans);
    if (Ans.empty())
    {
        error("Invalid");
      //  throw
    }

    for(int i=0;i<userNumber;++i)
    {
        string s=userLog[i].ID;
        if(s==id) error("Invalid");
    }

    Element tempElement(Ans[0], id.c_str());
    userID.deleteElement(tempElement);
}

void UserManager::changePassword(string &id, string &password1, string &password2)
{
    if (!checkPrivilege(1)) error("Invalid");
    if(id.length()>30||password1.length()>30||password2.length()>30) error("Invalid");

    vector<int> Ans;
    userID.findElement(id.c_str(), Ans);
    if (Ans.empty()) error("Invalid");

    User tempUser = readOtherData<User>(USER, Ans[0]);

    if (password2.empty())
    {
        if (userLog[userNumber - 1].privilege != 7) error("Invalid");
        strcpy(tempUser.password, password1.c_str());
    } else
    {
        if (tempUser.password != password1) error("Invalid");
        strcpy(tempUser.password, password2.c_str());
    }

    writeOtherData<User>(USER, tempUser, Ans[0]);
}

