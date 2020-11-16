#ifndef UTILS_H
#define UTILS_H

#include <linux/types.h>
#include <iostream>
#include <boost/crc.hpp>

class Utils {
  public:
  static unsigned char CheckSum(unsigned char* p, int datalen);
  static int MSleep(unsigned int ms, bool force_sleep = true);
  static std::string DecToStr(int para,int w);
  static std::pair<unsigned, unsigned> Crc16AndXmodem(void const* b, std::size_t l);
};


#endif // UTILS_H
