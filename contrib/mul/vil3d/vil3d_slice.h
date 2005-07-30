// This is mul/vil3d/vil3d_slice.h
#ifndef vil3d_slice_h_
#define vil3d_slice_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author Tim Cootes.

#include <vil/vil_image_view.h>
#include <vil3d/vil3d_image_view.h>
#include <vcl_cassert.h>

//: Return a 2D view of slice k of 3D image aligned as (j,i).
//  result(x,y,p)=im(y,x,k,p)
// \relates vil3d_image_view
// \relates vil_image_view
template <class T>
inline vil_image_view<T> vil3d_slice_ji(const vil3d_image_view<T> &im, unsigned k)
{
  assert(k<im.nk());
  // Tweak ensure contiguous check works for 1 plane images
  vcl_ptrdiff_t pstep = im.planestep();
  if (im.nplanes()==1) pstep=im.ni()*im.nj();
  return vil_image_view<T>(im.memory_chunk(),
                            im.origin_ptr()+k*im.kstep(),
                            im.nj(), im.ni(), im.nplanes(),
                            im.jstep(), im.istep(), pstep);
}

//: Return a 2D view of slice k of 3D image aligned as (i,j).
//  result(x,y,p)=im(x,y,k,p)
// \relates vil3d_image_view
// \relates vil_image_view
template <class T>
inline vil_image_view<T> vil3d_slice_ij(const vil3d_image_view<T> &im, unsigned k)
{
  assert(k<im.nk());
  // Tweak ensure contiguous check works for 1 plane images
  vcl_ptrdiff_t pstep = im.planestep();
  if (im.nplanes()==1) pstep=im.ni()*im.nj();
  return vil_image_view<T>(im.memory_chunk(),
                            im.origin_ptr()+k*im.kstep(),
                            im.ni(), im.nj(), im.nplanes(),
                            im.istep(), im.jstep(), pstep);
}

//: Return a 2D view of slice i of 3D image aligned as (j,k).
//  result(x,y,p)=im(i,x,y,p)
// \relates vil3d_image_view
// \relates vil_image_view
template <class T>
inline vil_image_view<T> vil3d_slice_jk(const vil3d_image_view<T> &im, unsigned i)
{
  assert(i<im.ni());
  // Tweak ensure contiguous check works for 1 plane images
  vcl_ptrdiff_t pstep = im.planestep();
  if (im.nplanes()==1) pstep=im.nj()*im.nk();
  return vil_image_view<T>(im.memory_chunk(),
                            im.origin_ptr()+i*im.istep(),
                            im.nj(), im.nk(), im.nplanes(),
                            im.jstep(), im.kstep(), pstep);
}

//: Return a 2D view of slice i of 3D image aligned as (k,j).
//  result(x,y,p)=im(i,y,x,p)
// \relates vil3d_image_view
// \relates vil_image_view
template <class T>
inline vil_image_view<T> vil3d_slice_kj(const vil3d_image_view<T> &im, unsigned i)
{
  assert(i<im.ni());
  // Tweak ensure contiguous check works for 1 plane images
  vcl_ptrdiff_t pstep = im.planestep();
  if (im.nplanes()==1) pstep=im.nj()*im.nk();
  return vil_image_view<T>(im.memory_chunk(),
                            im.origin_ptr()+i*im.istep(),
                            im.nk(), im.nj(), im.nplanes(),
                            im.kstep(), im.jstep(), pstep);
}

//: Return a 2D view of slice j of 3D image aligned as (k,i).
//  result(x,y,p)=im(y,i,x,p)
// \relates vil3d_image_view
// \relates vil_image_view
template <class T>
inline vil_image_view<T> vil3d_slice_ki(const vil3d_image_view<T> &im, unsigned j)
{
  assert(j<im.nj());
  // Tweak ensure contiguous check works for 1 plane images
  vcl_ptrdiff_t pstep = im.planestep();
  if (im.nplanes()==1) pstep=im.ni()*im.nk();
  return vil_image_view<T>(im.memory_chunk(),
                            im.origin_ptr()+j*im.jstep(),
                            im.nk(), im.ni(), im.nplanes(),
                            im.kstep(), im.istep(), pstep);
}

//: Return a 2D view of slice j of 3D image aligned as (i,k).
//  result(x,y,p)=im(x,i,y,p)
// \relates vil3d_image_view
// \relates vil_image_view
template <class T>
inline vil_image_view<T> vil3d_slice_ik(const vil3d_image_view<T> &im, unsigned j)
{
  assert(j<im.nj());
  // Tweak ensure contiguous check works for 1 plane images
  vcl_ptrdiff_t pstep = im.planestep();
  if (im.nplanes()==1) pstep=im.ni()*im.nk();
  return vil_image_view<T>(im.memory_chunk(),
                            im.origin_ptr()+j*im.jstep(),
                            im.ni(), im.nk(), im.nplanes(),
                            im.istep(), im.kstep(), pstep);
}

//: Define format of a slice from a 3D volume
enum vil3d_slice_format
{
  VIL3D_SLICE_FORMAT_IJ=0,
  VIL3D_SLICE_FORMAT_JI=1,
  VIL3D_SLICE_FORMAT_IK=2,
  VIL3D_SLICE_FORMAT_KI=3,
  VIL3D_SLICE_FORMAT_JK=4,
  VIL3D_SLICE_FORMAT_KJ=5
};

//: Return a 2D view of slice of a 3D image aligned as defined by slice_format.
//  Thus if slice_format==VIL3D_SLICE_FORMAT_IJ then i-j slice returned at z=slice_index.
// \relates vil3d_image_view
// \relates vil_image_view
template <class T>
inline vil_image_view<T> vil3d_slice(const vil3d_image_view<T> &im, unsigned slice_index,
                                     vil3d_slice_format slice_format)
{
  switch (slice_format)
  {
    case VIL3D_SLICE_FORMAT_IJ: return vil3d_slice_ij(im,slice_index);
    case VIL3D_SLICE_FORMAT_JI: return vil3d_slice_ji(im,slice_index);
    case VIL3D_SLICE_FORMAT_IK: return vil3d_slice_ik(im,slice_index);
    case VIL3D_SLICE_FORMAT_KI: return vil3d_slice_kj(im,slice_index);
    case VIL3D_SLICE_FORMAT_JK: return vil3d_slice_jk(im,slice_index);
    case VIL3D_SLICE_FORMAT_KJ: return vil3d_slice_kj(im,slice_index);
  }
  return 0.0; // to avoid compiler warning
}

//: Return number of possible slices of image given the selected format
//  Thus if slice_format==VIL3D_SLICE_FORMAT_IJ then return image.nk().
inline unsigned vil3d_n_slices(const vil3d_image_view_base& image,
                               vil3d_slice_format slice_format)
{
  switch (slice_format)
  {
    case VIL3D_SLICE_FORMAT_IJ: return image.nk();
    case VIL3D_SLICE_FORMAT_JI: return image.nk();
    case VIL3D_SLICE_FORMAT_IK: return image.nj();
    case VIL3D_SLICE_FORMAT_KI: return image.nj();
    case VIL3D_SLICE_FORMAT_JK: return image.ni();
    case VIL3D_SLICE_FORMAT_KJ: return image.ni();
  }
  return 0; // to avoid compiler warning
}

//: Return pixel width in i direction of slice, given widths of original 3D volume pixels
//  Thus if slice_format==VIL3D_SLICE_FORMAT_JK then return src_j_width.
inline double vil3d_slice_pixel_width_i(double src_i_width, double src_j_width, double src_k_width,
                               vil3d_slice_format slice_format)
{
  switch (slice_format)
  {
    case VIL3D_SLICE_FORMAT_IJ: return src_i_width;
    case VIL3D_SLICE_FORMAT_JI: return src_j_width;
    case VIL3D_SLICE_FORMAT_IK: return src_i_width;
    case VIL3D_SLICE_FORMAT_KI: return src_k_width;
    case VIL3D_SLICE_FORMAT_JK: return src_j_width;
    case VIL3D_SLICE_FORMAT_KJ: return src_k_width;
  }
  return 0.0; // to avoid compiler warning
}

//: Return pixel width in j direction of slice, given widths of original 3D volume pixels
//  Thus if slice_format==VIL3D_SLICE_FORMAT_JK then return src_k_width.
inline double vil3d_slice_pixel_width_j(double src_i_width, double src_j_width, double src_k_width,
                               vil3d_slice_format slice_format)
{
  switch (slice_format)
  {
    case VIL3D_SLICE_FORMAT_IJ: return src_j_width;
    case VIL3D_SLICE_FORMAT_JI: return src_i_width;
    case VIL3D_SLICE_FORMAT_IK: return src_k_width;
    case VIL3D_SLICE_FORMAT_KI: return src_i_width;
    case VIL3D_SLICE_FORMAT_JK: return src_k_width;
    case VIL3D_SLICE_FORMAT_KJ: return src_j_width;
  }
  return 0.0; // to avoid compiler warning
}

//: Return separation of neighbouring slices, given widths of original 3D volume pixels
//  Thus if slice_format==VIL3D_SLICE_FORMAT_JK then return src_i_width.
inline double vil3d_slice_separation(double src_i_width, double src_j_width, double src_k_width,
                               vil3d_slice_format slice_format)
{
  switch (slice_format)
  {
    case VIL3D_SLICE_FORMAT_IJ: return src_k_width;
    case VIL3D_SLICE_FORMAT_JI: return src_k_width;
    case VIL3D_SLICE_FORMAT_IK: return src_j_width;
    case VIL3D_SLICE_FORMAT_KI: return src_j_width;
    case VIL3D_SLICE_FORMAT_JK: return src_i_width;
    case VIL3D_SLICE_FORMAT_KJ: return src_i_width;
  }
  return 0.0; // to avoid compiler warning
}


#endif // vil3d_slice_h_
