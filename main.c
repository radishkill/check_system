#include <stdio.h>
int main(int argc, char* argv[])
{
#pragma omp parallel for  //后面是for循环
     for (int i = 0; i < 10; i++ )
     {
         printf("i = %d\n", i);
     }
     return 0;
}
