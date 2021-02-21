#include <string>
#include <fstream>
#include <vector>

using namespace std;

#ifndef BOOKSTORE_BLOCKLIST_H
#define BOOKSTORE_BLOCKLIST_H

const int maxLen = 500;

class Element
{
public:
    int offset = -1;
    char key[61] = {0};

    Element();

    Element(int _offset, const char *_key);

    Element(const Element &o);

    Element &operator=(const Element &o);

    bool operator<(const Element &o) const;

};

class Block
{
public:
    int pre = -1;
    int nxt = -1;
    int len = 0;
    Element block[500];

    Block();

    Block(const Block &o);

    Block &operator=(const Block &o);

};

class BlockList
{
private:
    string fileName;
    fstream in, out, nextIn;

private:
    int nextBlock(int offset);

    void splitBlock(int offset);

    void mergeBlock(int offset1, int offset2);

    void deleteBlock(int offset);

public:
    BlockList();

    explicit BlockList(const string &_fileName);

    void findElement(const char *key, vector<int> &ans);

    void addElement(const Element &o);

    void deleteElement(const Element &o);

};

#endif //BOOKSTORE_BLOCKLIST_H
