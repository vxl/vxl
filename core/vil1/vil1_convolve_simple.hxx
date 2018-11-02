// This is core/vil1/vil1_convolve_simple.hxx
#ifndef vil1_convolve_simple_hxx_
#define vil1_convolve_simple_hxx_
//:
// \file
// \author fsm

#include <algorithm>
#include <vector>
#include <iostream>
#include "vil1_convolve_simple.h"

#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#ifndef TRACE
#define TRACE 0
#endif
#if TRACE
#endif

//--------------------------------------------------------------------------------

//:
// fsm: With 2.7 the assembler complains that this symbol is defined
// twice. Moreover, it \e is defined twice (both .weak), even if we don't
// explicitly instantiate it. If explicitly instantiated, it is defined once
// .global and once .weak. Presumably, the compiler incorrectly thinks it needs
// to instantiate it twice just because it is called twice.
// To get around this annoying bug without breaking up the source file, this
// function must be called through a pointer-to-function on gcc 2.7.2, since
// that seems to trick the compiler into emitting only one instantiation.
template <class I1, class I2, class AC, class O>
void vil1_convolve_simple(I1 const* const* input1, unsigned w1, unsigned h1,
                          I2 const* const* input2, unsigned w2, unsigned h2,
                          AC *,
                          O       * const* out)
{
  //?? typedef typename vil1_ip_traits<O* const*>::pixel_type OutType;

#if TRACE
  std::cerr << "w1 h1 = " << w1 << ' ' << h1 << std::endl;
  std::cerr << "w2 h2 = " << w2 << ' ' << h2 << std::endl;
#endif

  // here we go : vrrrm.. vrrrm..
  for (int jo=0; jo<int(h1+h2)-1; ++jo) {
    for (int io=0; io<int(w1+w2)-1; ++io) {
      // initialize accumulator
      AC acc(0);

      // bounds.
      // The type unification complains there is no max(int, unsigned)
      // template. Hence the explicit casts.
      int ibeg = std::max(int( 0), int(io-w2+1));
      int iend = std::min(int(w1), int(io+1));
      int jbeg = std::max(int( 0), int(jo-h2+1));
      int jend = std::min(int(h1), int(jo+1));

      // accumulate
      for (int j1=jbeg; j1<jend; ++j1) {
        for (int i1=ibeg; i1<iend; ++i1) {
          int i2 = io - i1;
          int j2 = jo - j1;
          acc += AC(input1[j1][i1]) * AC(input2[j2][i2]);
        }
      }

      // assign
      out[jo][io] = O/*utType*/(acc);
    }
  }
}

#define VIL1_CONVOLVE_SIMPLE_INSTANTIATE0(I1, I2, AC, O) \
template void vil1_convolve_simple(I1 const * const *, unsigned, unsigned, \
                                   I2 const * const *, unsigned, unsigned, \
                                   AC *,  \
                                   O * const *)

//----------------------------------------------------------------------

#include <vil1/vil1_memory_image_of.h>

template <class I1, class I2, class AC, class O>
void vil1_convolve_simple(vil1_memory_image_of<I1> const &input1,    // input 1
                          int x1, int y1, unsigned w1, unsigned h1,
                          vil1_memory_image_of<I2> const &input2,    // input 2
                          int x2, int y2, unsigned w2, unsigned h2,
                          AC *,
                          vil1_memory_image_of<O>        &output,    // output
                          int xo, int yo)
{
  // assert that the memory to be touched may be touched :
  assert( input1.in_range(x1, y1, w1, h1) );
  assert( input2.in_range(x2, y2, w2, h2) );
  assert( output.in_range(xo, yo, w1+w2-1, h1+h2-1) );

  //
  std::vector<I1 const *> in1(h1);
  for (unsigned k=0; k<h1; ++k)
    in1[k] = input1[y1+k] + x1;
#if TRACE
  std::cerr << in1.size() << " rasters in in1\n";
#endif

  //
  std::vector<I2 const *> in2(h2);
  for (unsigned k=0; k<h2; ++k)
    in2[k] = input2[y2+k] + x2;
#if TRACE
  std::cerr << in2.size() << " rasters in in2\n";
#endif

  //
  std::vector<O *> out(h1+h2-1);
  for (unsigned k=0; k<h1+h2-1; ++k)
    out[k] = output[yo+k] + xo;
#if TRACE
  std::cerr << out.size() << " rasters in out\n";
#endif

  // call the simpler routine (see comment above for explanation of hack).
  static void (*f)(I1 const * const *, unsigned, unsigned,
                   I2 const * const *, unsigned, unsigned,
                   AC *,
                   O        * const *) = nullptr;
  if (!f)
    f = vil1_convolve_simple;
  (*f)(const_cast<I1 const * const *>(/* xxx */&in1[0]), w1, h1,
       const_cast<I2 const * const *>(/* xxx */&in2[0]), w2, h2,
       (AC*)nullptr,
       const_cast<O        * const *>(/* xxx */&out[0]));
}

#define VIL1_CONVOLVE_SIMPLE_INSTANTIATE1(I1, I2, AC, O) \
template void vil1_convolve_simple(vil1_memory_image_of<I1 > const &, \
                                   int, int, unsigned, unsigned,      \
                                   vil1_memory_image_of<I2 > const &, \
                                   int, int, unsigned, unsigned,      \
                                   AC *, \
                                   vil1_memory_image_of<O >        &, \
                                   int, int)

//----------------------------------------------------------------------

//: $\displaystyle {\rm out}_{{\rm off}+k} = \sum_{i+j=k} a_{{\rm off}+i} b_{{\rm off}+j}$
template <class I1, class I2, class AC, class O>
void vil1_convolve_simple(vil1_memory_image_of<I1> const &in1,
                          vil1_memory_image_of<I2> const &in2,
                          AC *,
                          vil1_memory_image_of<O>        &out)
{
  // see comment above for explanation of hack.
  static void (*f)(I1 const * const *, unsigned, unsigned,
                   I2 const * const *, unsigned, unsigned,
                   AC *,
                   O        * const *) = nullptr;
  if (!f)
    f = vil1_convolve_simple;
  (*f)(in1.row_array(), in1.width(), in1.height(),
       in2.row_array(), in2.width(), in2.height(),
       (AC*)nullptr,
       out.row_array());
}

#define VIL1_CONVOLVE_SIMPLE_INSTANTIATE2(I1, I2, AC, O) \
template void vil1_convolve_simple(vil1_memory_image_of<I1 > const &, \
                                   vil1_memory_image_of<I2 > const &, \
                                   AC *, \
                                   vil1_memory_image_of<O >        &)

//--------------------------------------------------------------------------------

//
#define VIL1_CONVOLVE_SIMPLE_INSTANTIATE(I1, I2, AC, O) \
VIL1_CONVOLVE_SIMPLE_INSTANTIATE0(I1, I2, AC, O); \
VIL1_CONVOLVE_SIMPLE_INSTANTIATE1(I1, I2, AC, O); \
VIL1_CONVOLVE_SIMPLE_INSTANTIATE2(I1, I2, AC, O)

#endif // vil1_convolve_simple_hxx_
