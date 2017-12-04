/*
 * 简易ls程序
 *
 * Author: 阎覃 <ethanyt@qq.com>
 * Blog: https://ethan-yt.github.io/
 * Github: https://github.com/Ethan-yt/JNU-Linux-exp
 *
 * 本程序实现了简易的ls命令，可以用的选项有-R、-S、-a、-i、-l、
 * -r、-t、-1，功能分别为递归查看子目录、按大小排序、显示隐藏文
 * 件、显示inode、长格式、倒序显示、按修改时间排序、单列显示。
 *
 * 用法： ./ls [-RSailrt1] [file ...]
 *
 */

#include <iostream>
#include <dirent.h>
#include <sys/stat.h>
#include <sstream>
#include <iomanip>
#include <pwd.h>
#include <grp.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <vector>
#include <stack>

#define LONG_FORMAT     1<<0        // -l
#define INODE           1<<1        // -i
#define SORT_BY_TIME    1<<2        // -t
#define ALL             1<<3        // -a
#define ONE_PER_LINE    1<<4        // -1
#define SORT_BY_SIZE    1<<5        // -S
#define REVERSE         1<<6        // -r
#define RECURSIVE       1<<7        // -R

using namespace std;

struct MyDir {
    DIR *dir;
    string name;
};

stack<MyDir> dirs;      // 被查询的所有目录

const char *homeDir;    // home目录 以便替换~
int options = 0;        // 配置项 以二进制形式存储
int cols;               // 控制台的宽度 便于格式化输出

struct MyFile {
    struct stat s{};
    string strInode;
    string strLinksNumber;
    string strOwnerName;
    string strGroupName;
    string strBytes;
    string strName;

    MyFile(const string &dirName, const string &fileName) {
        string fullPath = dirName + "/" + fileName;
        lstat(fullPath.c_str(), &s);

        if (options & INODE)                        // 节点
            strInode = to_string(s.st_ino);

        if (options & LONG_FORMAT) {                // 长格式输出
            strLinksNumber = to_string(s.st_nlink); // 链接数
            passwd *pw = getpwuid(s.st_uid);        // 用户名
            strOwnerName = pw->pw_name;
            group *gr = getgrgid(s.st_gid);         // 用户组
            strGroupName = gr->gr_name;
            strBytes = to_string(s.st_size);        // 字节数
        }

        strName = fileName;                         // 文件名
    }

    friend bool operator<(const MyFile &lhs, const MyFile &rhs) {
        if (options & REVERSE) {
            if (options & SORT_BY_SIZE && lhs.s.st_size != rhs.s.st_size)
                return lhs.s.st_size < rhs.s.st_size;
            if (options & SORT_BY_TIME && lhs.s.st_mtime != rhs.s.st_mtime)
                return lhs.s.st_mtime < rhs.s.st_mtime;
            else
                return lhs.strName > rhs.strName;
        } else {
            if (options & SORT_BY_SIZE && lhs.s.st_size != rhs.s.st_size)
                return lhs.s.st_size > rhs.s.st_size;
            if (options & SORT_BY_TIME && lhs.s.st_mtime != rhs.s.st_mtime)
                return lhs.s.st_mtime > rhs.s.st_mtime;
            else
                return lhs.strName < rhs.strName;
        }
    }

    string getFileMode() {
        ostringstream ss;
        char c1[] = "pcdb-ls";                          // 文件类型
        int mask1[] = {S_IFIFO, S_IFCHR, S_IFDIR,
                       S_IFBLK, S_IFREG, S_IFLNK, S_IFSOCK};
        for (int n = 0; n < 7; n++) {
            if ((s.st_mode & S_IFMT) == mask1[n]) {
                ss << c1[n];
                break;
            }
        }
        char c2[] = "rwxrwxrwx-";                       // 属性
        int mask2[] = {S_IRUSR, S_IWUSR, S_IXUSR, S_IRGRP,
                       S_IWGRP, S_IXGRP, S_IROTH, S_IWOTH, S_IXOTH};
        for (int n = 0; n < 9; n++) {
            if (s.st_mode & mask2[n])
                ss << c2[n];
            else
                ss << c2[9];
        }
        return ss.str();
    }

    string getDateTime() {
        ostringstream ss;

        time_t today;
        time(&today);
        tm *t = localtime(&today);                  // 今天
        int thisYear = t->tm_year;
        t = localtime(&s.st_mtime);                 // 文件修改时间

        ss << right << setw(2) << t->tm_mon + 1 << " ";
        ss << right << setw(2) << t->tm_mday << " ";

        if (t->tm_year == thisYear) {               // 今年修改 则显示时间
            ss << put_time(t, "%H:%M");
        } else {                                    // 非今年修改 显示年份
            ss << put_time(t, " %Y");
        }

        return ss.str();
    }

};

struct Info {
    size_t maxInodeLen = 0;
    size_t maxLinksNumberLen = 0;
    size_t maxOwnerNameLen = 0;
    size_t maxGroupNameLen = 0;
    size_t maxBytesLen = 0;
    size_t maxNameLen = 0;
    unsigned long long blocks = 0;
};

// 获得最长长度以便格式化输出
Info getMaxLength(vector<MyFile> &files) {
    Info ret = Info();
    for (MyFile &f : files) {
        if ((options & INODE) && ret.maxInodeLen < f.strInode.length())
            ret.maxInodeLen = f.strInode.length();
        if (options & LONG_FORMAT) {
            if (ret.maxLinksNumberLen < f.strLinksNumber.length())
                ret.maxLinksNumberLen = f.strLinksNumber.length();
            if (ret.maxOwnerNameLen < f.strOwnerName.length())
                ret.maxOwnerNameLen = f.strOwnerName.length();

            if (ret.maxGroupNameLen < f.strGroupName.length())
                ret.maxGroupNameLen = f.strGroupName.length();
            if (ret.maxBytesLen < f.strBytes.length())
                ret.maxBytesLen = f.strBytes.length();
            ret.blocks += f.s.st_blocks;
        }
        if (ret.maxNameLen < f.strName.length())
            ret.maxNameLen = f.strName.length();
    }
    ret.maxNameLen = (ret.maxNameLen / 8 + 1) * 8;
    return ret;
}


void printUsage() {
    cerr << "用法： ./ls [-RSailrt1] [file ...]" << endl;
}

void display(vector<MyFile> files, const char *dirName) {

    Info info = getMaxLength(files);

    sort(files.begin(), files.end());                       // 排序

    if (dirName != nullptr)
        cout << dirName << ":" << endl;                     // 输出当前被查询文件夹
    if (options & LONG_FORMAT) {                            // 长格式输出
        cout << "total " << info.blocks << endl;            // total blocks
        for (auto &file : files) {
            if (options & INODE)
                cout << right << setw((int) info.maxInodeLen)
                     << file.strInode << " ";
            cout << file.getFileMode() << "  ";
            cout << right << setw((int) info.maxLinksNumberLen)
                 << file.strLinksNumber << " ";
            cout << left << setw((int) info.maxOwnerNameLen)
                 << file.strOwnerName << "  ";
            cout << left << setw((int) info.maxGroupNameLen)
                 << file.strGroupName << "  ";
            cout << right << setw((int) info.maxBytesLen)
                 << file.strBytes << " ";
            cout << file.getDateTime() << " ";
            cout << file.strName << endl;
        }
    } else {
        size_t c;
        size_t s = files.size();                            // 总数
        if (options & ONE_PER_LINE)
            c = 1;
        else {
            size_t len = (options & INODE)                  // 每条的总长度
                         ? info.maxNameLen + info.maxInodeLen + 1
                         : info.maxNameLen;
            c = cols / len;                                 // 列数
            if (c == 0) c = 1;
        }
        size_t r = s / c;                                   // 行数
        if (s % c > 0) r++;                                 // 向上取整
        for (size_t i = 0; i < r; i++) {
            for (size_t j = 0; j < c; j++) {
                if (j * r + i >= s) continue;
                if (options & INODE)
                    cout << right << setw((int) (info.maxInodeLen))
                         << files[j * r + i].strInode << " ";
                cout << left << setw((int) (info.maxNameLen))
                     << files[j * r + i].strName;
            }
            cout << endl;
        }
    }
}

int main(int argc, char *argv[]) {
    if ((homeDir = getenv("HOME")) == nullptr) {    // 获取home目录以便替换~
        homeDir = getpwuid(getuid())->pw_dir;
    }

    bool optionflag = true;
    int dirNum = 0;

    for (int i = 1; i < argc; i++) {                // 处理参数
        if (argv[i][0] != '-')
            optionflag = false;
        if (optionflag) {
            size_t len = strlen(argv[i]);
            for (int j = 1; j < len; j++) {
                switch (argv[i][j]) {
                    case 'l':
                        options |= LONG_FORMAT;
                        break;
                    case 'i':
                        options |= INODE;
                        break;
                    case 't':
                        options |= SORT_BY_TIME;
                        break;
                    case 'a':
                        options |= ALL;
                        break;
                    case '1':
                        options |= ONE_PER_LINE;
                        break;
                    case 'S':
                        options |= SORT_BY_SIZE;
                        break;
                    case 'r':
                        options |= REVERSE;
                        break;
                    case 'R':
                        options |= RECURSIVE;
                        break;
                    default:
                        cerr << "ls：非法参数 -- " << argv[i][j] << endl;
                        printUsage();
                        return EPERM;
                }
            }
        } else {
            dirNum++;
            MyDir d{};
            string path;
            if (argv[i][0] == '~') {
                path = homeDir;
                path += (argv[i] + 1);
            } else {
                path = argv[i];
            }

            d = {opendir(path.c_str()), path};
            if (d.dir == nullptr)
                cerr << "ls: " << argv[i]
                     << ": " << strerror(errno) << endl;
            else
                dirs.push(d);

        }
    }

    if (dirNum == 0) {
        MyDir d{opendir("."), "."};
        dirs.push(d);                        // 若不指定目录则查看当前目录
        dirNum++;
    }

    if (!(options & LONG_FORMAT)) {          // 若不是长格式获取控制台大小
        winsize w{};
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
        cols = (w.ws_col == 0) ? 82 : w.ws_col;
    }

    while (!dirs.empty()) {                     // 遍历所有被查询的文件夹
        MyDir dir = dirs.top();
        dirs.pop();
        vector<MyFile> files;                   // 存放当前目录的所有文件
        vector<MyDir> dirfiles;
        dirent *pDirent;
        while ((pDirent = readdir(dir.dir))) {  // 遍历每个文件夹的文件
            if (!(options & ALL)
                && pDirent->d_name[0] == '.')   // 过滤隐藏文件
                continue;
            MyFile file(dir.name, pDirent->d_name);
            if (options & RECURSIVE
                && S_ISDIR(file.s.st_mode)
                && file.strName != "."
                && file.strName != "..") {      // 递归查看文件夹
                string fullPath = dir.name + "/" + file.strName;
                DIR *d = opendir(fullPath.c_str());
                if (d != nullptr)
                    dirfiles.push_back({d, fullPath});

            }
            files.push_back(file);
        }
        for (auto i = dirfiles.rbegin(); i != dirfiles.rend(); i++)
            dirs.push(*i);

        closedir(dir.dir);
        display(files, dirNum > 1 || options & RECURSIVE
                       ? dir.name.c_str()
                       : nullptr);              // 输出files
        if (!dirs.empty()) cout << endl;        // 若没有结束则加一个空行
    }

    return 0;
}