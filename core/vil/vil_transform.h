// This is mul/vil2/vil2_transform.h
#ifndef vil2_transform_h_
#define vil2_transform_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief STL algorithm like methods.
// \author Ian Scott.

#include <vcl_cassert.h>
#include <vil2/vil2_image_view.h>


//: Apply a unary operation to each pixel in src to get dest.
template <class inP, class outP, class Op >
inline void vil2_transform(const vil2_image_view<inP >&src, vil2_image_view<outP >&dest, Op functor)
{
  dest.set_size(src.ni(), src.nj(), src.nplanes());
  for (unsigned p = 0; p < src.nplanes(); ++p)
    for (unsigned j = 0; j < src.nj(); ++j)
      for (unsigned i = 0; i < src.ni(); ++i)
        dest(i,j,p) = functor(src(i,j,p));
}

//: Apply a binary operation to each pixel in srcA and srcB to get dest.
template <class inA, class inB, class outP, class BinOp >
inline void vil2_transform(const vil2_image_view<inA >&srcA,
                           const vil2_image_view<inB >&srcB,
                           vil2_image_view<outP >&dest,
                           BinOp functor)
{
  assert(srcB.ni() == srcA.ni() && srcA.nj() == srcB.nj()
    && srcA.nplanes() == srcB.nplanes());
  dest.set_size(srcA.ni(), srcA.nj(), srcA.nplanes());
  for (unsigned p = 0; p < srcA.nplanes(); ++p)
    for (unsigned j = 0; j < srcA.nj(); ++j)
      for (unsigned i = 0; i < srcA.ni(); ++i)
        dest(i,j,p) = functor(srcA(i,j,p),srcB(i,j,p));
}

//: Apply a binary operation to each pixel in srcA and srcB to get dest.
// non-const dest version, assumes dest is already correct size.
template <class inA, class inB, class outP, class BinOp >
inline void vil2_transform(const vil2_image_view<inA >&srcA,
                           const vil2_image_view<inB >&srcB,
                           const vil2_image_view<outP >&dest,
                           BinOp functor)
{
  assert(dest.ni() == srcA.ni() && srcA.nj() == dest.nj()
    && srcA.nplanes() == dest.nplanes());
  assert(srcB.ni() == srcA.ni() && srcA.nj() == srcB.nj()
    && srcA.nplanes() == srcB.nplanes());
  vil2_image_view<outP >& nc_dest = const_cast<vil2_image_view<outP >&>(dest);
  for (unsigned p = 0; p < srcA.nplanes(); ++p)
    for (unsigned j = 0; j < srcA.nj(); ++j)
      for (unsigned i = 0; i < srcA.ni(); ++i)
        nc_dest(i,j,p) = functor(srcA(i,j,p),srcB(i,j,p));
}


#endif // vil2_transform_h_
