//在写这个的时候借鉴了paperL学长的写法
#include <iostream>
#include<fstream>
#include "bookstore.h"

using namespace std;

int main(int argc, const char *argv[]) {
    Bookstore work; //= Bookstore();
    //初始化会 error：Call to implicitly-deleted copy constructor of 'Bookstore'
    string Command_line;
    while (getline(cin, Command_line)) {
        //work.showDetail();//调试用的，输出所有块状链表上的信息
        if (Command_line == "exit" || Command_line == "quit")break;
        work.operation(Command_line);
    }
    return 0;
}
