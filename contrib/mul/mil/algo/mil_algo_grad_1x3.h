#ifndef mil_algo_grad_1x3_h_
#define mil_algo_grad_1x3_h_
//: \file
//  \brief Apply gradient operator to 2D planes of data
//  \author Tim Cootes, Ian Scott

//: Compute gradients of single plane of 2D data using simple 1x3 filters
//  Computes both x and y gradients of an nx x ny plane of data
void mil_algo_grad_1x3(float* gx, int gx_xstep, int gx_ystep,
                       float* gy, int gy_xstep, int gy_ystep,
                       const unsigned char* src,
             int s_xstep, int s_ystep, unsigned nx, unsigned ny);

//: Compute gradients of single plane of 2D data using simple 1x3 filters
//  Computes both x and y gradients of an nx x ny plane of data
void mil_algo_grad_1x3(float* gx, int gx_xstep, int gx_ystep,
                       float* gy, int gy_xstep, int gy_ystep,
                       const float* src,
             int s_xstep, int s_ystep, unsigned nx, unsigned ny);

#endif
