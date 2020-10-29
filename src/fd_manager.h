#ifndef FD_MANAGER_H
#define FD_MANAGER_H


#include <unistd.h>

#include <iostream>
#include <list>

enum FdType {
    DATATYPE_TCP, DATATYPE_UDP, DATATYPE_USART, DATATYPE_CAN, DATATYPE_GPIO, DATATYPE_NULL
};

struct FdContext
{
    FdType type;
    int fd;
    int status;
    char addr[16];
    void (*call_back)(int fd, unsigned int events, void *arg);
    unsigned int events;
    void *arg;
    bool operator == (const FdContext &b) const
    {
        return fd == b.fd;
    }
};

class FdManage {
private:
    std::list<FdContext> fd_list_;
public:
    FdManage()
    {
    }
    FdContext *Append(FdContext fdMsg)
    {
        FdContext *p;
        fd_list_.push_back(fdMsg);
        p = &(*(--fd_list_.end()));//取地址
        return p;
    }
    void Drop(int fd)
    {
        FdContext fdMsgTemp;
        fdMsgTemp.fd = fd;
        fd_list_.remove(fdMsgTemp);
        std::cout<<fd<<":closed"<< std::endl;
        close(fd);
    }
    void Drop(FdContext fdMsg) {
        fd_list_.remove(fdMsg);
        std::cout << fdMsg.fd << ":closed" << std::endl;
        close(fdMsg.fd);
    }
    FdContext *FindMsg(int fd)
    {
        std::list<FdContext>::iterator iter = fd_list_.begin();
        while (iter != fd_list_.end()) {
            if ((*iter).fd == fd) {
                return &(*iter);
            }
            iter++;
        }
        return NULL;
    }
    std::list<FdContext> *GetList()
    {
        return &fd_list_;
    }
    void SetStatusByFd(int fd, int status)
    {
        FdContext *fm;
        fm = FindMsg(fd);
        if (fm != NULL) {
            fm->status = status;
        }
    }
};

#endif // FD_MANAGER_H
