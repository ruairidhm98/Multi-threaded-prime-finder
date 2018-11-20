#include <limits.h>
#include <math.h>
#include "isprime.h"

/*
 * is_prime() - returns 1 if argument is prime, 0 if not
 */

int is_prime(unsigned long number) {
   
   unsigned long tNum, tLim, result, a;

   if (number == 1 || number == 2)
      return 1;
   if ((number % 2) == 0)
      return 0;
   
   a = 6;
   result = pow((double) a, (double) number);
   if (result < ULONG_MAX) 
       if ((result - a) % number == 0) return 1;
   
   for (tNum = 3, tLim = number; tLim > tNum; tLim = number / tNum, tNum += 2) {
      if ((number % tNum) == 0)
         return 0;
   }
   return 1;
}
