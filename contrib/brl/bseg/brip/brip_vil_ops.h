// This is brl/bseg/brip/brip_vil_ops.h
#ifndef brip_vil_ops_h_
#define brip_vil_ops_h_
//:
// \file
// \brief This file contains templated, vil1 versions code provided by brip_float_ops
// \author Matt Leotta, (mleotta@lems.brown.edu)
// \date Nov 12 2003
//
// This file contains several useful image operations originally defined in
// brip_float_ops for vil1 floating point images.  Some of the original operations
// are covered in the new vil library, these are use when possible and not duplicated here.
//
// \verbatim
//  Modifications
// \endverbatim

#include <vil/vil_image_view.h>
#include <vil/vil_math.h>
#include <vil/vil_fill.h>
#include <vil/algo/vil_sobel_3x3.h>

//: computes the conditioning of the 2n+1 x 2n+1 gradient neigborhood
template<class T>
inline void
brip_sqrt_grad_singular_values(const vil_image_view<T>& input, vil_image_view<T>& output, unsigned n)
{
  unsigned N = (2*n+1)*(2*n+1);
  vil_image_view<T> grad_i, grad_j;
  vil_sobel_3x3(input, grad_i, grad_j);

  unsigned ni = input.ni();
  unsigned nj = input.nj();
  unsigned np = input.nplanes();
  output.set_size(ni, nj, np);
  vil_fill(output,(T)1);

  for (unsigned p=0; p<np; ++p){
    for (unsigned j=n; j<nj-n; ++j){
      for (unsigned i=n; i<ni-n; ++i){
        T IxIx=(T)0, IxIy=(T)0, IyIy=(T)0;
        for (int x = -n; x<=(int)n; ++x)
          for (int y = -n; y<=(int)n; ++y){
            T gx = grad_i(i+x, j+y,p), gy = grad_j(i+x, j+y,p);
          IxIx += gx*gx;
          IxIy += gx*gy;
          IyIy += gy*gy;
        }
        // calculate the absolute value of the determinate (should work of all types)
        T IxIxIyIy = IxIx*IyIy;
        T IxIy2 = IxIy*IxIy;
        T abs_det = (IxIxIyIy>IxIy2?(IxIxIyIy-IxIy2):(IxIy2-IxIxIyIy))/N;
        vil_math_sqrt_functor vil_sqrt;
        output(i,j,p)=vil_sqrt(abs_det);
      }
    }
  }

  // fill in the boundary with zeros
  for (unsigned c=0; c<n; ++c){
    vil_fill_row(output, c, (T)0);
    vil_fill_row(output, nj-c-1, (T)0);
    vil_fill_col(output, c, (T)0);
    vil_fill_col(output, ni-c-1, (T)0);
  }
}

#endif // brip_vil_ops_h_
