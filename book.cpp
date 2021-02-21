#include "book.h"
#include <cstring>
#include <iostream>
#include <iomanip>
#include "program.h"
#include "error.h"
#include <algorithm>
#include <sstream>

using namespace std;

extern UserManager userManager;
extern BookManager bookManager;
extern Account account;

Book::Book() = default;

Book::Book(string &isbn, string &_name, string &_author, string &_keyword, int _quantity, double _price)
{
    quantity = _quantity;
    price = _price;
    strcpy(ISBN, isbn.c_str());
    strcpy(name, _name.c_str());
    strcpy(author, _author.c_str());
    strcpy(keyword, _keyword.c_str());
}

Book::Book(const Book &o)
{
    quantity = o.quantity;
    price = o.price;
    strcpy(ISBN, o.ISBN);
    strcpy(name, o.name);
    strcpy(author, o.author);
    strcpy(keyword, o.keyword);
}

bool Book::operator<(const Book &o) const
{
    return strcmp(ISBN, o.ISBN) < 0;
}

void Book::show()
{
    cout << ISBN << '\t' << name << '\t' << author << '\t' << keyword << '\t';
    cout << setiosflags(ios::fixed) << setprecision(2) << price << '\t' << quantity << endl;
}

BookManager::BookManager()
        : bookISBN(BOOKISBN_INDEXFILE), bookName(BOOKNAME_INDEXFILE), bookAuthor(BOOKAUTHOR_INDEXFILE),
          bookKeyword(BOOKKEYWORD_INDEXFILE)
{}

void BookManager::splitKeyword(const string &keyword, vector<string> &keys)
{
    stringstream ss(keyword);
    string temp;
    while (getline(ss, temp, '|'))keys.push_back(temp);
}

void BookManager::select(string &isbn)
{
    if (!userManager.checkPrivilege(3)) error("Invalid");
    if (isbn.length() > 20) error("Invalid");

    Book tempBook;
    vector<int> Ans;
    bookISBN.findElement(isbn.c_str(), Ans);
    if (!Ans.empty()) selectedBook[selectedBook.size() - 1] = Ans[0];
    else
    {
        strcpy(tempBook.ISBN, isbn.c_str());
        int offset = writeOtherData<Book>(BOOK, tempBook);
        selectedBook[selectedBook.size() - 1] = offset;

        bookNumber++;
        writeBasicData<int>(BOOKNUMBER, bookNumber);

        Element newElement(offset, isbn.c_str());
        bookISBN.addElement(newElement);
    }
}

void BookManager::modify(string str[])
{
    if (!userManager.checkPrivilege(3))error("Invalid");

  //  cout<<selectedBook.size()<<endl;
 //   if (selectedBook.size()<=1) error("Invalid");

    int offset = selectedBook[selectedBook.size() - 1];
    Book tempBook = readOtherData<Book>(BOOK, offset);

    for (int i = 0; i < 5; ++i)
    {
        if (str[i].empty()) break;

        if (str[i][1] == 'I')
        {
            string newISBN = str[i].substr(6, str[i].size() - 6);
            if (newISBN.length() > 20) error("Invalid");

            vector<int> Ans;
            bookISBN.findElement(newISBN.c_str(), Ans);
            if (!Ans.empty()) error("Invalid");

            Element oldElement(offset, tempBook.ISBN);
            bookISBN.deleteElement(oldElement);
            Element newElement(offset, newISBN.c_str());
            bookISBN.addElement(newElement);

            strcpy(tempBook.ISBN, newISBN.c_str());
            writeOtherData<Book>(BOOK, tempBook, offset);
        } else if (str[i][1] == 'n')
        {
            string newName = str[i].substr(7, str[i].size() - 8);
            if (newName.length() > 60) error("Invalid");

            string oldName = tempBook.name;
            if (!oldName.empty())
            {
                Element oldElement(offset, tempBook.name);
                bookName.deleteElement(oldElement);
            }
            Element newElement(offset, newName.c_str());
            bookName.addElement(newElement);

            strcpy(tempBook.name, newName.c_str());
            writeOtherData<Book>(BOOK, tempBook, offset);
        } else if (str[i][1] == 'a')
        {
            string newAuthor = str[i].substr(9, str[i].size() - 10);
            if (newAuthor.length() > 60) error("Invalid");

            string oldAuthor = tempBook.author;
            if (!oldAuthor.empty())
            {
                Element oldElement(offset, tempBook.author);
                bookAuthor.deleteElement(oldElement);
            }
            Element newElement(offset, newAuthor.c_str());
            bookAuthor.addElement(newElement);

            strcpy(tempBook.author, newAuthor.c_str());
            writeOtherData<Book>(BOOK, tempBook, offset);
        } else if (str[i][1] == 'k')
        {
            string newKeyword = str[i].substr(10, str[i].size() - 11);
            if (newKeyword.length() > 60) error("Invalid");
            string oldKeyword = tempBook.keyword;

            vector<string> Old, New;
            splitKeyword(oldKeyword, Old);
            splitKeyword(newKeyword, New);
            sort(New.begin(), New.end());
            for (int n = 0; n < New.size() - 1; ++n)
                if (New[n] == New[n + 1]) error("Invalid");

            if (!oldKeyword.empty())
            {
                for (int j = 0; j < Old.size(); ++j)
                {
                    Element oldElement(offset, Old[i].c_str());
                    bookKeyword.deleteElement(oldElement);
                }
            }

         //   cout<<"*"<<New.size()<<endl;
            for (int j = 0; j < New.size(); ++j)
            {
                Element newElement(offset, New[i].c_str());
                bookKeyword.addElement(newElement);
            }

            strcpy(tempBook.keyword, newKeyword.c_str());
            writeOtherData<Book>(BOOK, tempBook, offset);
        } else if (str[i][1] == 'p')
        {
            double newPrice = atof(str[i].substr(7, str[i].size() - 7).c_str());

            tempBook.price = newPrice;
            writeOtherData<Book>(BOOK, tempBook, offset);
        } else error("Invalid");
    }
}

void BookManager::import(int _quantity, double _cost)
{
    if (!userManager.checkPrivilege(3)) error("Invalid");
    if(_quantity>1000000) error("Invalid");

   // if (selectedBook.size()<=1) error("Invalid");
    int offset = selectedBook[selectedBook.size() - 1];
    Book tempBook = readOtherData<Book>(BOOK, offset);

    tempBook.quantity += _quantity;
    writeOtherData<Book>(BOOK, tempBook, offset);

    transactionNumber++;
    writeBasicData<int>(TRANSACTION, transactionNumber);

    cost += _cost;
    writeBasicData<double>(COST, cost);

    string isbn=readOtherData<Book>(BOOK,offset).ISBN;
    string id=userManager.userLog[userManager.userLog.size()-1].ID;
    Account tempAccount(id,isbn,-_quantity,_cost);
    fstream file;
    file.open(FINANCE_FILE,ios::in|ios::out|ios::binary);
    file.seekp(0,ios::end);
    file.write(reinterpret_cast<const char *>(&tempAccount),sizeof(Account));
    file.close();
}

void BookManager::show(string str[])
{
    if (!userManager.checkPrivilege(1)) error("Invalid");

    vector<Book> Ans;

    if (str[0].empty())
    {
        for (int j = 0; j < bookNumber; ++j)
        {
            Book tempBook = readOtherData<Book>(BOOK, j * sizeof(Book));
            Ans.push_back(tempBook);
        }
    } else
    {
        vector<int> offsetAns;

        if (str[0][1] == 'I')
        {
            string key = str[0].substr(6, str[0].size() - 6);
            bookISBN.findElement(key.c_str(), offsetAns);
        } else if (str[0][1] == 'n')
        {
            string key = str[0].substr(7, str[0].size() - 8);
            bookName.findElement(key.c_str(), offsetAns);
        } else if (str[0][1] == 'a')
        {
            string key = str[0].substr(9, str[0].size() - 10);
            bookAuthor.findElement(key.c_str(), offsetAns);
        } else if (str[0][1] == 'k')
        {
            string key = str[0].substr(10, str[0].size() - 11);
            bookKeyword.findElement(key.c_str(), offsetAns);
           // cout<<offsetAns.size()<<endl;

        } else error("Invalid");

        if (offsetAns.empty()) cout << '\n';
        else
        {
            sort(offsetAns.begin(),offsetAns.end());
            vector<int>::iterator it=offsetAns.begin();
            while(it!=offsetAns.end())
            {
                if(count(offsetAns.begin(),offsetAns.end(),*it)>1)
                    it=offsetAns.erase(it);
                else  it++;
            }
            for (int j = 0; j < offsetAns.size(); ++j)
            {
                Book tempBook = readOtherData<Book>(BOOK, offsetAns[j]);
                Ans.push_back(tempBook);
            }
        }
    }

    sort(Ans.begin(), Ans.end());
    for (int j = 0; j < Ans.size(); ++j) Ans[j].show();
}

void BookManager::showFinance(int _time)
{
    if (!userManager.checkPrivilege(7)) error("Invalid");

    if (_time == 0)
        cout << setiosflags(ios::fixed) << setprecision(2) << "+ " << income << " - " << cost << '\n';
    else
    {
        if (_time > transactionNumber) error("Invalid");

        double _cost = 0, _income = 0;

        fstream file;
        file.open(FINANCE_FILE, ios::in | ios::out | ios::binary);
        file.seekg((transactionNumber - _time) * sizeof(Account), ios::beg);
        Account tempAccount;
        for (int i = transactionNumber - _time; i < transactionNumber; ++i)
        {
            file.read(reinterpret_cast<char *>(&tempAccount), sizeof(Account));
            if (tempAccount.quantity > 0) _income += tempAccount.quantity*tempAccount.price;
            else _cost += tempAccount.price;
        }
        file.close();

        cout << setiosflags(ios::fixed) << setprecision(2) << "+ " << _income << " - " << _cost << '\n';
    }
}

void BookManager::buy(string &isbn, int _quantity)
{
    if (!userManager.checkPrivilege(1)) error("Invalid");
  //  if(selectedBook.size()<=1) error("Invalid");
    if (isbn.length() > 20) error("Invalid");
    if (_quantity > 1000000) error("Invalid");

    vector<int> Ans;
    bookISBN.findElement(isbn.c_str(), Ans);
    if (Ans.empty()) error("Invalid");
    Book tempBook = readOtherData<Book>(BOOK, Ans[0]);
    if (tempBook.quantity < _quantity) error("Invalid");

    tempBook.quantity -= _quantity;
    writeOtherData<Book>(BOOK, tempBook, Ans[0]);

    transactionNumber++;
    writeBasicData<int>(TRANSACTION, transactionNumber);

    income += _quantity * tempBook.price;
    writeBasicData<double>(INCOME, income);
    cout << setiosflags(ios::fixed) << setprecision(2) << _quantity * tempBook.price << endl;

    fstream file;
    file.open(FINANCE_FILE, ios::in | ios::out | ios::binary);
    file.seekp(0, ios::end);
    User tempUser = userManager.userLog[userManager.userNumber - 1];
 //   cout<<tempUser.ID<<endl;
    string id=tempUser.ID;
    Account tempAccount(id, isbn, _quantity, tempBook.price);
    file.write(reinterpret_cast<const char *>(&tempAccount), sizeof(Account));
}

