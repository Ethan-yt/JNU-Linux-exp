#include <iostream>
#include <unistd.h>
#include <signal.h>

void handler(int signo) {
    std::cout << "[子进程]收到信号:" << signo << "\tpid:" << getpid() << "\t父进程pid:" << getppid() << std::endl;
    std::cout << "[子进程]子进程退出" << std::endl;
    exit(0);
}

int main() {
    int pid;
    pid = fork();
    if (pid < 0) {
        std::cout << "[父进程]创建子进程失败" << std::endl;
    } else if (pid == 0) {
        signal(SIGUSR1, handler);
        while (true);
    } else {
        std::cout << "[父进程]创建子进程成功，等待子进程绑定信号" << std::endl;
        sleep(2);
        std::cout << "[父进程]向pid:" << pid << "发送信号" << std::endl;
        kill(pid, SIGUSR1);
        std::cout << "[父进程]等待子进程" << std::endl;
        int status;
        wait(&status);
        std::cout << "[父进程]子进程退出 返回值:" << status << std::endl;
    }


    return 0;
} 