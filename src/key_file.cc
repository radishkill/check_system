#include "key_file.h"

/*
 * 这里面的步骤应该是
 * 1. 检查base_path是否存在  不存在就报错返回
 * 2. 检查PUF00 是否存在  不存在就报错返回
 * 2. 检查PUF00中是否包含1000个激励相应对 没有就报错返回 （可以跳过）
 *
 */
KeyFile::KeyFile(const char* base_path) {
  base_path_ = base_path;
}

int KeyFile::AppendPufFile() {
  return 0;
}
