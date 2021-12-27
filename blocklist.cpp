#include "blocklist.h"

//Node:
Node::Node() {
    offset = -1;
    memset(str, '\0', sizeof(str));
}

Node::Node(const int &_offset, const string &arg) {
    offset = _offset;
    memset(str, '\0', sizeof(str));
    strcpy(str, arg.c_str());
}

Node &Node::operator=(const Node &arg) {
    offset = arg.offset;
    strcpy(str, arg.str);
    return *this;
}

bool Node::operator==(const Node &arg) const {
    if (arg.offset != offset)return 0;
    string s1 = arg.str, s2 = str;
    return s1 == s2;
}

bool Node::operator<(const Node &arg) const {
    string s1 = str, s2 = arg.str;
    return s1 < s2;
}

bool Node::operator<=(const Node &arg) const {
    string s1 = str, s2 = arg.str;
    return s1 <= s2;
}

//Block:
Block::Block() {
    nxt = -1;
    Node_num = 0;
    //for(int i=0;i<321;++i)node[i]=Node();这个没必要，还费时间，所以给引掉了
}

Block &Block::operator=(const Block &arg) {
    nxt = arg.nxt;
    Node_num = arg.Node_num;
    for (int i = 0; i < Node_num; ++i)node[i] = arg.node[i];
    for (int i = Node_num; i < BLOCK_SIZE; ++i)node[i] = Node();
    return *this;
}

//blocklist
blocklist::blocklist(const string &arg) : fname(arg) {
    fs.open(fname, ALL);
    if (!fs.is_open()) {
        fs.close();
        fs.clear();
        fs.open(fname, ALL | ios::trunc);//trunc会新建本地文件
        Block InitBlock;//在刚开始写入一个空块
        InitBlock.Node_num = 0;
        InitBlock.nxt = -1;
        for (int i = 0; i < BLOCK_SIZE; ++i)InitBlock.node[i] = Node();
        fs.seekp(0, ios::beg);
        fs.write(reinterpret_cast<char *>(&InitBlock), sizeof(InitBlock));
    }
    fs.close();
    fs.clear();//这个是和学长学的 如果没有文件新建文件；
}

blocklist::~blocklist() {
    fs.close();
    fs1.close();
}

inline int blocklist::next(const int &offset) {//找到下一个块的offset
    fs.open(fname, ALL);
    fs.clear();
    fs.seekg(offset, ios::beg);
    int nxt_;
    fs.read(reinterpret_cast<char *>(&nxt_), sizeof(int));
    fs.close();
    fs.clear();
    return nxt_;
}


int blocklist::find_Block(const string &arg) {//找到要插入的点的位置,返回其在文件中的偏移量
    fs.open(fname, ALL);
    fs.clear();
    fs.seekg(0, ios::beg);
    int nxt_ = 0, now = 0, last = 0;
    char *temp = new char[64];
    string get_temp;
    while (nxt_ != -1) {
        last = now;
        now = nxt_;
        fs.read(reinterpret_cast<char *>(&nxt_), sizeof(int));//读取这个block的nxt数据
        Block tempB;
        fs.seekg(0, ios::beg);
        fs.read(reinterpret_cast<char *>(&tempB), sizeof(tempB));
        fs.clear();
        fs.seekg(now + 12, ios::beg);
        fs.read(temp, 64);
        get_temp = temp;//读取这个block中第一个Node的str值用于比较

        if (arg < get_temp)break;
        else if (nxt_ == -1) {
            last = now;
            break;
        }
        fs.clear();
        fs.seekg(nxt_, ios::beg);
    }
    delete[] temp;
    fs.close();
    fs.clear();
    return last;
}


int blocklist::find_Node(const string &arg) {
    int offset = find_Block(arg);//找到这个node所应该在的块的偏移量
    //取出对应Block的值
    Block tempBlock;
    fs.open(fname, ALL);
    fs.seekg(offset, ios::beg);
    fs.read(reinterpret_cast<char *>(&tempBlock), sizeof(tempBlock));
    fs.close();
    fs.clear();
    //没写二分，二分边界比较麻烦，所以就遍历找点了，因为我觉得这里log和sqrt(n)差不太多，之前在find_Block时找就有sqrt(n)的复杂度，这里换二分也简化不了很多sqrt(n)的大复杂度
    int pos = 0;
    for (pos = 0; pos < tempBlock.Node_num; ++pos) {
        string tempStr = tempBlock.node[pos].str;
        if (arg == tempStr)break;//找到就可以break，题目中说ISBN唯一
        if (arg < tempStr) {
            pos = tempBlock.Node_num;
            break;
        }//如果目前的点已经比arg字典序大了就没必要再找了，直接设pos为最后 表示匹配失败
    }
    return pos == tempBlock.Node_num ? -1 : tempBlock.node[pos].offset;
}

void blocklist::find_Node(const string &arg, vector<int> &alloc) {
    int offset = find_Block(arg);
    Block tempBlock;
    fs.open(fname, ALL);
    fs.seekg(offset, ios::beg);
    fs.read(reinterpret_cast<char *>(&tempBlock), sizeof(tempBlock));
    fs.close();
    fs.clear();
    for (int i = 0; i < tempBlock.Node_num; ++i) {
        string tempStr = tempBlock.node[i].str;
        if (arg == tempStr)alloc.push_back(tempBlock.node[i].offset);
        if (arg < tempStr)break;//理由同上
    }
}

void blocklist::merge(const int &offset1, const int &offset2) {
    Block tempBlock1, tempBlock2;
    //读取信息
    fs.open(fname, ALL);//blocklist.h中 #define ALL (ios::in|ios::out|ios::binary)
    fs.seekg(offset1, ios::beg);
    fs.read(reinterpret_cast<char *>(&tempBlock1), sizeof(tempBlock1));
    fs.clear();
    fs.seekg(offset2, ios::beg);
    fs.read(reinterpret_cast<char *>(&tempBlock2), sizeof(tempBlock2));
    fs.close();
    fs.clear();
    //合并相邻的两个块
    for (int i = 0; i < tempBlock2.Node_num; ++i)tempBlock1.node[i + tempBlock1.Node_num] = tempBlock2.node[i];
    tempBlock1.Node_num += tempBlock2.Node_num;
    tempBlock1.nxt = tempBlock2.nxt;
    //更新块状链表信息
    fs.open(fname, ios::out | ios::binary);
    fs.seekp(offset1, ios::beg);
    fs.write(reinterpret_cast<char *>(&tempBlock1), sizeof(tempBlock1));
    fs.close();
    fs.clear();
}


void blocklist::split(const int &offset, const int &cut_point) {
    Block tempBlock, newBlock = Block();
    //读入信息
    fs.open(fname, ALL);
    fs.clear();
    fs.seekg(offset, ios::beg);
    fs.read(reinterpret_cast<char *>(&tempBlock), sizeof(tempBlock));
    fs.close();
    fs.clear();
    //分裂offset地址存的块，第一块分HALF_SIZE个node 第二块分剩下的，即 Node_num-HALF_SIZE个node
    for (int i = 0; i < tempBlock.Node_num - HALF_SIZE; ++i) {
        newBlock.node[i] = tempBlock.node[i + HALF_SIZE];
        tempBlock.node[i + HALF_SIZE] = Node();
    }
    newBlock.Node_num = tempBlock.Node_num - HALF_SIZE;
    tempBlock.Node_num = HALF_SIZE;
    newBlock.nxt = tempBlock.nxt;//让分裂出来的这个块的nxt指向之前tempBlock的nxt的指向的块
    //转到写入模式
    fs.open(fname, ALL);
    fs.clear();///血的教训，seekp和seekg用同一个fstream进行时一定要重新打开文件，因为这个卡了一天
    fs.seekp(0, ios::end);//在最后存入新分裂的块
    fs.clear();//不知道没有这个会不会崩，当保险用，好像是如果读到eof会失控，所以就每个end后边都加了一个
    tempBlock.nxt = (int) fs.tellp();//让原来的tempBlock的nxt指向新分裂出来的块的 地址(offset)
    //更新块状链表信息
    fs.write(reinterpret_cast<char *>(&newBlock), sizeof(newBlock));
    fs.clear();
    fs.seekp(offset, ios::beg);
    fs.write(reinterpret_cast<char *>(&tempBlock), sizeof(tempBlock));
    fs.close();
    fs.clear();
}


void blocklist::add_Node(const Node &arg) {
    int offset = find_Block(arg.str);//找到这个node所应该在的块的偏移量
    Block tempBlock;
    //读取offset存的块信息
    fs.open(fname, ALL);
    fs.clear();
    fs.seekg(offset, ios::beg);
    fs.read(reinterpret_cast<char *>(&tempBlock), sizeof(tempBlock));
    fs.close();
    fs.clear();
    //找添加点的位置pos
    int pos = 0;
    for (pos = 0; pos < tempBlock.Node_num; ++pos)//找到按照字符串升序排序下这个点应在的位置pos
        if (arg <= tempBlock.node[pos])break;

    for (int i = tempBlock.Node_num; i > pos; --i)tempBlock.node[i] = tempBlock.node[i - 1];
    tempBlock.Node_num++;
    tempBlock.node[pos] = arg;
    //转为写入模式
    fs.open(fname, ios::out | ios::binary);
    fs.clear();
    fs.seekp(offset, ios::beg);
    fs.write(reinterpret_cast<char *>(&tempBlock), sizeof(tempBlock));
    fs.close();
    fs.clear();
    if (tempBlock.Node_num > BLOCK_SIZE - 10)split(offset, HALF_SIZE);
    //showDetail();
}


int blocklist::del_Node(const string &arg) {

    int offset = find_Block(arg);
    Block tempBlock;
    //读入tempBlock
    fs.open(fname, ALL);
    fs.clear();
    fs.seekg(offset, ios::beg);
    fs.read(reinterpret_cast<char *>(&tempBlock), sizeof(tempBlock));
    fs.close();
    fs.clear();
    //找到arg所在的位置pos
    int pos;
    for (pos = 0; pos < tempBlock.Node_num; ++pos) {
        string tempStr = tempBlock.node[pos].str;
        if (arg == tempStr)break;
        if (arg < tempStr) {
            pos = tempBlock.Node_num;
            break;
        }//如果此时的字典序已经大于arg了，那么就意味着找不到了，直接break
    }
    if (pos == tempBlock.Node_num)return -1;
    Node tempNode = tempBlock.node[pos];
    tempBlock.Node_num--;
    for (int i = pos; i < tempBlock.Node_num; ++i)tempBlock.node[i] = tempBlock.node[i + 1];
    tempBlock.node[tempBlock.Node_num] = Node();
    //转为写入模式
    fs.open(fname, ALL);
    fs.clear();
    fs.seekp(offset, ios::beg);
    fs.write(reinterpret_cast<char *>(&tempBlock), sizeof(tempBlock));
    fs.close();
    fs.clear();
    //转换为读取状态
    fs.open(fname, ALL);
    fs.clear();
    while (tempBlock.nxt != -1) {
        Block nextBlock;
        fs1.seekg(tempBlock.nxt, ios::beg);
        fs1.read(reinterpret_cast<char *>(&nextBlock), sizeof(nextBlock));
        if (tempBlock.Node_num + nextBlock.Node_num <= BLOCK_SIZE - 10) {
            merge(offset, tempBlock.nxt);
            fs.seekg(offset, ios::beg);
            fs.read(reinterpret_cast<char *>(&tempBlock), sizeof(tempBlock));
        } else break;//如果不能继续合并就退出去
    }
    fs.close();
    fs.clear();
    return tempNode.offset;//因为块状链表类是一个独立的功能类，所以访问不到存书的文件，无法设置Book的isdelete，所以返回这个offset让外边处理
}

int blocklist::del_Node(const Node &arg) {
    int offset = find_Block(arg.str);
    Block tempBlock;
    //读取tempBlock
    fs.open(fname, ALL);
    fs.clear();
    fs.seekg(offset, ios::beg);
    fs.read(reinterpret_cast<char *>(&tempBlock), sizeof(tempBlock));
    fs.close();
    fs.clear();
    int pos;
    for (pos = 0; pos < tempBlock.Node_num; ++pos) {
        if (arg == tempBlock.node[pos])break;
        if (arg < tempBlock.node[pos]) {
            pos = tempBlock.Node_num;
            break;
        }
    }
    if (pos == tempBlock.Node_num) return -1;
    Node tempNode = tempBlock.node[pos];
    tempBlock.Node_num--;
    for (int i = pos; i < tempBlock.Node_num; ++i)tempBlock.node[i] = tempBlock.node[i + 1];
    tempBlock.node[tempBlock.Node_num] = Node();
    //转为写入模式
    fs.seekp(offset, ios::beg);
    fs.write(reinterpret_cast<char *>(&tempBlock), sizeof(tempBlock));
    fs.close();
    fs.clear();
    //转为读取模式
    fs.open(fname, ALL);
    fs.clear();
    while (tempBlock.nxt != -1) {
        Block nextBlock;
        fs1.seekg(tempBlock.nxt, ios::beg);
        fs1.read(reinterpret_cast<char *>(&nextBlock), sizeof(nextBlock));
        if (tempBlock.Node_num + nextBlock.Node_num <= BLOCK_SIZE - 10) {
            merge(offset, tempBlock.nxt);
            fs.seekg(offset, ios::beg);
            fs.read(reinterpret_cast<char *>(&tempBlock), sizeof(tempBlock));
        } else break;//如果能合并相邻块就一直合并，直到不能合并退出来
    }
    fs.close();
    fs.clear();
    return tempNode.offset;//原因同上边的del函数
}

void blocklist::showDetail() {
    fs.open(fname, ALL);
    fs.clear();
    fs.seekg(0, ios::beg);
    Block tempBlock;
    fs.read(reinterpret_cast<char *>(&tempBlock), sizeof(tempBlock));
    cout << fname << " Inf:" << endl;
    while (1) {
        cout << "Node_num=" << tempBlock.Node_num << " next_offset=" << tempBlock.nxt << endl;
        for (int i = 0; i < tempBlock.Node_num; ++i)cout << tempBlock.node[i].str << " ";
        puts("");
        if (tempBlock.nxt == -1)break;
        fs.clear();
        fs.seekg(tempBlock.nxt, ios::beg);
        fs.read(reinterpret_cast<char *>(&tempBlock), sizeof(tempBlock));
    }
    fs.close();
    fs.clear();
}
