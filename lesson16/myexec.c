#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<assert.h>
#include<sys/wait.h>
#include<sys/types.h>

int main(int argc, char *argv[])
{
    printf("process is running\n!");
    pid_t id = fork();
    assert(id!=-1);
    if(id == 0)
    {
        sleep(1);
        // execvp(argv[1], &argv[1]);
    //    execlp("ls", "ls","-a", "-l", NULL);
        char *arv_[] = { "mycpp", NULL };
        execvp("./mybin", arv_);
    //    printf("原子进程!\n");
       //  char* const envp_[] = {
          //   (char*)"MYENV=1111111111",
           //  NULL
       //  };
    //    execv("/usr/bin/ls", arv_);
       //  extern char** environ;
       //  execle("./mybin","mybin",NULL, envp_);  // 最后一个为自定义环境变量
       //  putenv((char*)"MYENV=444323232"); // 将指定环境变量导入到系统中 environ指向的表
       //  execle("./mybin","mybin",NULL, environ); // 系统的环境变量，默认不传，子进程也会获取
        exit(10);
    }
    // 下面为父进程的代码，子进程的替换不会影响父进程的代码
    int status = 0;
    int ret = waitpid(id, &status, 0);
    if(ret > 0) printf("wait success: exit code:%d, sig:%d\n", (status>>8) & 0xFF, status & 0x7F);
//    pid_t id = fork();
    // 用我们的程序将别人的程序执行起来
    // .c -> exe -> load -> process -> 运行 -> 执行我们现在所写的代码
    // printf("process is running...\n");
    // 只要是个函数，掉用就有可能失败 就是没有替换成功 继续执行下面的原代码
    // 只有错误时会返回，返回值为-1
    // execl("/usr/bin/ls"/*要执行哪个程序*/, "ls","-a", "-l", "--color=auto", NULL/*你想怎么执行*/);  // 所有的execl* 系列的接口都必须以NULL结尾
    // printf("execl\n");
    // 为何下面的printf没有执行呢？ printf是在execl之后的，execl执行完毕后，代码已经完全被替换 开始新的程序的代码了
    // printf("process running done...\n");
    return 0;
}
