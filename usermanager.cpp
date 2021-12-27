#include "usermanager.h"

//User:
User::User() {
    privilege = -1;
    curBook = -1;
    memset(id, 0, sizeof(id));
    memset(passwd, 0, sizeof(passwd));
    memset(name, 0, sizeof(name));
}

User &User::operator=(const User &arg) {
    privilege = arg.privilege;
    curBook = arg.curBook;
    strcpy(id, arg.id);
    strcpy(passwd, arg.passwd);
    strcpy(name, arg.name);
    return *this;
}

//UserManager:
UserManager::UserManager() : id_cmd("id.dat"), fname("users.dat") {
    //初始vector中默认为游客账户，权限为0
    User tempUser;
    tempUser.privilege = 0;
    userStack.clear();
    userStack.push_back(tempUser);
    userNumber = 1;
    fs.open(fname, ALL);
    if (!fs.is_open()) {//第一次运行创建文件时要加入root账户
        fs.close();
        fs.clear();
        fs1.open(fname, ALL | ios::trunc);
        fs1.seekp(0, ios::beg);
        string tempStr;
        tempUser.privilege = 7;
        tempStr = "root", strcpy(tempUser.id, tempStr.c_str());
        tempStr = "root", strcpy(tempUser.name, tempStr.c_str());
        tempStr = "sjtu", strcpy(tempUser.passwd, tempStr.c_str());
        fs1.write(reinterpret_cast<char *>(&tempUser), sizeof(User));
        fs1.close();
        fs1.clear();
        id_cmd.add_Node(Node(0, "root"));
    } else fs.close(), fs.clear();
}

UserManager::~UserManager() {
    fs.close();
    fs1.close();
}

inline bool UserManager::userStringCheck(userStringTypeEnum userStringType, string arg) {
    if (arg.size() > 30)return false;
    //id/passwd:判断是否为连续无空格数字字母下划线，长度不超过30 username无限制
    if (userStringType == stringId || userStringType == stringPasswd) {
        for (int i = arg.size() - 1; i >= 0; --i) {
            if (!((arg[i] >= 'a' && arg[i] <= 'z') || (arg[i] >= 'A' && arg[i] <= 'Z')
                  || (arg[i] >= '0' && arg[i] <= '9') || arg[i] == '_'))
                return false;
        }
    }
    return true;
}

inline User UserManager::freadUser(const int &offset) {
    User tempUser;
    fs.open(fname, ALL);
    fs.seekg(offset, ios::beg);
    fs.read(reinterpret_cast<char *>(&tempUser), sizeof(User));
    fs.close();
    fs.clear();
    return tempUser;
}

bool UserManager::privilegeCheck(const int &privilegeNeed) {//判断当前账号的权限是否符合所需权限
    return userStack[userNumber - 1].privilege >= privilegeNeed;
}

int UserManager::userSelect() {//返回当前账号所选择的书籍的存贮offset
    return userStack[userNumber - 1].curBook;
}

void UserManager::changeSelect(const int &offset) {
    userStack[userNumber - 1].curBook = offset;
}

void UserManager::su(const string &id, const string &passwd) {
    if (id.empty() || !userStringCheck(stringId, id) || !userStringCheck(stringPasswd, passwd))
        return printf("Invalid\n"), void();//判断字符串合法
    //查找是否有该用户,若有记录下来后边用
    int offset = id_cmd.find_Node(id);
    if (offset == -1) return printf("Invalid\n"), void();
    string tempStr;
    User tempUser;
    tempUser = freadUser(offset);
    if (!passwd.empty()) {//输入了密码的情况
        tempStr = tempUser.passwd;
        if (passwd == tempStr) {//检查密码，加入UserStack
            tempUser.curBook = -1;
            ++userNumber;
            userStack.push_back(tempUser);
        }//如果目前没有该账户或密码错误
        else return printf("Invalid\n"), void();//这里不太清楚，高权限输入密码登录低权限如果错了我按照非法处理的
    } else {//检查是否为高权限登录到低权限
        if (privilegeCheck(tempUser.privilege + 1)) {// + 1 : 需要权限大于
            tempUser.curBook = -1;
            ++userNumber;
            userStack.push_back(tempUser);
        } else printf("Invalid\n");
    }
}

void UserManager::logout() {
    //无登录账户（只有0权限游客账户)操作非法
    //只要有登录账户，权限必大于等于1
    if (userNumber == 1)
        printf("Invalid\n");
    else {
        userStack.pop_back();
        --userNumber;
    }
}

void UserManager::reg(const string &id, const string &passwd, const string &name) {//注册customer  为权限为1账户
    useradd(id, passwd, 1, name, 1);
}

void UserManager::useradd(const string &id, const string &passwd, const int &privilege, const string &name,
                          const int &registerFlag) {
    if (registerFlag == 0 &&
        (userStack[userNumber - 1].privilege < 3 || userStack[userNumber - 1].privilege <= privilege)) {
        //register是为了区分 用户register指令(1) 和 管理者adduser指令(0) register的不需要权限，adduser的需要判断权限
        return printf("Invalid\n"), void();
    }
    if (id.empty() || passwd.empty() || name.empty()//任何一项为空则非法
        || !userStringCheck(stringId, id) || !userStringCheck(stringPasswd, passwd) ||
        !userStringCheck(stringName, name)
        || !(privilege == 1 || privilege == 3 || privilege == 7))
        return printf("Invalid\n"), void();

    if (id_cmd.find_Node(id) != -1) return printf("Invalid\n"), void();//不能重复添加账户
    //账户写入文件
    User tempUser;
    tempUser.privilege = privilege;
    tempUser.curBook = -1;
    strcpy(tempUser.id, id.c_str());
    strcpy(tempUser.passwd, passwd.c_str());
    strcpy(tempUser.name, name.c_str());

    fs.open(fname, ALL);
    fs.seekp(0, ios::end);
    Node tempNode((int) fs.tellp(), id);
    fs.write(reinterpret_cast<char *>(&tempUser), sizeof(User));
    fs.close();
    fs.clear();
    //同时更新维护id的块状链表
    id_cmd.add_Node(tempNode);
}

void UserManager::repwd(const string &id, const string &oldpwd, const string &newpwd) {
    //非游客号才有密码，这里用privilegeCheck函数判断其是否权限小于1
    if (!privilegeCheck(1))return printf("Invalid\n"), void();
    //检验字符串合法性
    if (newpwd.empty() || !userStringCheck(stringId, id) || !userStringCheck(stringPasswd, oldpwd) ||
        !userStringCheck(stringPasswd, newpwd))
        return printf("Invalid\n"), void();
    //如果不存在账户则非法
    int offset = id_cmd.find_Node(id);
    if (offset == -1)return printf("Invalid\n"), void();

    User tempUser;
    string temps;
    tempUser = freadUser(offset);
    temps = tempUser.passwd;
    if (!oldpwd.empty()) {//输入了oldpwd
        if (temps != oldpwd)return printf("Invalid\n"), void();
    } else if (!privilegeCheck(7)) {//没输入，如果不是7，就非法
        return printf("Invalid\n"), void();
    }
    //上边排除了输入了但不对的，和没输入但不是店主的,剩下就是合法的
    strcpy(tempUser.passwd, newpwd.c_str());
    fs.open(fname, ALL);
    fs.clear();
    fs.seekp(offset, ios::beg);
    fs.write(reinterpret_cast<char *>(&tempUser), sizeof(tempUser));
    fs.close();
    fs.clear();
}

void UserManager::del(const string &id) {//删除账号
    //只有店主才能删号
    if (!privilegeCheck(7))return printf("Invalid\n"), void();
    //检验字符串合法性
    if (id.empty() || !userStringCheck(stringId, id)) return printf("Invalid\n"), void();
    //不能删除已经登陆的账户
    string temps;
    for (int i = 0; i < userNumber; ++i) {
        temps = userStack[i].id;
        if (temps == id)return printf("Invalid\n"), void();
    }
    if (id_cmd.find_Node(id) == -1)return printf("Invalid\n"), void();
    id_cmd.del_Node(id);//调用维护id的块状链表的del_Node函数删除在链表上id
}
