#include "blocklist.h"
#include <cstring>
#include "error.h"
#include <iostream>

Element::Element() = default;

Element::Element(int _offset, const char *_key)
{
    offset = _offset;
    strcpy(key, _key);
}

Element::Element(const Element &o)
{
    offset = o.offset;
    memset(key, 0, sizeof(key));
    strcpy(key, o.key);
}

Element &Element::operator=(const Element &o)
{
    if (this == &o) return *this;
    offset = o.offset;
    memset(key, 0, sizeof(key));
    strcpy(key, o.key);
    return *this;
}

bool Element::operator<(const Element &o) const
{
    return strcmp(key, o.key) < 0;
}

Block::Block() = default;

Block::Block(const Block &o)
{
    pre = o.pre;
    nxt = o.nxt;
    len = o.len;
    for (int i = 0; i < len; ++i) block[i] = o.block[i];
    for (int i = len; i < maxLen; ++i) block[i] = Element();
}

Block &Block::operator=(const Block &o)
{
    if (this == &o) return *this;
    pre = o.pre;
    nxt = o.nxt;
    len = o.len;
    for (int i = 0; i < len; ++i) block[i] = o.block[i];
    for (int i = len; i < maxLen; ++i) block[i] = Element();
    return *this;
}

int BlockList::nextBlock(int offset)
{
    nextIn.open(fileName, ios::in | ios::binary);
 //   assert(nextIn);
    if (!nextIn) error("open file error");

    nextIn.seekg(offset, ios::beg);
    int next;
    nextIn.read(reinterpret_cast<char *>(&next), sizeof(int));

    nextIn.close();
    return next;
}

void BlockList::splitBlock(int offset)
{
    in.open(fileName, ios::in | ios::binary);
    out.open(fileName, ios::in | ios::out | ios::binary);
    assert(out);
    if ((!in) || (!out)) error("open file error");

    Block thisBlock, newBlock;

    in.seekg(offset, ios::beg);
    in.read(reinterpret_cast<char *>(&thisBlock), sizeof(Block));

    /* different */    thisBlock.len = newBlock.len = maxLen / 2;
    newBlock.pre = offset;
    newBlock.nxt = thisBlock.nxt;

    in.seekg(0, ios::end);
    int offset2 = in.tellg();
    if (thisBlock.nxt >= 0)
    {
        out.seekp(thisBlock.nxt + sizeof(int), ios::beg);
        out.write(reinterpret_cast<char *>(&offset2), sizeof(int));
    }
    thisBlock.nxt = offset2;

    for (int i = 0; i < maxLen / 2; ++i)
        newBlock.block[i] = thisBlock.block[i + maxLen / 2];

    out.seekp(offset2, ios::beg);
    out.write(reinterpret_cast<char *>(&newBlock), sizeof(Block));
    out.seekp(offset, ios::beg);
    out.write(reinterpret_cast<char *>(&thisBlock), sizeof(Block));

    in.close();
    out.close();
}

void BlockList::mergeBlock(int offset1, int offset2)
{
    in.open(fileName, ios::in | ios::binary);
    out.open(fileName, ios::in | ios::out | ios::binary);
    assert(out);
    if ((!in) || (!out)) error("open file error");

    Block block1, block2;

    in.seekg(offset1, ios::beg);
    in.read(reinterpret_cast<char *>(&block1), sizeof(Block));
    in.seekg(offset2, ios::beg);
    in.read(reinterpret_cast<char *>(&block2), sizeof(Block));

    block1.nxt = block2.nxt;
    if (block2.nxt >= 0)
    {
        out.seekp(block2.nxt + sizeof(int), ios::beg);
        out.write(reinterpret_cast<char *>(&offset1), sizeof(int));
    }

    for (int i = 0; i < block2.len; ++i)
        block1.block[i + block1.len] = block2.block[i];
    block1.len += block2.len;

    out.seekp(offset1, ios::beg);
    out.write(reinterpret_cast<char *>(&block1), sizeof(Block));
    deleteBlock(offset2);

    in.close();
    out.close();
}

void BlockList::deleteBlock(int offset)
{
    in.open(fileName, ios::in | ios::binary);
    out.open(fileName, ios::in | ios::out | ios::binary);
    assert(out);
    if ((!in) || (!out)) error("open file error");

    int pre, nxt;
    in.seekg(offset, ios::beg);
    in.read(reinterpret_cast<char *>(&pre), sizeof(int));
    in.read(reinterpret_cast<char *>(&nxt), sizeof(int));

    if (pre == -1)
    {
        Block temp;
        out.seekp(0, ios::beg);
        out.write(reinterpret_cast<char *>(&temp), sizeof(Block));
    } else
    {
        out.seekp(pre, ios::beg);
        out.write(reinterpret_cast<char *>(&nxt), sizeof(int));
    }

    if (nxt != -1)
    {
        out.seekp(nxt + sizeof(int), ios::beg);
        out.write(reinterpret_cast<char *>(&pre), sizeof(int));
    }

    in.close();
    out.close();
}

BlockList::BlockList() = default;

BlockList::BlockList(const string &_fileName)
{
    fileName = _fileName;
    fstream test_exist(_fileName, ios::binary | ios::in);
    if (test_exist){
        test_exist.close();
        return;
    }
    fstream creator(_fileName, ios::binary | ios::out);
    creator.close();



}

void BlockList::findElement(const char *key, vector<int> &ans)
{
    in.open(fileName, ios::in | ios::binary);
//    out.open(fileName, ios::in | ios::out | ios::binary);
//    assert(out);
//    if ((!in) || (!out)) error("open file error");
    if (!in) error("open file error");

    in.seekg(0, ios::end);
    if (in.tellg() == 0)
    {
        Block tempBlock;
        out.open(fileName, ios::in | ios::out | ios::binary);
        out.write(reinterpret_cast<char *>(&tempBlock), sizeof(Block));
        out.close();
    }

    Element nextElement;
    in.seekg(0, ios::beg);
    int curOffset = in.tellg();
    int nextOffset = nextBlock(curOffset);
    while (nextOffset != -1)
    {
        in.seekg(nextOffset + 3 * sizeof(int), ios::beg);
        in.read(reinterpret_cast<char *>(&nextElement), sizeof(Element));
        if (strcmp((const char *) key, nextElement.key) <= 0) break;
        curOffset = nextOffset;
        nextOffset = nextBlock(nextOffset);
    }

    bool flag = true;
    while (curOffset >= 0 && flag)
    {
        flag = false;

        Element tempElement(-1, key);
        Block tempBlock;

        in.seekg(curOffset, ios::beg);
        in.read(reinterpret_cast<char *>(&tempBlock), sizeof(Block));
        int pos = lower_bound(tempBlock.block, tempBlock.block + tempBlock.len, tempElement) - tempBlock.block;

        if (pos == tempBlock.len)
        {
            curOffset = nextBlock(curOffset);
            flag = true;
        }

        for (int i = pos; i < tempBlock.len; ++i)
        {
            if (strcmp(tempBlock.block[i].key, key) < 0) break;
            else if (strcmp(tempBlock.block[i].key, key) > 0) continue;
            ans.push_back(tempBlock.block[i].offset);
            if (i == tempBlock.len - 1)
            {
                curOffset = nextBlock(curOffset);
                flag = true;
            }
        }
    }

    in.close();
}

void BlockList::addElement(const Element &o)
{
    in.open(fileName, ios::in | ios::binary);
    out.open(fileName, ios::in | ios::out | ios::binary);
    assert(out);
    if ((!in) || (!out)) error("open file error");

    Block tempBlock;

    in.seekg(0, ios::end);
    if (in.tellg() == 0)
        out.write(reinterpret_cast<char *>(&tempBlock), sizeof(Block));

    Element nextElement;
    in.seekg(0, ios::beg);
    int curOffset = in.tellg();
    int nextOffset = nextBlock(curOffset);
    while (nextOffset != -1)
    {
        in.seekg(nextOffset + 3 * sizeof(int), ios::beg);
        in.read(reinterpret_cast<char *>(&nextElement), sizeof(Element));
        if (o < nextElement) break;
        curOffset = nextOffset;
        nextOffset = nextBlock(nextOffset);
    }

    int pos;
    bool flag = true;
    in.seekg(curOffset, ios::beg);
    in.read(reinterpret_cast<char *>(&tempBlock), sizeof(Block));
    for (int i = 0; i < tempBlock.len; ++i)
    {
        if (o < tempBlock.block[i])
        {
            flag = false;
            pos = i;
            break;
        }
    }

    if (!flag)
    {
        for (int i = tempBlock.len; i > pos; --i)
            tempBlock.block[i] = tempBlock.block[i - 1];
        tempBlock.block[pos] = o;
    } else tempBlock.block[tempBlock.len] = o;

    tempBlock.len++;

    out.seekp(curOffset, ios::beg);
    out.write(reinterpret_cast<char *>(&tempBlock), sizeof(Block));

    in.close();
    out.close();

    if (tempBlock.len == maxLen) splitBlock(curOffset);
}

void BlockList::deleteElement(const Element &o)
{
    in.open(fileName, ios::in | ios::binary);
    out.open(fileName, ios::in | ios::out | ios::binary);
    assert(out);
    if ((!in) || (!out)) error("open file error");

    Block tempBlock;

    in.seekg(0, ios::end);
    if (in.tellg() == 0) out.write(reinterpret_cast<char *>(&tempBlock), sizeof(Block));

    Element nextElement;
    in.seekg(0, ios::beg);
    int curOffset = in.tellg();
    int nextOffset = nextBlock(curOffset);
    while (nextOffset != -1)
    {
        in.seekg(nextOffset + 3 * sizeof(int), ios::beg);
        in.read(reinterpret_cast<char *>(&nextElement), sizeof(Element));
        if (strcmp(o.key, nextElement.key) <= 0) break;
        curOffset = nextOffset;
        nextOffset = nextBlock(nextOffset);
    }

    int pos = -1;
    while (pos == -1 && curOffset >= 0)
    {
        in.seekg(curOffset, ios::beg);
        in.read(reinterpret_cast<char *>(&tempBlock), sizeof(Block));

        int minPos = lower_bound(tempBlock.block, tempBlock.block + tempBlock.len, o) - tempBlock.block;
        for (int i = minPos; i < tempBlock.len; ++i)
        {
            if (tempBlock.block[i].offset == o.offset && strcmp(tempBlock.block[i].key, o.key) == 0)
            {
                pos = i;
                break;
            }
        }
        if (pos == -1)
        {
            curOffset = nextBlock(curOffset);
            if (curOffset < 0) error("Invalid");
        }
    }

    for (int i = pos; i < tempBlock.len; ++i)
        tempBlock.block[i] = tempBlock.block[i + 1];
    tempBlock.len--;

    out.seekp(curOffset, ios::beg);
    out.write(reinterpret_cast<char *>(&tempBlock), sizeof(Block));

    int nextLen;
    nextOffset=nextBlock(curOffset);
    if (nextOffset >= 0)
    {
        in.seekg(nextOffset + 2 * sizeof(int), ios::beg);
        in.read(reinterpret_cast<char *>(&nextLen), sizeof(int));
    }

    in.close();
    out.close();

    if (nextOffset >= 0 && tempBlock.len + nextLen < maxLen) mergeBlock(curOffset, nextOffset);
}

