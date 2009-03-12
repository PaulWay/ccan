/**
 * ilog - Integer logarithm.
 *
 * ILOG_32() and ILOG_64() compute the minimum number of bits required to store
 *  an unsigned 32-bit or 64-bit value without any leading zero bits.
 * This can also be thought of as the location of the highest set bit, with
 *  counting starting from one (so that 0 returns 0, 1 returns 1, and 2**31
 *  returns 32).
 * When the value is known to be non-zero ILOGNZ_32() and ILOGNZ_64() can
 *  compile into as few as two instructions, one of which may get optimized out
 *  later.
 * STATIC_ILOG_32 and STATIC_ILOG_64 allow computation on compile-time
 *  constants, so other compile-time constants can be derived from them.
 *
 * Example:
 *  #include <stdio.h>
 *  #include <limits.h>
 *  #include <ccan/ilog/ilog.h>
 *
 *  int main(void){
 *    int i;
 *    printf("ILOG_32(0x%08X)=%i\n",0,ILOG_32(0));
 *    for(i=1;i<=STATIC_ILOG_32(USHRT_MAX);i++){
 *      uint32_t v;
 *      v=(uint32_t)1U<<i-1;
 *      //Here we know v is non-zero, so we can use ILOGNZ_32().
 *      printf("ILOG_32(0x%08X)=%i\n",v,ILOGNZ_32(v));
 *    }
 *    return 0;
 *  }
 *
 * License: LGPL (v2 or later)
 */
#include <string.h>
#include "config.h"

int main(int _argc,const char *_argv[]){
  /*Expect exactly one argument.*/
  if(_argc!=2)return 1;
  if(strcmp(_argv[1],"depends")==0){
    /*PRINTF-CCAN-PACKAGES-YOU-NEED-ONE-PER-LINE-IF-ANY*/
    return 0;
  }
  return 1;
}
