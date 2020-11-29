// a single header file is required
#include <ev.h>

#include <iostream>
#include <string>

// all watcher callbacks have a similar signature
// this callback is called when data is readable on stdin
static void
stdin_cb(EV_P_ ev_io *w, int revents)
{
  std::cout << "get:";
  std::string str;
  std::cin >> str;
  std::cout << str << std::endl;;
}

// another callback, this time for a time-out
static void
timeout_cb(EV_P_ ev_timer *w, int revents)
{
  std::cout << "e" << std::endl;
  std::cout << ev_time() << std::endl;
}

int main(void)
{
  int a = 2359126;
  std::cout << std::hex << ((a/0x100)&0xff) << " " << a%0x100 << std::endl;

  // break was called, so exit
  return 0;
}