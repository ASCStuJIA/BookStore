#include "bookstore.h"

//Book:
Book::Book() {
    memset(ISBN, 0, sizeof(ISBN));
    memset(name, 0, sizeof(name));
    memset(author, 0, sizeof(author));
    memset(keyword, 0, sizeof(keyword));
    price = -1;
    quantity = 0;
}

Book &Book::operator=(const Book &arg) {
    strcpy(ISBN, arg.ISBN);
    strcpy(name, arg.name);
    strcpy(author, arg.author);
    strcpy(keyword, arg.keyword);
    price = arg.price;
    quantity = arg.quantity;
    return *this;
}

bool Book::operator<(const Book &arg) const {
    string s1 = ISBN, s2 = arg.ISBN;
    return s1 < s2;
}

//************FileManager***************
//FinanceFile:
FileManager::FileManager() :
        fnameBook("books.dat"), fnameFinance("finance.dat"), fnameLog("log.dat") {
    //检查book.dat
    fs.open(fnameBook, ALL);
    if (!fs.is_open()) {
        fs.close();
        fs.clear();
        fs.open(fnameBook, ALL | ios::trunc);
        //books.dat开头写入图书总数(int)，便于后边访问
        int tempInt = 0;
        fs.seekp(0, ios::beg);
        fs.write(reinterpret_cast<char *>(&tempInt), sizeof(int));
    }
    fs.close();
    fs.clear();

    //检查finance.dat
    fs.open(fnameFinance, ALL);
    if (!fs.is_open()) {
        fs.close();
        fs.clear();
        fs.open(fnameFinance, ALL | ios::trunc);
        //finance.dat开头写入交易次数(int),总收入(double),总支出(double)，便于后边统计finance
        int tempInt = 0;
        double tempDouble = 0;
        fs.seekp(0, ios::beg);
        fs.write(reinterpret_cast<char *>(&tempInt), sizeof(int));
        fs.write(reinterpret_cast<char *>(&tempDouble), sizeof(double));
        fs.write(reinterpret_cast<char *>(&tempDouble), sizeof(double));
    }
    fs.close();
    fs.clear();

    //检查log.dat
    fs.open(fnameLog, ALL);
    if (!fs.is_open()) {
        fs.close();
        fs.clear();
        fs.open(fnameLog, ALL | ios::trunc);
    }
    fs.close();
    fs.clear();
}

inline void FileManager::financeInit(int &tradeNum, double &income, double &expense) {
    //获取finance.dat开头数据tradeNumber,totIncome,totExpense
    fs.open(fnameFinance, ALL);
    fs.seekg(0, ios::beg);
    fs.read(reinterpret_cast<char *>(&tradeNum), sizeof(int));
    fs.read(reinterpret_cast<char *>(&income), sizeof(double));
    fs.read(reinterpret_cast<char *>(&expense), sizeof(double));
    fs.close();
    fs.clear();
}

inline void FileManager::financeInitWrite(int &num, double &income, double &expense) {
    //写入finance.dat开头数据tradeNumber,totIncome,totExpense
    fs.open(fnameFinance, ALL);
    fs.clear();
    fs.seekp(0, ios::beg);
    fs.write(reinterpret_cast<char *>(&num), sizeof(int));
    fs.write(reinterpret_cast<char *>(&income), sizeof(double));
    fs.write(reinterpret_cast<char *>(&expense), sizeof(double));
    fs.close();
    fs.clear();
}

inline void FileManager::freadFinance(const int &time) {
    int offset, Limit;
    double Price, Income = 0, Expense = 0;
    bool Sgn;
    int Time = time;
    const int delta = sizeof(double) + sizeof(bool);
    fs.open(fnameFinance, ALL);
    fs.clear();
    fs.seekg(0, ios::end);
    fs.clear();
    offset = (int) fs.tellg() - delta;
    Limit = sizeof(int) + sizeof(double) + sizeof(double);
    while (offset >= Limit && Time--) {//注意不要让offset读到Limit内的数
        fs.seekg(offset, ios::beg);
        fs.read(reinterpret_cast<char *>(&Price), sizeof(double));
        fs.read(reinterpret_cast<char *>(&Sgn), sizeof(bool));
        //依据判断Sgn判断收入或是支出
        if (Sgn) Expense += Price;
        else Income += Price;
        offset -= delta;
    }
    printf("+ %.2lf - %.2lf\n", Income, Expense);
    fs.close();
    fs.clear();
}

inline void FileManager::financeWrite(double &price, bool &sgn) {
    //在finance.dat末写入一笔交易
    fs.open(fnameFinance, ALL);
    fs.clear();
    fs.seekp(0, ios::end);
    fs.clear();
    fs.write(reinterpret_cast<char *>(&price), sizeof(double));
    fs.write(reinterpret_cast<char *>(&sgn), sizeof(bool));
    fs.close();
    fs.clear();
}

//BookFile:
inline void FileManager::bookInit(int &bookNum) {
    //获取books.dat开头数据bookNumber
    fs.open(fnameBook, ALL);
    fs.clear();
    fs.seekg(0, ios::beg);
    fs.read(reinterpret_cast<char *>(&bookNum), sizeof(int));
    fs.close();
    fs.clear();
}

inline void FileManager::bookInitWrite(int &bookNum) {
    //写入books.dat开头数据bookNumber
    fs.open(fnameBook, ALL);
    fs.clear();
    fs.seekp(0, ios::beg);
    fs.write(reinterpret_cast<char *>(&bookNum), sizeof(bookNum));
    fs.close();
    fs.clear();
}

inline void FileManager::freadBook(int offset, Book &arg) {
    fs.open(fnameBook, ALL);
    fs.clear();
    fs.seekg(offset, ios::beg);
    fs.read(reinterpret_cast<char *>(&arg), sizeof(arg));
    fs.close();
    fs.clear();
}

inline void FileManager::freadBook(vector<Book> &array) {//读取全部书籍

    fs.open(fnameBook, ALL);
    fs.clear();
    fs.seekg(0, ios::beg);
    int bookNum;
    Book tempBook;
    fs.read(reinterpret_cast<char *>(&bookNum), sizeof(int));
    for (int i = 0; i < bookNum; ++i) {
        fs.read(reinterpret_cast<char *>(&tempBook), sizeof(Book));
        array.push_back(tempBook);
    }
    fs.close();
    fs.clear();
}

inline int FileManager::fwriteBook(Book &arg) {//在文件中写入新书，并返回其offset
    fs.open(fnameBook, ALL);
    fs.seekp(0, ios::end);
    fs.clear();
    int offset = fs.tellp();
    fs.write(reinterpret_cast<char *>(&arg), sizeof(Book));
    fs.close();
    fs.clear();
    return offset;
}

inline void FileManager::fwriteBook(int offset, Book &arg) {//在offset处书的信息更新为arg
    fs.open(fnameBook, ALL);
    fs.clear();
    fs.seekp(offset, ios::beg);
    fs.write(reinterpret_cast<char *>(&arg), sizeof(Book));
    fs.close();
    fs.clear();
}

//Bookstore:
Bookstore::Bookstore() : isbn_cmd("isbn.dat"), name_cmd("name.dat"), author_cmd("author.dat"),
                         keyword_cmd("keyword.dat") {
    bookNumber = 0, tradeNumber = 0;
    totIncome = 0, totExpense = 0;
    File_cmd.financeInit(tradeNumber, totIncome, totExpense);
    File_cmd.bookInit(bookNumber);
}

//辅助函数
inline void Bookstore::splitString(string &arg, string &ret, int keywordFlag) {//将arg拆分出第一部分ret
    int p1 = 0, p2;
    ret.clear();
    if (keywordFlag == 0) {//以空格和tab为分隔符
        while ((arg[p1] == ' ' || arg[p1] == '\t') && p1 < arg.length())++p1;
        if (p1 < arg.length()) {
            p2 = p1;
            while ((arg[p2] != ' ' && arg[p2] != '\t') && p2 < arg.length())++p2;
            ret = arg.substr(p1, p2 - p1);
            arg = arg.substr(p2, arg.length() - p2);
        } else arg.clear();//整个arg都为空白字符，这里要清理一下的原因是，arg有可能有空白格，清除一下就可以把arg置为空
    } else {//拆分keyword字符串的情况,以'|'为分隔符；进来前保证arg没有首尾空格
        while ((arg[p1] != '|') && p1 < arg.length())++p1;
        if (p1 < arg.length()) {
            ret = arg.substr(0, p1);
            arg = arg.substr(p1 + 1, arg.length() - p1 - 1);
        } else ret = arg, arg.clear();
    }
}

inline bool Bookstore::bookStringCheck(bookStringTypeEnum bookStringType, const string &arg) {
    //首先为了后边方便，这里认为空的arg为合法
    if (!arg.empty()) {
        if (bookStringType == stringISBN) {
            if (arg.length() > 20)return false;
            for (int i = 0; i < arg.length(); ++i)//ISBN码中只含有数字和‘-‘
                if (!(isdigit(arg[i]) || (arg[i] == '-')))return false;
        } else if (bookStringType == stringBookName || bookStringType == stringAuthor) {
            if (arg.length() > 60)return false;
            if (arg[0] != '\"' || arg[arg.length() - 1] != '\"')return false;
            for (int i = 1; i < arg.length() - 1; ++i)//中间不能存在双引号
                if (arg[i] == '\"')return false;
        } else {//bookStringType == stringKeyword
            if (arg.length() > 60)return false;
            if (arg[0] != '\"' || arg[arg.length() - 1] != '\"')return false;//规定必须内容用双引号标注，否则非法
            for (int i = 1; i < arg.length() - 1; ++i)//中间不能存在双引号
                if (arg[i] == '\"')return false;
        }
    }
    return true;
}

inline void Bookstore::printBook(const Book &arg) {
    printf("%s\t%s\t%s\t%s\t%.2lf\t%d\n", arg.ISBN, arg.name, arg.author, arg.keyword,
           arg.price == -1 ? 0.00 : arg.price, arg.quantity);
}

void Bookstore::addFinance(double price, bool sgn) {//sgn:true支出,false收入
    ++tradeNumber;
    if (sgn)totExpense += price;
    else totIncome += price;
    File_cmd.financeWrite(price, sgn);
    File_cmd.financeInitWrite(tradeNumber, totIncome, totExpense);
}

int Bookstore::find(const string &ISBN) {
    return isbn_cmd.find_Node(ISBN);
}

void Bookstore::find(findTypeEnum findType, const string &key, vector<int> &vec) {//重载了find，原因见bookstore头文件
    if (findType == findName) name_cmd.find_Node(key, vec);
    else if (findType == findAuthor) author_cmd.find_Node(key, vec);
    else if (findType == findKeyword) keyword_cmd.find_Node(key, vec);
    else return printf("Invalid\n"), void();//找的信息是维护之外的则非法
}

//功能函数

void Bookstore::showLog(logTypeEnum logType) {
    //这个函数表现形式自拟，我就不写了
}

void Bookstore::showFinance(int time) {//这里time在前边声明时默认置为了-1，所以不传值时time=-1
    if (!user_cmd.privilegeCheck(7)) return printf("Invalid\n"), void();//判断是不是店长，店长才有权限check finance
    if (time == -1)//即为 show 指令
        printf("+ %.2lf - %.2lf\n", totIncome, totExpense);
    else {//输出最近time次收入支出
        if (time == 0)//由于freadFinance函数无法处理0的情况，会输出 +0，-0 正常来说是不应该输出的
            printf("\n");
        else if (time <= tradeNumber)
            File_cmd.freadFinance(time);
        else printf("Invalid\n");
    }
}

void Bookstore::import(const int &quantity, const double &price) {//进货
    //未选中书时，即select=-1时不能进行import操作,还要注意一个坑，price是总价不是单价
    if (user_cmd.userSelect() == -1 || !user_cmd.privilegeCheck(3)) return printf("Invalid\n"), void();
    addFinance(price, 1);//第二维sgn=1是代表支出，反之则为收入
    //更新quantity
    Book tempBook;
    File_cmd.freadBook(user_cmd.userSelect(), tempBook);
    tempBook.quantity += quantity;
    File_cmd.fwriteBook(user_cmd.userSelect(), tempBook);
}


void Bookstore::buy(const string &ISBN, const int &quantity) {
    //有账户才能买书，所以要check(1)
    if (ISBN.empty() || quantity < 0 || !user_cmd.privilegeCheck(1)) return printf("Invalid\n"), void();
    int offset = find(ISBN);
    if (offset == -1)return printf("Invalid\n"), void();
    //修改Book.quantity
    Book tempBook;
    File_cmd.freadBook(offset, tempBook);
    if (tempBook.quantity < quantity) //库存不够
        return printf("Invalid\n"), void();
    if (tempBook.price == -1) {//这种情况是应对select之后新创的没有指明书价的情况,按照初始化此时price=-1，参考你学长的处理情况，是默认书价为0
        addFinance(0, 0);//第二维0代表收入
        tempBook.quantity -= quantity;
        File_cmd.fwriteBook(offset, tempBook);
        return printf("0.00\n"), void();//按照要求应该输出保留两位小数的花费
    }
    //正常情况
    addFinance(quantity * tempBook.price, 0);
    tempBook.quantity -= quantity;
    File_cmd.fwriteBook(offset, tempBook);
    printf("%.2lf\n", tempBook.price * quantity);
}


void Bookstore::select(const string &ISBN) {
    //注意custom(privilege=1)没有select权限
    if (!user_cmd.privilegeCheck(3) || ISBN.empty() || !bookStringCheck(stringISBN, ISBN))
        return printf("Invalid\n"), void();
    int offset = find(ISBN);
    if (offset == -1) {//没有此书则新建空白书
        Book tempBook;
        strcpy(tempBook.ISBN, ISBN.c_str());
        //select创建新书时，除了更新isdn文件和book文件之外，也要记得更新当前用户的SelectBook
        //这里因为创建的是空白书，没有name之类的信息，所以就不用更新，后边如果modify更新了信息，那么到时候在modify里再进行操作
        offset = File_cmd.fwriteBook(tempBook);
        File_cmd.bookInitWrite(++bookNumber);//更新图书数量
        user_cmd.changeSelect(offset);
        isbn_cmd.add_Node(Node(offset, ISBN));
    } else user_cmd.changeSelect(offset);//有则选中
}

void Bookstore::modify(const int &offset, const string &ISBN, string name, string author, string keyword,
                       const double &price) {
    //offset = user_cmd.userSelect() 已在operation中保证不为 -1
    //price在operation中已保证非负，-1表无值
    //operation中已保证正确的输入格式
    //isbn_cmd.showDetail();
    if (!user_cmd.privilegeCheck(3)//写在一行太长了，这里就按照判断类别调整了一下
        || !bookStringCheck(stringISBN, ISBN) || !bookStringCheck(stringBookName, name) ||
        !bookStringCheck(stringAuthor, author) || !bookStringCheck(stringAuthor, keyword) ||
        (!ISBN.empty() && find(ISBN) != -1)) //如果改为已存在的ISBN则非法
        return printf("Invalid\n"), void();
    Book tempBook;//读取原Book内容
    File_cmd.freadBook(offset, tempBook);

    if (!keyword.empty()) {//有return需优先判断
        //去除首末双引号
        keyword = keyword.substr(1, keyword.size() - 2);
        string tempStr1, tempStr2;
        //防止重复 keyword
        tempStr2 = keyword;
        splitString(tempStr2, tempStr1, 1);//让tempStr1取到tempStr2的第一段有效串,并且在tempStr2中删除tempStr1取到的部分
        vector<string> tempVec;
        while (!tempStr1.empty()) {
            tempVec.push_back(tempStr1);
            splitString(tempStr2, tempStr1, 1);
        }
        for (int i = 0; i < tempVec.size(); ++i)
            for (int j = i + 1; j < tempVec.size(); ++j)
                if (tempVec[i] == tempVec[j])return printf("Invalid\n"), void();
        //先把旧keyword内容全删掉
        tempStr2 = tempBook.keyword;
        splitString(tempStr2, tempStr1, 1);
        while (!tempStr1.empty()) {
            keyword_cmd.del_Node(Node(offset, tempStr1));
            splitString(tempStr2, tempStr1, 1);
        }
        //再加入新Keyword内容
        for (int i = 0; i < tempVec.size(); ++i)
            keyword_cmd.add_Node(Node(offset, tempVec[i]));
        strcpy(tempBook.keyword, keyword.c_str());
    }
    if (!ISBN.empty()) {
        isbn_cmd.del_Node(tempBook.ISBN);
        isbn_cmd.add_Node(Node(offset, ISBN));
        strcpy(tempBook.ISBN, ISBN.c_str());
    }
    if (!name.empty()) {
        name = name.substr(1, name.size() - 2);//去除首末双引号
        name_cmd.del_Node(Node(offset, tempBook.name));
        name_cmd.add_Node(Node(offset, name));
        strcpy(tempBook.name, name.c_str());
    }
    if (!author.empty()) {
        author = author.substr(1, author.size() - 2);//去除首末双引号
        author_cmd.del_Node(Node(offset, tempBook.author));
        author_cmd.add_Node(Node(offset, author));
        strcpy(tempBook.author, author.c_str());
    }
    if (price != -1) tempBook.price = price;
    //更新书的信息
    File_cmd.fwriteBook(offset, tempBook);
}

void Bookstore::operation(string cmd) {
    //本函数保证调用其他函数时的参数为非空无空白字符字符串或正常整形变量
    string arg0, arg1, arg2, arg3;
    splitString(cmd, arg0);//提取出命令类型arg0
    //账户操作
    if (arg0 == "su") {
        splitString(cmd, arg1);
        splitString(cmd, arg2);
        splitString(cmd, arg0);//如果还有其余多余输入，则非法
        if (arg0.empty())user_cmd.su(arg1, arg2);
        else return printf("Invalid\n"), void();
    } else if (arg0 == "logout") {
        splitString(cmd, arg0);
        if (arg0.empty())user_cmd.logout();
        else return printf("Invalid\n"), void();
    } else if (arg0 == "useradd") {
        string arg4;
        splitString(cmd, arg1);
        splitString(cmd, arg2);
        splitString(cmd, arg3);
        splitString(cmd, arg4);
        splitString(cmd, arg0);
        if (arg0.empty()) {
            int tempNum = 0;
            for (int i = 0; i < arg3.size(); ++i)tempNum = tempNum * 10 + (arg3[i] - '0');//字符串转数字
            user_cmd.useradd(arg1, arg2, tempNum, arg4);
        } else return printf("Invalid\n"), void();
    } else if (arg0 == "register") {
        splitString(cmd, arg1);
        splitString(cmd, arg2);
        splitString(cmd, arg3);
        splitString(cmd, arg0);
        if (arg0.empty())user_cmd.reg(arg1, arg2, arg3);
        else return printf("Invalid\n"), void();
    } else if (arg0 == "delete") {
        splitString(cmd, arg1);
        splitString(cmd, arg0);
        if (arg0.empty())user_cmd.del(arg1);
        else return printf("Invalid\n"), void();
    } else if (arg0 == "passwd") {
        splitString(cmd, arg1);
        splitString(cmd, arg2);
        splitString(cmd, arg3);
        splitString(cmd, arg0);
        if (arg0.empty())user_cmd.repwd(arg1, arg2, arg3);
        else return printf("Invalid\n"), void();
    }

        //书籍操作
    else if (arg0 == "select") {
        splitString(cmd, arg1);
        splitString(cmd, arg0);
        if (arg0.empty())select(arg1);
        else return printf("Invalid\n"), void();
    } else if (arg0 == "modify") {
        if (user_cmd.userSelect() != -1 && user_cmd.privilegeCheck(3)) {
            string modifyISBN, modifyName, modifyAuthor, modifyKeyword;
            modifyISBN.clear();
            modifyName.clear();
            modifyAuthor.clear();
            modifyKeyword.clear();
            double modifyPrice = -1;
            splitString(cmd, arg1);
            if (arg1.empty())return printf("Invalid\n"), void();//modify指令后没有任何参数则为非法指令
            while (!arg1.empty()) {
                if (arg1.substr(0, 6) == "-ISBN=") {
                    if (modifyISBN.empty()) {//参数只能出现一次，重复出现则非法
                        modifyISBN = arg1.substr(6, arg1.length() - 6);
                        if (modifyISBN.empty())return printf("Invalid\n"), void();//如果该参数为空则非法
                    } else return printf("Invalid\n"), void();
                } else if (arg1.substr(0, 6) == "-name=") {
                    if (modifyName.empty()) {
                        modifyName = arg1.substr(6, arg1.length() - 6);
                        if (modifyName.empty())return printf("Invalid\n"), void();
                    } else return printf("Invalid\n"), void();
                } else if (arg1.substr(0, 8) == "-author=") {
                    if (modifyAuthor.empty()) {
                        modifyAuthor = arg1.substr(8, arg1.length() - 8);
                        if (modifyAuthor.empty())return printf("Invalid\n"), void();
                    } else return printf("Invalid\n"), void();
                } else if (arg1.substr(0, 9) == "-keyword=") {
                    if (modifyKeyword.empty()) {
                        modifyKeyword = arg1.substr(9, arg1.length() - 9);
                        if (modifyKeyword.empty())return printf("Invalid\n"), void();
                    } else return printf("Invalid\n"), void();
                } else if (arg1.substr(0, 7) == "-price=") {
                    if (modifyPrice == -1) {
                        arg1 = arg1.substr(7, arg1.length() - 7);
                        modifyPrice = 0;
                        for (int i = 0, Sgn = 0; i < arg1.size(); ++i) {
                            if (arg1[i] == '.') {
                                Sgn = 1;
                                continue;
                            }
                            else if (!isdigit(arg1[i]))return printf("Invalid\n"), void();
                            if (!Sgn)//小数点前的
                                modifyPrice = modifyPrice * 10 + (arg1[i] - '0');
                            else //小数部分
                                modifyPrice += 1.0f * (arg1[i] - '0') * pow(1e-1, Sgn++);//Sgn是为了记小数点后几位
                        }
                    } else return printf("Invalid\n"), void();
                } else return printf("Invalid\n"), void();
                splitString(cmd, arg1);
            }
            modify(user_cmd.userSelect(), modifyISBN, modifyName, modifyAuthor, modifyKeyword, modifyPrice);
        } else return printf("Invalid\n"), void();
    } else if (arg0 == "import") {
        splitString(cmd, arg1);
        splitString(cmd, arg2);
        splitString(cmd, arg0);
        if (!arg0.empty()) return printf("Invalid\n"), void();
        int tempInt = 0;
        double tempDouble = 0;
        for (int i = 0; i < arg1.size(); ++i) {//arg1转整数
            if (!isdigit(arg1[i]))return printf("Invalid\n"), void();
            tempInt = tempInt * 10 + (arg1[i] - '0');
        }
        for (int i = 0, Sgn = 0; i < arg2.size(); ++i) {//arg2转浮点数
            if (arg2[i] == '.') {
                Sgn = 1;
                continue;
            }
            else if (!isdigit(arg2[i]))return printf("Invalid\n"), void();
            if (!Sgn)tempDouble = tempDouble * 10.0 + 1.0f * (arg2[i] - '0');
            else tempDouble += 1.0f * (arg2[i] - '0') * pow(1e-1, Sgn++);
        }
        import(tempInt, tempDouble);
    } else if (arg0 == "show") {
        if (!cmd.empty()) {//带参数show指令
            splitString(cmd, arg1);
            //这里注意区分分支，show->finance 也可以 show->book
            //show finance
            if (arg1.substr(0, 7) == "finance") {
                if (!user_cmd.privilegeCheck(7))return printf("Invalid\n"), void();
                splitString(cmd, arg2);
                if (!arg2.empty()) {//show finance [-time] 指令
                    splitString(cmd, arg0);
                    if (!arg0.empty())return printf("Invalid\n"), void();
                    else {
                        int tempInt = 0;
                        for (int i = 0; i < arg2.size(); ++i) {//转整数
                            if (!isdigit(arg2[i]))return printf("Invalid\n"), void();
                            tempInt = tempInt * 10 + (arg2[i] - '0');
                        }
                        showFinance(tempInt);
                    }
                } else //show finance 考虑全部
                    showFinance();
            }
                // Book单关键词show
            else if (user_cmd.privilegeCheck(1)) {
                if (arg1.substr(0, 6) == "-ISBN=") {
                    arg1 = arg1.substr(6, arg1.length() - 6);
                    if (!arg1.empty() && bookStringCheck(stringISBN, arg1)) {
                        int offset = find(arg1);
                        if (offset == -1)printf("\n");//如果没有符合条件的书，输出一个空行
                        else {
                            Book tempBook;
                            File_cmd.freadBook(offset, tempBook);
                            printBook(tempBook);
                        }
                    } else return printf("Invalid\n"), void();
                } else {
                    vector<int> tempVec;//用来存所有符合的书的offset
                    tempVec.clear();
                    if (arg1.substr(0, 6) == "-name=") {
                        arg1 = arg1.substr(6, arg1.length() - 6);
                        if (!arg1.empty() && bookStringCheck(stringBookName, arg1))
                            find(findName, arg1.substr(1, arg1.size() - 2), tempVec);//arg1.substr()去双引号
                        else return printf("Invalid\n"), void();
                    } else if (arg1.substr(0, 8) == "-author=") {
                        arg1 = arg1.substr(8, arg1.length() - 8);
                        if (!arg1.empty() && bookStringCheck(stringAuthor, arg1))
                            find(findAuthor, arg1.substr(1, arg1.size() - 2), tempVec);
                        else return printf("Invalid\n"), void();
                    } else if (arg1.substr(0, 9) == "-keyword=") {
                        arg1 = arg1.substr(9, arg1.length() - 9);
                        if (!arg1.empty() && bookStringCheck(stringKeyword, arg1))
                            find(findKeyword, arg1.substr(1, arg1.size() - 2), tempVec);
                        else return printf("Invalid\n"), void();
                    }//show命令参数不存在
                    else return printf("Invalid\n"), void();
                    if (!tempVec.empty()) {
                        //把tempVec中所有书籍按照ISBN号排序并输出
                        vector<Book> tempBookArray;
                        Book tempBook;
                        tempBookArray.clear();
                        for (int i = 0; i < tempVec.size(); ++i) {
                            File_cmd.freadBook(tempVec[i], tempBook);
                            tempBookArray.push_back(tempBook);
                        }
                        sort(tempBookArray.begin(), tempBookArray.end());
                        for (int i = 0; i < tempBookArray.size(); ++i)printBook(tempBookArray[i]);
                    }//空的，没有符合的则输出空行
                    else printf("\n");
                }
            } else return printf("Invalid\n"), void();//show命令带非finance的参数且权限小于1
        } else {//show指令，按照ISBN顺序输出全部书籍
            if (!user_cmd.privilegeCheck(1)) return printf("Invalid\n"), void();
            vector<Book> tempBookArray;
            tempBookArray.clear();
            File_cmd.freadBook(tempBookArray);
            sort(tempBookArray.begin(), tempBookArray.end());
            for (int i = 0; i < tempBookArray.size(); ++i)printBook(tempBookArray[i]);
        }
    } else if (arg0 == "buy") {
        splitString(cmd, arg1);
        splitString(cmd, arg2);
        splitString(cmd, arg0);
        if (!arg0.empty())return printf("Invalid\n"), void();
        int tempInt = 0;
        for (int i = 0; i < arg2.size(); ++i) {
            if (!isdigit(arg2[i]))return printf("Invalid\n"), void();
            tempInt = tempInt * 10 + arg2[i] - '0';
        }
        buy(arg1, tempInt);
    } else if (arg0 == "report") {
        splitString(cmd, arg1);
        splitString(cmd, arg0);
        if (!arg0.empty()) return printf("Invalid\n"), void();
        if (arg1 == "finance")showLog(reportFinance);
        else if (arg1 == "employee")showLog(reportEmployee);
        else if (arg1 == "myself")showLog(reportMyself);
        else return printf("Invalid\n"), void();//非法指令
    } else if (arg0 == "log") {//这个函数具体展现形式我不清楚就没有管，如果你自己做这个遇到什么问题的话，你可以告诉我一下你们的要求，我去学习一下//我啥也不会实锤了
        splitString(cmd, arg0);
        if (!arg0.empty()) return printf("Invalid\n"), void();
        showLog(reportLog);
    } else if (!arg0.empty()) return printf("Invalid\n"), void();
    //注意空指令不要输出Invalid,不管就行
}

void Bookstore::showDetail() {
    isbn_cmd.showDetail();
    name_cmd.showDetail();
    author_cmd.showDetail();
    keyword_cmd.showDetail();
}
