#include <iostream>
#include <thread>

#include <stdlib.h>


int main() {
  std::thread th([](){
    std::cout << "test" << std::endl;
  });  
  std::cout << "test" << std::endl;  
  return 0;
}