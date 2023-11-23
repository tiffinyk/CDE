#include <string.h>
#include <stdio.h>


#define SIZE  20
int A[SIZE], B[SIZE], C[SIZE];
//void please_map_me();

void array_add(){  
   /*int a = 20;
   for (int s = 0; s < a; s++)
   {
      for (int i=0; i < s; i++)
      {
         #ifdef CGRA_COMPILER
            please_map_me();
         #endif
         C[i] = A[i]+B[i];
      }
   }*/
   for (int i=0; i<SIZE; i++){
      #ifdef CGRA_COMPILER
         please_map_me();
      #endif
      C[i] = A[i]+B[i];
   }
}

int main(){

   int i;
   for (i=0;i<SIZE; i++){
      A[i] = i * 2 + 5;
      B[i] = i * 3;
      C[i] = 0;
    }
    
   array_add();

   for (i=0;i<SIZE; i++){
      printf("%d\n", C[i]);
   }
   return 0;

}
