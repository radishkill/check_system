#include "eventmanager.h"

#include <iostream>

#include <sys/epoll.h>
#include <sys/eventfd.h>

namespace check_system {

EventManager::EventManager()
    : is_running_(false) {
  epoll_fd_ = epoll_create(0);

  event_fd_ = eventfd(0, 0);

  struct epoll_event ev;
  ev.data.u64 = 0;
  ev.events = EPOLLIN;
  ev.data.fd = event_fd_;
  epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, event_fd_, &ev);
}

EventManager::~EventManager() {

}

bool EventManager::Start(int num_threads) {
  std::lock_guard<std::mutex> l(mutex_);

//  if (thread_set_.find(std::this_thread) != thread_set_.end()) {
//    return false;
//  }

  is_running_ = true;
//  for (int i = 0; i < num_threads; ++i) {
//    std::thread th;
//    thread_set_.insert(th);
//  }
  RunThread();
  return true;
}

void EventManager::ListenFd(int fd, EventManager::EventType type, function<void ()> f) {
  std::lock_guard<std::mutex> l(mutex_);
  if (fds_.count(fd)) {
    fds_[fd][type] = f;
    EpollUpdate(fd, EPOLL_CTL_ADD);
  } else {
    fds_[fd][type] = f;
    EpollUpdate(fd, EPOLL_CTL_MOD);
  }
  eventfd_write(event_fd_, 1);
}

bool EventManager::RemoveFd(int fd, EventManager::EventType type) {
  std::lock_guard<std::mutex> l(mutex_);

  if (fds_.find(fd) == fds_.end()) {
    return false;
  }
  if (fds_[fd].size() == 1 &&
      fds_[fd].find(type) != fds_[fd].end()) {
    EpollUpdate(fd, EPOLL_CTL_DEL);
    fds_.erase(fd);
  } else {
    fds_[fd].erase(type);
    EpollUpdate(fd, EPOLL_CTL_MOD);
  }
  return true;
}

void EventManager::EpollUpdate(int fd, int epoll_op) {
  struct epoll_event ev;
  ev.data.u64 = 0;  // stop valgrind whinging
  ev.events = 0;
  for (auto i = fds_[fd].begin(); i != fds_[fd].end(); ++i) {
    switch (i->first) {
      case EventManager::kEventRead:
        ev.events |= EPOLLIN;
        break;
      case EventManager::kEventWrite:
        ev.events |= EPOLLOUT;
        break;
      case EventManager::kEventPri:
        ev.events |= EPOLLPRI;
        break;
      case EventManager::kEventError:
        ev.events |= EPOLLRDHUP | EPOLLHUP;
        break;
      default:
        break;
    }
  }
  ev.data.fd = fd;

  epoll_ctl(epoll_fd_, epoll_op, fd, &ev);
}

void EventManager::RunThread() {
  const int kMaxEvents = 32;
//  const int kEpollDefaultWait = 10000;

  struct epoll_event events[kMaxEvents];
  std::unique_lock<std::mutex> lk(mutex_);
  while (is_running_) {
    int timeout = -1;
    lk.unlock();
    //该函数用于轮询I/O事件的发生
    int ret = epoll_wait(epoll_fd_, events, kMaxEvents, timeout);
    lk.lock();

    if (ret < 0) {
      if (errno != EINTR) {
        std::cout << "Epoll error: " << errno << " fd is " << epoll_fd_;
      }
      continue;
    }

    // Execute triggered fd handlers
    for (int i = 0; i < ret; i++) {
      int fd = events[i].data.fd;
      if (fd == event_fd_) {
        uint64_t val;
        eventfd_read(event_fd_, &val);
      } else {
        int flags = events[i].events;
        if ((flags | EPOLLIN) &&
            fds_.find(fd) != fds_.end() &&
            fds_[fd].find(kEventRead) != fds_[fd].end()) {
          function<void()> f = fds_[fd][kEventRead];
          lk.unlock();
          f();
          lk.lock();
        }
        if ((flags | EPOLLOUT) &&
            fds_.find(fd) != fds_.end() &&
            fds_[fd].find(kEventWrite) != fds_[fd].end()) {
          function<void()> f = fds_[fd][kEventWrite];
          lk.unlock();
          f();
          lk.lock();
        }
        if ((flags | EPOLLHUP | EPOLLRDHUP) &&
            fds_.find(fd) != fds_.end() &&
            fds_[fd].find(kEventError) != fds_[fd].end()) {
          function<void()> f = fds_[fd][kEventError];
          lk.unlock();
          f();
          lk.lock();
        }
      }
    }
  }
 }
}
