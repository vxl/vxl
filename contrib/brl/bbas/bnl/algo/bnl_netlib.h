#ifndef BNL_NETLIB_H_
#define BNL_NETLIB_H_

//:
// \file
//  \brief 
//  \author Kongbin Kang
//
//

extern "C"{
  int simpru_(double (*f)(float*), float *a, float *b, int *m, float *srule);
  int adaptquad_(double (*f)(float*), float *a, float *b, float *tol, float *srmat, float *integral, float* errbnd, int* m, int *stat); 
};

#endif
