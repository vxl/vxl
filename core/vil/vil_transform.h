// This is core/vil/vil_transform.h
#ifndef vil_transform_h_
#define vil_transform_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief STL algorithm like methods.
// \author Ian Scott.

#include <vcl_cassert.h>
#include <vil/vil_image_view.h>

//: Apply a unary operation to each pixel in image.
// \param functor should take a value of type T and return same type
template <class T, class F >
inline void vil_transform(vil_image_view<T >& image, F functor)
{
  const unsigned ni = image.ni(), nj= image.nj(), np = image.nplanes();

  vcl_ptrdiff_t istep=image.istep(),jstep=image.jstep(),pstep = image.planestep();
  T* plane = image.top_left_ptr();

  if (istep==1)
  {
    // Optimise special case
    for (unsigned p=0;p<np;++p,plane += pstep)
    {
      T* row = plane;
      for (unsigned j=0;j<nj;++j,row += jstep)
      {
        T *pixel = row, *end_row = row+ni;
        for (;pixel!=end_row;++pixel) *pixel = functor(*pixel);
      }
    }
  }
  else
  if (jstep==1)
  {
    // Optimise special case
    for (unsigned p=0;p<np;++p,plane += pstep)
    {
      T* col = plane;
      for (unsigned i=0;i<ni;++i,col += istep)
      {
        T *pixel = col, *end_col=col+nj;
        for (;pixel!=end_col;++pixel) *pixel = functor(*pixel);
      }
    }
  }
  else
  {
    // General case
    for (unsigned p=0;p<np;++p,plane += pstep)
    {
      T* row = plane;
      for (unsigned j=0;j<nj;++j,row += jstep)
      {
        T* pixel = row;
        for (unsigned i=0;i<ni;++i,pixel+=istep)
          *pixel = functor(*pixel);
      }
    }
  }
}


//: Apply a unary operation to each pixel in src to get dest.
// \param functor should take a value of type inP, and return a value of type outP
template <class inP, class outP, class Op >
inline void vil_transform(const vil_image_view<inP >&src, vil_image_view<outP >&dest, Op functor)
{
  const unsigned ni = src.ni(), nj= src.nj(), np = src.nplanes();

  dest.set_size(ni, nj, np);

  // Optimise special case;
  if (dest.istep()==1 && src.istep()==1)
  {
    for (unsigned p = 0; p < np; ++p)
      for (unsigned j = 0; j < nj; ++j)
      {
        const inP* src_row = &src(0,j,p);
        outP* dest_row = &dest(0,j,p);
        for (unsigned i = 0; i < ni; ++i)
          dest_row[i] = functor(src_row[i]);
      }
    return;
  }

  for (unsigned p = 0; p < np; ++p)
    for (unsigned j = 0; j < nj; ++j)
      for (unsigned i = 0; i < ni; ++i)
        dest(i,j,p) = functor(src(i,j,p));
}

//: Apply a binary function to each pixel in src and dest that modifies dest.
// \param functor should take two parameters (inP src, outP &dest);
template <class inP, class outP, class Op >
inline void vil_transform2(const vil_image_view<inP >&src, vil_image_view<outP >&dest, Op functor)
{
  const unsigned ni = src.ni(), nj= src.nj(), np = src.nplanes();

  dest.set_size(ni, nj, np);

  // Optimise special case;
  if (dest.istep()==1 && src.istep()==1)
  {
    for (unsigned p = 0; p < np; ++p)
      for (unsigned j = 0; j < nj; ++j)
      {
        const inP* src_row = &src(0,j,p);
        outP* dest_row = &dest(0,j,p);
        for (unsigned i = 0; i < ni; ++i)
          functor(src_row[i],dest_row[i]);
      }
    return;
  }

  for (unsigned p = 0; p < np; ++p)
    for (unsigned j = 0; j < nj; ++j)
      for (unsigned i = 0; i < ni; ++i)
         functor(src(i,j,p), dest(i,j,p));
}


//: Apply a binary operation to each pixel in srcA and srcB to get dest.
template <class inA, class inB, class outP, class BinOp >
inline void vil_transform(const vil_image_view<inA >&srcA,
                          const vil_image_view<inB >&srcB,
                          vil_image_view<outP >&dest,
                          BinOp functor)
{
  assert(srcB.ni() == srcA.ni() && srcA.nj() == srcB.nj() && srcA.nplanes() == srcB.nplanes());
  dest.set_size(srcA.ni(), srcA.nj(), srcA.nplanes());
  for (unsigned p = 0; p < srcA.nplanes(); ++p)
    for (unsigned j = 0; j < srcA.nj(); ++j)
      for (unsigned i = 0; i < srcA.ni(); ++i)
        dest(i,j,p) = functor(srcA(i,j,p),srcB(i,j,p));
}

//: Apply a binary operation to each pixel in srcA and srcB to get dest.
// non-const dest version, assumes dest is already correct size.
template <class inA, class inB, class outP, class BinOp >
inline void vil_transform(const vil_image_view<inA >&srcA,
                          const vil_image_view<inB >&srcB,
                          const vil_image_view<outP >&dest,
                          BinOp functor)
{
  assert(dest.ni() == srcA.ni() && srcA.nj() == dest.nj() && srcA.nplanes() == dest.nplanes());
  assert(srcB.ni() == srcA.ni() && srcA.nj() == srcB.nj() && srcA.nplanes() == srcB.nplanes());
  vil_image_view<outP >& nc_dest = const_cast<vil_image_view<outP >&>(dest);
  for (unsigned p = 0; p < srcA.nplanes(); ++p)
    for (unsigned j = 0; j < srcA.nj(); ++j)
      for (unsigned i = 0; i < srcA.ni(); ++i)
        nc_dest(i,j,p) = functor(srcA(i,j,p),srcB(i,j,p));
}


#endif // vil_transform_h_
