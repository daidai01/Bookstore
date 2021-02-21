#include "program.h"
#include "error.h"
#include "user.h"
#include "book.h"
#include <iostream>
#include <cstring>
#include <iomanip>

using namespace std;

BookManager bookManager;
UserManager userManager;
Account account;


Account::Account() = default;

Account::Account(string &id, string &isbn, int _quantity, double _price)
{
    quantity = _quantity;
    price = _price;
    strcpy(userID, id.c_str());
    strcpy(ISBN, isbn.c_str());
}

void init()
{
    fstream file;

    file.open(BASIC_FILE, ios::out);
    file.close();
    file.open(LOG_FILE, ios::out);
    file.close();
    file.open(USER_FILE, ios::out);
    file.close();
    file.open(BOOK_FILE, ios::out);
    file.close();
    file.open(FINANCE_FILE, ios::out);
    file.close();
    file.open(USERID_INDEXFILE, ios::out);
    file.close();
    file.open(BOOKISBN_INDEXFILE, ios::out);
    file.close();
    file.open(BOOKAUTHOR_INDEXFILE, ios::out);
    file.close();
    file.open(BOOKNAME_INDEXFILE, ios::out);
    file.close();
    file.open(BOOKKEYWORD_INDEXFILE, ios::out);
    file.close();

    file.open(BASIC_FILE, ios::in | ios::out | ios::binary);
    int m = 0;
    double n = 0;
    file.write(reinterpret_cast<char *>(&m), sizeof(int));
    file.write(reinterpret_cast<char *>(&m), sizeof(int));
    file.write(reinterpret_cast<char *>(&n), sizeof(double));
    file.write(reinterpret_cast<char *>(&n), sizeof(double));
    file.write(reinterpret_cast<char *>(&m), sizeof(int));
    file.close();

    file.open(USER_FILE, ios::in | ios::out | ios::binary);
    User root;
    strcpy(root.ID, "root");
    strcpy(root.name, "root");
    strcpy(root.password, "sjtu");
    root.privilege = 7;
    int offset=writeOtherData<User>(USER, root);
    Element tempElement(offset,"root");
    userManager.userID.addElement(tempElement);
 //   userManager.userLog.push_back(root);
  //  userManager.userNumber++;
  //  for(int i=0;i<userManager.userNumber;++i) cout<<userManager.userLog[i].privilege<<endl;
    file.close();
}

void splitLine(string &cmd,string &line, string str[])
{
    int i = 0, j = 0;
    while (i < line.length())
    {
        if(i==0)
        {
            while (line[i] != ' ' && i < line.length())
            {
                cmd+=line[i];
                i++;
            }
            i++;
            continue;
        }
        while (line[i] != ' ' && i < line.length())
        {
            str[j] += line[i];
            i++;
        }
        i++;
        j++;
    }
}

void runProgram(string &line)
{
    string cmd, str[5];
    splitLine(cmd,line, str);
 //   cout<<cmd<<endl;
   // cout<<str[0]<<endl;

    if (cmd == "quit" || cmd == "exit") exit(0);
    else if (cmd == "su")
    {
        if (str[0].empty() || !str[2].empty() || !str[3].empty() || !str[4].empty()) error("Invalid");
        userManager.login(str[0], str[1]);
    } else if (cmd == "logout")
    {
        if (!str[0].empty() || !str[1].empty() || !str[2].empty() || !str[3].empty() || !str[4].empty())
            error("Invalid");
        userManager.logout();
    } else if (cmd == "useradd")
    {
        if (str[0].empty() || str[1].empty() || str[2].empty() || str[3].empty() || !str[4].empty()) error("Invalid");
        int _privilege = atoi(str[2].c_str());
        userManager.useradd(str[0], str[1], _privilege, str[3]);
    } else if (cmd == "register")
    {
        if (str[0].empty() || str[1].empty() || str[2].empty() || !str[3].empty() || !str[4].empty()) error("Invalid");
        userManager.Register(str[0], str[1], str[2]);
    } else if (cmd == "delete")
    {
        if (str[0].empty() || !str[1].empty() || !str[2].empty() || !str[3].empty() || !str[4].empty())
            error("Invalid");
        userManager.Delete(str[0]);
    } else if (cmd == "passwd")
    {
        if (str[0].empty() || str[1].empty() || !str[3].empty() || !str[4].empty()) error("Invalid");
        userManager.changePassword(str[0], str[1], str[2]);
    } else if (cmd == "select")
    {
        if (str[0].empty() || !str[1].empty() || !str[2].empty() || !str[3].empty() || !str[4].empty())
            error("Invalid");
        bookManager.select(str[0]);
    } else if (cmd == "modify")
    {
        if (str[0].empty()) error("Invalid");
        else bookManager.modify(str);
    } else if (cmd == "import")
    {
        if (str[0].empty() || str[1].empty() || !str[2].empty() || !str[3].empty() || !str[4].empty()) error("Invalid");
        int _quantity = atoi(str[0].c_str());
        double _cost = atof(str[1].c_str());
        bookManager.import(_quantity, _cost);
    } else if (cmd == "show")
    {
        if (str[0] == "finance")
        {
            if (!str[2].empty() || !str[3].empty() || !str[4].empty()) error("Invalid");
            int time = atoi(str[1].c_str());
            bookManager.showFinance(time);
        } else
        {
            if (!str[1].empty() || !str[2].empty() || !str[3].empty() || !str[4].empty()) error("Invalid");
            bookManager.show(str);
        }
    } else if (cmd == "buy")
    {
        if (str[0].empty() || str[1].empty() || !str[2].empty() || !str[3].empty() || !str[4].empty()) error("Invalid");
        int _quantity = atoi(str[1].c_str());
        bookManager.buy(str[0], _quantity);
    } else if (cmd == "log")
    {
        if (!str[0].empty() || !str[1].empty() || !str[2].empty() || !str[3].empty() || !str[4].empty())
            error("Invalid");
        log();
    } else if (cmd == "report")
    {
        if (str[0].empty() || !str[1].empty() || !str[2].empty() || !str[3].empty() || !str[4].empty())
            error("Invalid");
        if (str[0] == "finance") reportFinance();
        else if (str[0] == "employee") reportEmployee();
        else if (str[0] == "myself") reportMyself();
        else error("Invalid");
    } else error("Invalid");
}

void reportFinance()
{
    if (!userManager.checkPrivilege(7)) error("Invalid");

    cout << "REPORT FINANCE" << endl;
    int transactionNumber = readBasicData<int>(TRANSACTION);
    double cost = readBasicData<double>(COST);
    double income = readBasicData<double>(INCOME);
    double profit = income - cost;
    cout << "number of transactions: " << transactionNumber << endl;
    cout << setiosflags(ios::fixed) << setprecision(2);
    cout << "total expenditure: " << cost << endl;
    cout << "total income: " << income << endl;
    if (profit >= 0) cout << "profit: +" << profit << endl;
    else cout << "profit: -" << -profit << endl;
    cout << "Details:" << endl;

    Account tempAccount;

    fstream file;
    file.open(FINANCE_FILE, ios::in | ios::binary);
    for (int i = 0; i < transactionNumber; ++i)
    {
        file.read(reinterpret_cast<char *>(&tempAccount), sizeof(Account));
        cout << tempAccount.time << " ";
        if (tempAccount.quantity > 0) cout << "buy ";
        else cout << "import ";
        cout << tempAccount.userID << " " << tempAccount.ISBN << " " << abs(tempAccount.quantity) << " "
             << tempAccount.price << endl;
    }

    file.close();
}

void reportEmployee()
{
    if (!userManager.checkPrivilege(7)) error("Invalid");

    cout << "REPORT EMPLOYEE" << endl;
}

void log()
{
    if (!userManager.checkPrivilege(7)) error("Invalid");

    cout << "LOG" << endl;
    string log;
    fstream file;
    file.open(LOG_FILE, ios::in);
    while (getline(file, log)) cout << log << endl;

    file.close();
}

void reportMyself()
{
    if (!userManager.checkPrivilege(3)) error("Invalid");

    cout << "REPORT MYSELF" << endl;
 }



