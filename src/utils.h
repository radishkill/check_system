#ifndef UTILS_H
#define UTILS_H

#include <linux/types.h>


class Utils {
  public:
  static unsigned char CheckSum(unsigned char* p, int datalen);
  static int MSleep(unsigned int ms, bool force_sleep = true);
};


#endif // UTILS_H
