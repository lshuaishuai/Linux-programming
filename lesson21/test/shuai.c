#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<string.h>
#include<unistd.h>

#define FILENAME "log.txt"

int main()
{
   //  int fd = open(FILENAME, O_WRONLY | O_CREAT, 0666);
    
    //int fd = open(FILENAME, O_RDONLY, 0666);
    // if(fd < 0)
    // {
     //    perror("open");
      //   return 1;
   //  }

   //  int count = 5;
   // char* msg = "hello shuaishuai\n";
   //  while(count--)
   //  {
   //      write(fd, msg, strlen(msg));
   //  }
    // char buf[1024];
   //  while(1)
   //  {
     //    ssize_t s = read(fd, buf, strlen(msg));
       // if(s>0)
     //   {
     //        printf("%s",buf);
       //  }
    //     else break;
    // }

   //  char buf[1024];
   //  ssize_t s = read(0, buf, sizeof(buf));
   //  if(s > 0)
   //  {
   //      buf[s] = 0;
   //      write(1, buf, strlen(buf));
   //      write(2, buf, strlen(buf));
   //  }
    
    int fd = open("out.txt", O_CREAT | O_TRUNC | O_RDWR, 0666);
    if(fd  < 0)
    {
        perror("open");
        return 1;
    }
    close(1);
    dup2(fd, 1);
    char buf[1024];
    while(1)
    {
        ssize_t s = read(0, buf, sizeof(buf)-1);
        if(s < 0)
        {
            perror("read");
            break;
        }
        else{
            printf("%s", buf);
            fflush(stdout);
        }
    }

    return 0;
}
