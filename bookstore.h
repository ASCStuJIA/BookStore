#include <cstdio>
#include <iostream>
#include <algorithm>
#include <string>
#include <cstring>
#include "usermanager.h"

using namespace std;

class Book {
public:
    char ISBN[24];
    char name[64];
    char author[64];
    char keyword[64];
    double price;
    int quantity;

    Book();

    Book &operator=(const Book &arg);

    bool operator<(const Book &arg) const;
};


class FileManager {
private:
    const string fnameBook, fnameFinance, fnameLog;//分别存放书名文件，收支统计文件，日志文件
    fstream fs;
public:
    FileManager();

    //对财务文件操作函数：
    inline void financeInit(int &tradeNum, double &income, double &expense);//用来取一开始在 构造函数 里留存空间里存的数(交易次数,收入,支出)
    inline void financeInitWrite(int &num, double &income, double &expense);//本可以用const的，但是强制类型转换好像不行
    inline void freadFinance(const int &time);//读取最后time次的交易信息
    inline void financeWrite(double &price, bool &sgn);//sgn代表是收/支
    //对书籍文件操作函数:
    inline void bookInit(int &bookNum);//用来取一开始在 构造函数 里留存空间里存的数(书的数量)
    inline void bookInitWrite(int &bookNum);//修改/更新 bookInit函数维护的信息
    inline void freadBook(int offset, Book &arg);//读取offset位置的图书信息
    inline void freadBook(vector<Book> &vec);//读取所有图书信息
    inline int fwriteBook(Book &arg);//修改/写入图书信息
    inline void fwriteBook(int offset, Book &arg);
};


class Bookstore {
private:
    enum logTypeEnum {
        reportMyself, reportEmployee, reportLog, reportFinance
    };
    enum findTypeEnum {
        findName, findAuthor, findKeyword
    };
    enum bookStringTypeEnum {
        stringISBN, stringBookName, stringAuthor, stringKeyword
    };
    //变量
    blocklist isbn_cmd;
    blocklist name_cmd;
    blocklist author_cmd;
    blocklist keyword_cmd;//上边四个是对每个参数都维护一个块状数组，便于在sqrt(n)复杂度下对每个参数进行删，查，改，插入操作
    UserManager user_cmd;//用户信息类
    FileManager File_cmd;//控制文件修改和读写的类
    int tradeNumber;//总交易次数
    double totIncome, totExpense;//总收入、总支出
    int bookNumber;//图书总数
//辅助函数
    inline void splitString(string &arg, string &ret, int keywordFlag = 0);

    inline bool bookStringCheck(bookStringTypeEnum bookStringType, const string &arg);

    inline void printBook(const Book &arg);

    void addFinance(double price, bool sgn);

    int find(const string &ISBN);

    void find(findTypeEnum findType, const string &key,
              vector<int> &vec);//重载了find函数，和 ISBN区分开的原因是，题目保证ISBN是唯一的，但是其他信息没保证，所以这里要用vector把信息为key的书用vector全取出来，然后再按照ISBN排序后输出
//功能函数
    void showLog(logTypeEnum logType);//这个函数我不太明白当面展示的形式是什么，所以就没做，如果你知道的话，或者需要的话，你告诉我一下具体的形式，我去学习一下，也可以帮着你坐坐
    void showFinance(int time = -1);//默认设为-1位了在不输入time时以此判定输出所有书
    void import(const int &quantity, const double &price);

    void buy(const string &ISBN, const int &quantity);

    void select(const string &ISBN);

    void modify(const int &offset, const string &ISBN, string name, string author, string keyword, const double &price);

public:
    Bookstore();

    void operation(string cmd);

    void showDetail();//调试用的
};
