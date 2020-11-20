#ifndef CHECK_SYSTEM_EVENTMANAGER_H
#define CHECK_SYSTEM_EVENTMANAGER_H
#include <algorithm>
#include <list>
#include <map>
#include <set>
#include <functional>
#include <memory>
#include <vector>
#include <thread>
#include <mutex>

namespace check_system {
using namespace std;

class EventManager {
 public:
  enum EventType {
    kEventRead,
    kEventWrite,
    kEventPri,
    kEventError
  };
  EventManager();
  virtual ~EventManager();
  bool Start(int num_threads);
  void ListenFd(int fd, EventType type, function<void()> f);
  bool RemoveFd(int fd, EventType type);
  void EpollUpdate(int fd, int epoll_op);
  int GetFd() {return epoll_fd_;}
 private:
  std::set<thread> thread_set_;
  int epoll_fd_;
  int event_fd_;
  int is_running_;
  std::map<int, std::map<EventType, function<void()>>> fds_;
  std::mutex mutex_;
  void RunThread();
};

}

#endif // CHECK_SYSTEM_EVENTMANAGER_H
