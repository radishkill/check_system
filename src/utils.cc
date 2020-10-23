#include "utils.h"

#include <errno.h>
#include <time.h>

unsigned char Utils::CheckSum(unsigned char *p, int datalen) {
  unsigned char cksum = 0;
  int i;
  for (i = 0; i < datalen; i++) {
    cksum += *(p+i);
  }
  return (~cksum);
}

int Utils::MSleep(unsigned int ms, bool force_sleep) {
  int retval;
  int save_errno;
  struct timespec ts;
  ts.tv_sec = ms / 1000;
  ms -= (ts.tv_sec * 1000);
  ts.tv_nsec = ms * 1000 * 1000;
  do {
    retval = nanosleep(&ts, &ts);
    save_errno = errno;
  } while (retval == -1 && save_errno == EINTR && force_sleep);
  return 0;
}
