#include <string>
#include "user.h"
#include "book.h"
#include "error.h"

#define BASIC_FILE "basic.dat"
#define LOG_FILE "log.dat"
#define USER_FILE "user.dat"
#define BOOK_FILE "book.dat"
#define FINANCE_FILE "finance.dat"
#define USERID_INDEXFILE "userID.dat"
#define BOOKISBN_INDEXFILE "bookISBN.dat"
#define BOOKAUTHOR_INDEXFILE "bookAuthor.dat"
#define BOOKNAME_INDEXFILE "bookName.dat"
#define BOOKKEYWORD_INDEXFILE "bookKeyword.dat"

using namespace std;

#ifndef BOOKSTORE_PROGRAM_H
#define BOOKSTORE_PROGRAM_H

class Account
{
    friend void reportFinance();

public:
    long long int time;
    char userID[31];
    char ISBN[21];
    int quantity; /* >0表示income,<0表示cost */
    double price;

    Account();

    Account(string &id, string &isbn, int _quantity, double _price);
};

void init();

void splitLine(string &cmd, string &line, string str[]);

void runProgram(string &cmd);

void reportFinance();

void reportEmployee();

void log();

void reportMyself();


enum basicDataType
{
    BOOKNUMBER, USERNUMBER, COST, INCOME, TRANSACTION
};

enum otherDataType
{
    BOOK, USER
};

template<class T>
T readBasicData(basicDataType type)
{
    int offset;
    if (type == BOOKNUMBER) offset = 0;
    else if (type == USERNUMBER) offset = sizeof(int);
    else if (type == COST) offset = 2 * sizeof(int);
    else if (type == INCOME) offset = 2 * sizeof(int) + sizeof(double);
    else if (type == TRANSACTION) offset = 2 * sizeof(int) + 2 * sizeof(double);
    else error("basic data type error");

    T tempT;

    fstream file;
    file.open(BASIC_FILE, ios::in | ios::binary);
    file.seekg(offset, ios::beg);
    file.read(reinterpret_cast<char *>(&tempT), sizeof(T));

    file.close();
    return tempT;
}

template<class T>
void writeBasicData(basicDataType type, const T &o)
{
    int offset;
    if (type == BOOKNUMBER) offset = 0;
    else if (type == USERNUMBER) offset = sizeof(int);
    else if (type == COST) offset = 2 * sizeof(int);
    else if (type == INCOME) offset = 2 * sizeof(int) + sizeof(double);
    else if (type == TRANSACTION) offset = 2 * sizeof(int) + 2 * sizeof(double);
    else error("basic data type error");

    fstream file;
    file.open(BASIC_FILE, ios::in | ios::out | ios::binary);
    file.seekp(offset, ios::beg);
    file.write(reinterpret_cast<const char *>(&o), sizeof(T));

    file.close();
}

template<class T>
T readOtherData(otherDataType type, int offset)
{
    T tempT;

    fstream file;
    if (type == BOOK) file.open(BOOK_FILE, ios::in | ios::binary);
    else if (type == USER) file.open(USER_FILE, ios::in | ios::binary);
    else error("other data type error");
    file.seekg(offset, ios::beg);
    file.read(reinterpret_cast<char *>(&tempT), sizeof(T));

    file.close();
    return tempT;
}

template<class T>
int writeOtherData(otherDataType type, const T &o, int offset=-1)
{
    fstream file;
    if (type == BOOK) file.open(BOOK_FILE, ios::in | ios::out | ios::binary);
    else if (type == USER) file.open(USER_FILE, ios::in | ios::out | ios::binary);
    else error("other data type error");
    if (offset == -1)
    {
        file.seekp(0, ios::end);
        offset = file.tellp();
    } else file.seekp(offset, ios::beg);
    file.write(reinterpret_cast<const char *>(&o), sizeof(T));

    file.close();
    return offset;
}


#endif //BOOKSTORE_PROGRAM_H
