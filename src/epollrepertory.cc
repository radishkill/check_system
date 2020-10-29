#include "epollrepertory.h"

EpollRepertory::EpollRepertory()
{
  epoll_fd_ = epoll_create1(0);
}

void EpollRepertory::EventAdd(FdContext &fdMsg) {
    //向管理器里面添加数据
    FdContext *p = this->Append(fdMsg);
    struct epoll_event epv = {0, {nullptr}};

    epv.data.ptr = p;
    epv.events = fdMsg.events;

    if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, fdMsg.fd, &epv) < 0) {
        std::cout << "add error" << std::endl;
    }
}

void EpollRepertory::EventDel(FdContext &fdMsg) {
    this->Drop(fdMsg);
    struct epoll_event epv = {0, {0}};

    epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, fdMsg.fd, &epv);
}

void EpollRepertory::EventDel(int fd) {
    FdContext fdMsg;
    fdMsg.fd = fd;
    this->Drop(fdMsg);
    struct epoll_event epv = {0, {0}};

    epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, fdMsg.fd, &epv);
}

void EpollRepertory::EventMod(int events, int fd) {

}
