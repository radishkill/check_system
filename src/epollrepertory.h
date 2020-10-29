#ifndef EPOLLREPERTORY_H
#define EPOLLREPERTORY_H

#include <sys/epoll.h>
#include <string.h>
#include <ctime>
#include <iostream>

#include "fd_manager.h"


class EpollRepertory : public FdManage
{
public:
    EpollRepertory();
    void EventAdd(FdContext &fdMsg);
    void EventDel(FdContext &fdMsg);
    void EventDel(int fd);
    void EventMod(int events, int fd);
    int GetEpollFd() {return epoll_fd_;}
private:
    int epoll_fd_;
};

#endif // EPOLLREPERTORY_H
