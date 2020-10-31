#include "utils.h"

#include <errno.h>
#include <time.h>

#include <sstream>
#include <iomanip>

#include <boost/crc.hpp>

using namespace std;
//生成校验码
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
//将10进制数转换成固定长度的字符串并把多余值置0
std::string Utils::DecToStr(int para, int w){
  int max = 1;
  int min = -1;
  int n = w;
  if (w <= 1 && para <= 0)
    return w==1?"0":"";

  while (n--) {
    max *= 10;
    min *= 10;
  }
  std::ostringstream ss;
  if (para >= max) {
    ss << std::setfill('9') << std::setw(w) << "";
  } else if (para <= (min/10)) {
    ss << '-' << std::setfill('9') << std::setw(w-1) << "";
  } else {
    ss << std::setfill('0') << std::setw(w) << para;
  }
  ss.flush();
  return ss.str();
}

std::pair<unsigned, unsigned> Utils::Crc16AndXmodem(const void *b, size_t l) {
  std::pair<unsigned, unsigned>  result;
  boost::crc_basic<16> crc1( 0x8005u, 0u, 0u, true, true );

  crc1.process_bytes( b, l );
  result.first = crc1.checksum();

  crc1 = boost::crc_basic<16>( 0x8408u, crc1.get_initial_remainder(),
  crc1.get_final_xor_value(), crc1.get_reflect_input(),
  crc1.get_reflect_remainder() );
  crc1.process_bytes( b, l );
  result.second = crc1.checksum();
  return result;
}
