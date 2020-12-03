
#include <iostream>
#include <string>
#include <ctime>
#include <cstdlib>

int main(int argc, char** argv)
{
  int seed = 0;
  int n = 0;
  
  seed = std::atoi(argv[1]);
  n = std::atoi(argv[2]);
  std::cout << "seed=" << seed << std::endl;
  std::cout << "n=" << n << std::endl;
  std::srand(seed);
  for (int i = 0; i < n; i++) {
    std::rand();
    if (i == n-1) {
      std::cout << std::rand()%0x100 << std::endl;
    }
  }

  return 0;
}