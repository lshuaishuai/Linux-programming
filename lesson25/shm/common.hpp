#ifndef _COMM_HPP_
#define _COMM_HPP_

#include <iostream>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <cerrno>
#include <cstring>
#include <cstdlib>
#include <unistd.h>

#define PATHNAME "."
#define PROJ_ID 0x66
#define MAX_SIZE 4096

key_t getKey()
{
    key_t k = ftok(PATHNAME, PROJ_ID);  // 可以获取同样的key！
    if(k < 0)
    {
        // cin cout cerr -> stdin stdout stderr -> 0,1,2
        std::cerr << errno << ":" << strerror(errno) << std::endl;
        exit(1);
    }
    return k;
}

int getShmHelper(key_t k, int flags)
{
    int shmId = shmget(k, MAX_SIZE, flags);
    if(shmId < 0)
    {
        std::cerr << errno << ":" << strerror(errno) << std::endl;
        exit(2);
    }
    return shmId;
}

// 给之后的进程获取共享内存
int getShm(key_t k)
{
    return getShmHelper(k, IPC_CREAT/*可以设定为0*/); 
}

// 给第一个进程使用 创建共享内存 
int creatShm(key_t k)
{
    return getShmHelper(k, IPC_EXCL | IPC_CREAT | 0600); // 0600为权限
}

void *attachShm(int shmId)
{
    void *mem = shmat(shmId, nullptr, 0);  // 64位系统 指针占8字节
    if((long long)mem == -1L)
    {
        std::cerr << "shmat " << errno << ":" << strerror(errno) << std::endl;
        exit(3);
    }
    return mem;
}

void detachShm(void *start)
{
    if(shmdt(start) == -1)
    {
        std::cerr << errno << ":" << strerror(errno) << std::endl;
    }
}

void delShm(int shmId)
{
    if(shmctl(shmId, IPC_RMID, nullptr) == -1)
    {
        std::cerr << errno << ":" << strerror(errno) << std::endl;
    }
}
 
#endif