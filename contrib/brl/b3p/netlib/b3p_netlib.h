#ifndef B3P_NETLIB_H_
#define B3P_NETLIB_H_

// \:
//  \file
//  \author Kongbin Kang
//  \date Jan 5, 2005
//  \brief declearations for netlib functions

#ifdef __cplusplus
extern "C" {
#endif
int trapru_(double f(float*), float *a, float *b, int *m, float *	trule);

int simpru_(double f(float* ), float *a, float *b, int *m, float *srule);

//: input parameters
//  f: integrant
//  a: left endpoint
//  b: right endpoint
//  tol: 
//  
//  output parameters
//  errbdd: error estimation
//  m: substates
int adaptquad_(int f(float*), float *a, float *b, float *tol, float * srmat, float *integral, float *errbdd, int *m, int *state);

#ifdef __cplusplus
}
#endif


#endif 

