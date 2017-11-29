#include <iostream>
#include <fcntl.h>
#include <unistd.h>

int main() {

    int file = open("test", O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR); // 创建文件
    if (file == -1) {
        perror("无法创建目标文件");
        return errno;
    }

    pid_t pid;
    pid = fork();
    if (pid < 0) { // error
        printf("Fork Failed\n");
        return 1;
    }

    char buffer[100];
    sprintf(buffer, "pid: %d \n", getpid());

    write(file, buffer, strlen(buffer));

    if (pid != 0) {// parent process
        wait(nullptr);
    }
    return 0;
}