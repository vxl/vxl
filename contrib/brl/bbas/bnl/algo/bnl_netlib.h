#ifndef BNL_NETLIB_H_
#define BNL_NETLIB_H_

//:
// \file
//  \brief 
//  \author Kongbin Kang
//
//

#include <b3p_netlib.h>

extern "C"{
  int simpru_(double (*f)(float*), float *a, float *b, int *m, float *srule);
};

#endif
