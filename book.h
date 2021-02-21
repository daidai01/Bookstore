#include <string>
#include "blocklist.h"

using namespace std;

#ifndef BOOKSTORE_BOOK_H
#define BOOKSTORE_BOOK_H

class Book
{
    friend class BookManager;

private:
    char ISBN[21] = {0};
    char name[61] = {0};
    char author[61] = {0};
    char keyword[61] = {0};
    int quantity = 0;
    double price = 0;

public:
    Book();

    Book(string &isbn, string &_name, string &_author, string &_keyword, int _quantity, double _price);

    Book(const Book &o);

    bool operator<(const Book &o) const;

    void show();

};

class BookManager
{
private:
    int bookNumber = 0;
    double cost = 0;
    double income = 0;
    int transactionNumber = 0;


    BlockList bookISBN;
    BlockList bookName;
    BlockList bookAuthor;
    BlockList bookKeyword;

public:
    vector<int> selectedBook;

    BookManager();

    void splitKeyword(const string &keyword, vector<string> &keys);

    void select(string &isbn);

    void modify(string str[]);

    void import(int _quantity, double _cost);

    void show(string str[]);

    void showFinance(int _time);

    void buy(string &isbn, int _quantity);
};

#endif //BOOKSTORE_BOOK_H
