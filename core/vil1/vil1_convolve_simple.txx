/*
  fsm@robots.ox.ac.uk
*/
#include "vil_convolve.h"

#include <vcl/vcl_cassert.h>
#include <vcl/vcl_algorithm.h>
#include <vcl/vcl_cmath.h>
#include <vcl/vcl_vector.h>
#include <vcl/vcl_iostream.h>

#include <vil/vil_memory_image_of.h>
#include <vil/vil_ip_traits.h>

#define trace if (true) ; else cerr

//--------------------------------------------------------------------------------

template <class I1, class I2, class O, class AC>
void vil_convolve_simple(I1 const* const* input1, unsigned w1, unsigned h1,
			 I2 const* const* input2, unsigned w2, unsigned h2,
			 O       * const* out,
			 AC *)
{
  typedef typename vil_ip_traits<O* const*>::pixel_type OutType;

  trace << "w1 h1 = " << w1 << ' ' << h1 << endl;
  trace << "w2 h2 = " << w2 << ' ' << h2 << endl;

  // here we go : vrrrm.. vrrrm..
  for (int jo=0; jo<int(h1+h2)-1; ++jo) {
    for (int io=0; io<int(w1+w2)-1; ++io) {
      // initialize accumulator
      AC acc(0);
      
      // bounds.
      // The type unification complains there is no max(int, unsigned) 
      // template. On gcc 2.95 we could use max<int> to disambiguate,
      // but SGI native doesn't accept that. Hence the explict casts.
      // fsm
      int ibeg = vcl_max(int( 0), int(io-w2+1));
      int iend = vcl_min(int(w1), int(io+1));
      int jbeg = vcl_max(int( 0), int(jo-h2+1));
      int jend = vcl_min(int(h1), int(jo+1));

      // accumulate
      for (int j1=jbeg; j1<jend; ++j1) {
	for (int i1=ibeg; i1<iend; ++i1) {
	  int i2 = io - i1;
	  int j2 = jo - j1;
	  acc += AC(input1[j1][i1]) * AC(input2[j2][i2]);
	}
      }
      
      // assign
      out[jo][io] = OutType(acc);
    }
  }
}

template <class I1, class I2, class O, class AC>
void vil_convolve_simple(vil_memory_image_of<I1> const &input1,    // input 1
			 int x1, int y1, unsigned w1, unsigned h1,
			 vil_memory_image_of<I2> const &input2,    // input 2
			 int x2, int y2, unsigned w2, unsigned h2,
			 vil_memory_image_of<O>        &output,    // ouput
			 int xo, int yo,
			 AC *)
{
  // assert that the memory to be touched may be touched :
  assert( input1.in_range(x1, y1, w1, h1) );
  assert( input2.in_range(x2, y2, w2, h2) );
  assert( output.in_range(xo, yo, w1+w2-1, h1+h2-1) );

  //
  vcl_vector<I1 const *> in1(h1);
  for (unsigned k=0; k<h1; ++k)
    in1[k] = input1[y1+k] + x1;
  trace << in1.size() << " rasters in in1" << endl;
  
  //
  vcl_vector<I2 const *> in2(h2);
  for (unsigned k=0; k<h2; ++k)
    in2[k] = input2[y2+k] + x2;
  trace << in2.size() << " rasters in in2" << endl;

  //
  vcl_vector<O *> out(h1+h2-1);
  for (unsigned k=0; k<h1+h2-1; ++k)
    out[k] = output[yo+k] + xo;
  trace << out.size() << " rasters in out" << endl;

  // call the even simpler routine
  vil_convolve_simple/*<I1, I2, O, AC>*/(in1.begin(), w1, h1,
					 in2.begin(), w2, h2,
					 out.begin(),
					 (AC*)0);
}

// out_{off+k} = \sum_{i+j = k} a_{off+i} b_{off+j}
template <class I1, class I2, class O, class AC>
void vil_convolve_simple(vil_memory_image_of<I1> const &in1,
			 vil_memory_image_of<I2> const &in2,
			 vil_memory_image_of<O>        &out,
			 AC *)
{
  vil_convolve_simple/*<I1, I2, O, AC>*/(in1.row_array(), in1.width(), in1.height(),
					 in2.row_array(), in2.width(), in2.height(),
					 out.row_array(),
					 (AC*)0);
}
 

//--------------------------------------------------------------------------------

#define VIL_CONVOLVE_SIMPLE_INSTANTIATE0(I1, I2, O, AC) \
template void vil_convolve_simple/*<I1, I2, O, AC >*/(I1 const * const [], unsigned, unsigned,  \
                                                      I2 const * const [], unsigned, unsigned,  \
                                                      O * const [], \
                                                      AC *);

#define VIL_CONVOLVE_SIMPLE_INSTANTIATE1(I1, I2, O, AC) \
template void vil_convolve_simple/*<I1, I2, O, AC >*/(vil_memory_image_of<I1> const &, \
                                                      int, int, unsigned, unsigned,    \
                                                      vil_memory_image_of<I2> const &, \
                                                      int, int, unsigned, unsigned,    \
                                                      vil_memory_image_of<O>        &, \
                                                      int, int, \
                                                      AC *);

#define VIL_CONVOLVE_SIMPLE_INSTANTIATE2(I1, I2, O, AC) \
template void vil_convolve_simple/*<I1, I2, O, AC >*/(vil_memory_image_of<I1> const &, \
                                                      vil_memory_image_of<I2> const &, \
                                                      vil_memory_image_of<O>        &, \
                                                      AC *);

// fsm@robots: with 2.7 the assembler complains that this symbol is defined
// twice. moreover, it is defined twice, once .global and once .weak
// if someone works out what is going wrong I'd be interested to know.
#if defined(VCL_GCC_27)
#undef VIL_CONVOLVE_SIMPLE_INSTANTIATE0
#define VIL_CONVOLVE_SIMPLE_INSTANTIATE0(I1, I2, O, AC) /* */
#endif

// at last, the macro we've all been waiting for :
#define VIL_CONVOLVE_SIMPLE_INSTANTIATE(I1, I2, O, AC) \
VIL_CONVOLVE_SIMPLE_INSTANTIATE0(I1, I2, O, AC); \
VIL_CONVOLVE_SIMPLE_INSTANTIATE1(I1, I2, O, AC); \
VIL_CONVOLVE_SIMPLE_INSTANTIATE2(I1, I2, O, AC)
