//用块状链表维护一个按照字典序内部排列的数据，使得内部访问可以实现sqrt(N)访问，实现sqrt(N)删除，增加一个特定的数据而不用遍历全部
#include<iostream>
#include<fstream>
#include<cstring>
#include<cctype>
#include<cmath>
#include<string>
#include<vector>

#define ALL (ios::in|ios::out|ios::binary)
#define BLOCK_SIZE 330 //sqrt(1e5)
#define HALF_SIZE 165  //160 is also OK
using namespace std;

class Node {
public:
    int offset;//offset存储其在Books.dat文件中的偏移量
    char str[64];//所有命令参数最大长度60，为了凑整取64bit

    Node();

    Node(const int &_offset, const string &arg);

    Node &operator=(const Node &arg);

    bool operator==(const Node &arg) const;

    bool operator<(const Node &arg) const;

    bool operator<=(const Node &arg) const;
};

class Block {
public:
    int nxt, Node_num;
    Node node[BLOCK_SIZE];

    Block();

    Block &operator=(const Block &arg);
};

class blocklist {
private:
    const string fname;
    fstream fs, fs1;

    //内部操作，给外边使用的，这样布局也是借鉴了你的学长，应该是一种比较专业的分布方式吧
    inline int next(const int &offset);//返回下一个块在文件中的首地址
    void split(const int &offset, const int &cut_point);//块体积过大，分裂调整保证复杂度
    void merge(const int &offset1, const int &offset2);//合并相邻的体积较小的块，保证复杂度
    int find_Block(const string &);//找这个新加点应放入的块，返回这个块在文件中的首地址
public:
    //外部接口在其他函数里使用的
    blocklist(const string &);

    ~blocklist();

    int find_Node(const string &);//在块状链表中找到 该字符串 的Node并返回其 偏移量（offset）
    void find_Node(const string &, vector<int> &);//这个是给后边show(name|keyword|author)准备的，这几个比较特殊(信息不唯一)，后边会说到
    void add_Node(const Node &);//增加点
    int del_Node(const string &);//删除点
    int del_Node(const Node &);//我实现del操作的时候习惯的把它设为int类型返回offset或-1验证是否删除有效，这个题没用，你可以写成void形式，我懒得改了。。。
    void showDetail();//调试用的函数，没用，不用管
};
