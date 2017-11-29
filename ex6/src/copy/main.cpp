#include <iostream>
#include <fcntl.h>
#include <zconf.h>

#define BUF_SIZE 5

int main(int argc, char *argv[]) {

    if (argc != 3) {
        std::cout << "用法：copy 源文件 目标文件" << std::endl;
        return EPERM;
    }

    int srcFile = open(argv[1], O_RDONLY); // 打开文件
    if (srcFile == -1) {
        perror("无法打开源文件");
        return errno;
    }

    int dstFile = open(argv[2], O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR); // 创建文件
    if (dstFile == -1) {
        perror("无法创建目标文件");
        return errno;
    }

    char buffer[BUF_SIZE], *ptr;
    ssize_t bytesRead, bytesWrite;
    while ((bytesRead = read(srcFile, buffer, BUF_SIZE)) != 0) {
        if ((bytesRead == -1) && (errno != EINTR)) {
            perror("读文件时遇到错误");
            return errno;
        } else if (bytesRead > 0) {
            ptr = buffer;
            while ((bytesWrite = write(dstFile, ptr, (size_t) bytesRead)) != 0) {
                if ((bytesWrite == -1) && (errno != EINTR)) {
                    perror("写文件时遇到错误");
                    return errno;
                }
                std::cout << "已复制" << bytesWrite << "字节" << std::endl;
                if (bytesWrite == bytesRead) break;
                else if (bytesWrite > 0) {
                    ptr += bytesWrite;
                    bytesRead -= bytesWrite;
                }

            }

        }
    }
    std::cout << "操作完成!" << std::endl;

    close(srcFile);
    close(dstFile);
    return 0;
}