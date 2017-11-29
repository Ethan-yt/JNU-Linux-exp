#include <iostream>
#include<unistd.h>

int main(int argc,char **argv)
{
    static const char mesg[]="Happy New Years to you!";
    char buf[BUFSIZ];
    size_t rcount,wcount;
    int p_fd[2];
    size_t n;

    if(pipe(p_fd)<0){
        fprintf(stderr,"%s:pipe failed:%s\n",argv[0],strerror(errno));
        exit(1);
    }
    printf("Read end=fd %d,write end=fd %d\n",p_fd[0],p_fd[1]);
    n=strlen(mesg);
    if((wcount=write(p_fd[1],mesg,n))!=n){
        fprintf(stderr,"%s:write failed:%s\n",argv[0],strerror(errno));
        exit(1);
    }
    if((rcount=read(p_fd[0],buf,BUFSIZ))!=wcount){
        fprintf(stderr,"%s:read failed:%s\n",argv[0],strerror(errno));
        exit(1);
    }
    buf[rcount]='\0';
    printf("Read <%s> from pipe\n",buf);
    close(p_fd[0]);
    close(p_fd[1]);
    return 0;
}
