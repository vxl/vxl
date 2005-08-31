// This is mul/vil3d/vil3d_transform.h
#ifndef vil3d_transform_h_
#define vil3d_transform_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief STL algorithm like methods.
// \author Tim Cootes, Ian Scott.

#include <vcl_cassert.h>
#include <vcl_algorithm.h>
#include <vil3d/vil3d_image_view.h>

//: Apply a unary operation to each pixel in src to get dest.
// \param functor should take a value of type inP, and return a value of type outP
template <class inP, class outP, class Op >
inline void vil3d_transform(const vil3d_image_view<inP >&src, vil3d_image_view<outP >&dest, Op functor)
{
  const unsigned ni = src.ni(), nj= src.nj(), nk= src.nk(), np = src.nplanes();
  dest.set_size(ni, nj, nk, np);

  if (src.is_contiguous() && dest.is_contiguous())
  {
    vcl_transform(src.begin(), src.end(), dest.begin(), functor);
  }
  else
  {
    for (unsigned p = 0; p < np; ++p)
    for (unsigned k = 0; k < nk; ++k)
      for (unsigned j = 0; j < nj; ++j)
      {
        const inP* src_p = &src(0,j,k,p);
        vcl_ptrdiff_t src_step = src.istep();
        outP* dest_p = &dest(0,j,k,p);
        vcl_ptrdiff_t dest_step = dest.istep();

        for (int i = ni+1; --i != 0; src_p+=src_step, dest_p+=dest_step)
          *dest_p = functor(*src_p);
      }
  }
}

//: Apply a binary function to each pixel in src and dest that modifies dest.
// \param functor should take two parameters (inP src, outP &dest);
template <class inP, class outP, class Op >
inline void vil3d_transform2(const vil3d_image_view<inP >&src, vil3d_image_view<outP >&dest, Op functor)
{
  const unsigned ni = src.ni(), nj= src.nj(), nk= src.nk(), np = src.nplanes();
  dest.set_size(ni, nj, nk, np);

  // Note : Could optimise special cases significantly
  for (unsigned p = 0; p < np; ++p)
   for (unsigned k = 0; k < nk; ++k)
    for (unsigned j = 0; j < nj; ++j)
      for (unsigned i = 0; i < ni; ++i)
         functor(src(i,j,k,p), dest(i,j,k,p));
}

//: Apply a binary operation to each pixel in srcA and srcB to get dest.
template <class inA, class inB, class outP, class BinOp >
inline void vil3d_transform(const vil3d_image_view<inA >&srcA,
                           const vil3d_image_view<inB >&srcB,
                           vil3d_image_view<outP >&dest,
                           BinOp functor)
{
  assert(srcB.ni() == srcA.ni() && srcA.nj() == srcB.nj()
         && srcB.nk() == srcA.nk()
         && srcA.nplanes() == srcB.nplanes());
  dest.set_size(srcA.ni(), srcA.nj(), srcA.nk(), srcA.nplanes());
  for (unsigned p = 0; p < srcA.nplanes(); ++p)
   for (unsigned k = 0; k < srcA.nk(); ++k)
    for (unsigned j = 0; j < srcA.nj(); ++j)
      for (unsigned i = 0; i < srcA.ni(); ++i)
        dest(i,j,k,p) = functor(srcA(i,j,k,p),srcB(i,j,k,p));
}

//: Apply a binary operation to each pixel in srcA and srcB to get dest.
// non-const dest version, assumes dest is already correct size.
template <class inA, class inB, class outP, class BinOp >
inline void vil3d_transform(const vil3d_image_view<inA >&srcA,
                           const vil3d_image_view<inB >&srcB,
                           const vil3d_image_view<outP >&dest,
                           BinOp functor)
{
  assert(dest.ni() == srcA.ni() && srcA.nj() == dest.nj() 
         && srcA.nk() == dest.nk()
         && srcA.nplanes() == dest.nplanes());
  assert(srcB.ni() == srcA.ni() && srcA.nj() == srcB.nj() 
         && srcA.nk() == srcB.nk()
         && srcA.nplanes() == srcB.nplanes());
  vil3d_image_view<outP >& nc_dest = const_cast<vil3d_image_view<outP >&>(dest);
  for (unsigned p = 0; p < srcA.nplanes(); ++p)
   for (unsigned k = 0; k < srcA.nk(); ++k)
    for (unsigned j = 0; j < srcA.nj(); ++j)
      for (unsigned i = 0; i < srcA.ni(); ++i)
        nc_dest(i,j,k,p) = functor(srcA(i,j,k,p),srcB(i,j,k,p));
}


#endif // vil3d_transform_h_

