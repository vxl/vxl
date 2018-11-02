#include <iostream>
#include "vil_io_image_view_base.h"
//:
// \file
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vsl/vsl_binary_io.h>
#include <vil/vil_image_view_base.h>
#include <vil/vil_image_view.h>
#include <vil/io/vil_io_image_view.h>

//: Binary write image view base to stream
template <>
void vsl_b_write(vsl_b_ostream & os, vil_image_view_base_sptr const& view_base)
{
  switch (view_base->pixel_format())
  {
#if VXL_HAS_INT_64
   case VIL_PIXEL_FORMAT_UINT_64: {
     vil_image_view<vxl_uint_64> v(view_base);
     vsl_b_write(os, v);
     break; }
   case VIL_PIXEL_FORMAT_INT_64: {
     vil_image_view<vxl_int_64> v(view_base);
     vsl_b_write(os, v);
     break; }
#endif
   case VIL_PIXEL_FORMAT_UINT_32: {
     vil_image_view<vxl_uint_32> v(view_base);
     vsl_b_write(os, v);
     break; }
   case VIL_PIXEL_FORMAT_INT_32: {
     vil_image_view<vxl_int_32> v(view_base);
     vsl_b_write(os, v);
     break; }
   case VIL_PIXEL_FORMAT_UINT_16: {
     vil_image_view<vxl_uint_16> v(view_base);
     vsl_b_write(os, v);
     break; }
   case VIL_PIXEL_FORMAT_INT_16: {
     vil_image_view<vxl_int_16> v(view_base);
     vsl_b_write(os, v);
     break; }
   case VIL_PIXEL_FORMAT_BYTE: {
     vil_image_view<vxl_byte> v(view_base);
     vsl_b_write(os, v);
     break; }
   case VIL_PIXEL_FORMAT_SBYTE: {
     vil_image_view<vxl_sbyte> v(view_base);
     vsl_b_write(os, v);
     break; }
   case VIL_PIXEL_FORMAT_FLOAT: {
     vil_image_view<float> v(view_base);
     vsl_b_write(os, v);
     break; }
   case VIL_PIXEL_FORMAT_DOUBLE: {
     vil_image_view<double> v(view_base);
     vsl_b_write(os, v);
     break; }
   case VIL_PIXEL_FORMAT_BOOL: {
     vil_image_view<bool> v(view_base);
     vsl_b_write(os, v);
     break; }
   // No version 1 complex images were ever written. Now added. It should work. Fix if necessary
   case VIL_PIXEL_FORMAT_COMPLEX_FLOAT: {
    vil_image_view<std::complex<float> > v(view_base);
    vsl_b_write(os, v);
    break; }
   case VIL_PIXEL_FORMAT_COMPLEX_DOUBLE: {
    vil_image_view<std::complex<double> > v(view_base);
    vsl_b_write(os, v);
    break; }
   default: {
      std::cerr << "I/O ERROR: vsl_b_write(vsl_b_ostream &, vil_image_view_base_sptr const&)\n"
               << "           Unknown pixel format "<< view_base->pixel_format() << '\n';
      os.os().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
      return; }
  }
  //std::cerr << "warning: vsl_b_write not implemented for vil_image_view_base_sptr\n";
}

//: Binary load image view base from stream.
template <>
void vsl_b_read(vsl_b_istream& is, vil_image_view_base_sptr &view_base)
{
  if (!is) return;

  unsigned ni,nj,np;
  std::ptrdiff_t istep,jstep,pstep;
  vil_memory_chunk_sptr chunk;
  std::ptrdiff_t offset;

  short w;
  vsl_b_read(is, w);
  if (w != 1) {
    std::cerr << "warning: vsl_b_read not implemented for vil_image_view binary io version: " << w <<  '\n';
    return;
  }

  vsl_b_read(is, ni);
  vsl_b_read(is, nj);
  vsl_b_read(is, np);
  vsl_b_read(is, istep);
  vsl_b_read(is, jstep);
  vsl_b_read(is, pstep);
  if (ni*nj*np==0) {
    std::cerr << "warning: vsl_b_read image ni*nj*np = 0\n";
    //image.set_size(0,0,0);
  }
  else {
    vsl_b_read(is, chunk);
    vsl_b_read(is, offset);

    switch (chunk->pixel_format()) {
#if VXL_HAS_INT_64
     case VIL_PIXEL_FORMAT_UINT_64: {
       const auto* data = reinterpret_cast<const vxl_uint_64*>(chunk->data());
       view_base = new vil_image_view<vxl_uint_64>(chunk,data+offset,ni,nj,np,istep,jstep,pstep);
       break; }
     case VIL_PIXEL_FORMAT_INT_64: {
       const auto* data = reinterpret_cast<const vxl_int_64*>(chunk->data());
       view_base = new vil_image_view<vxl_int_64>(chunk,data+offset,ni,nj,np,istep,jstep,pstep);
       break; }
#endif
     case VIL_PIXEL_FORMAT_UINT_32: {
       const auto* data = reinterpret_cast<const vxl_uint_32*>(chunk->data());
       view_base = new vil_image_view<vxl_uint_32>(chunk,data+offset,ni,nj,np,istep,jstep,pstep);
       break; }
   case VIL_PIXEL_FORMAT_INT_32: {
       const auto* data = reinterpret_cast<const vxl_int_32*>(chunk->data());
       view_base = new vil_image_view<vxl_int_32>(chunk,data+offset,ni,nj,np,istep,jstep,pstep);
       break; }
   case VIL_PIXEL_FORMAT_UINT_16: {
       const auto* data = reinterpret_cast<const vxl_uint_16*>(chunk->data());
       view_base = new vil_image_view<vxl_uint_16>(chunk,data+offset,ni,nj,np,istep,jstep,pstep);
       break; }
   case VIL_PIXEL_FORMAT_INT_16: {
       const auto* data = reinterpret_cast<const vxl_int_16*>(chunk->data());
       view_base = new vil_image_view<vxl_int_16>(chunk,data+offset,ni,nj,np,istep,jstep,pstep);
       break; }
   case VIL_PIXEL_FORMAT_BYTE: {
       const auto* data = reinterpret_cast<const vxl_byte*>(chunk->data());
       view_base = new vil_image_view<vxl_byte>(chunk,data+offset,ni,nj,np,istep,jstep,pstep);
       break; }
   case VIL_PIXEL_FORMAT_SBYTE: {
       const auto* data = reinterpret_cast<const vxl_sbyte*>(chunk->data());
       view_base = new vil_image_view<vxl_sbyte>(chunk,data+offset,ni,nj,np,istep,jstep,pstep);
       break; }
   case VIL_PIXEL_FORMAT_FLOAT: {
       const auto* data = reinterpret_cast<const float*>(chunk->data());
       view_base = new vil_image_view<float>(chunk,data+offset,ni,nj,np,istep,jstep,pstep);
       break; }
   case VIL_PIXEL_FORMAT_DOUBLE: {
       const auto* data = reinterpret_cast<const double*>(chunk->data());
       view_base = new vil_image_view<double>(chunk,data+offset,ni,nj,np,istep,jstep,pstep);
       break; }
   case VIL_PIXEL_FORMAT_BOOL: {
       const bool* data = reinterpret_cast<const bool*>(chunk->data());
       view_base = new vil_image_view<bool>(chunk,data+offset,ni,nj,np,istep,jstep,pstep);
       break; }
    // No version 1 complex images were ever written. Now added. It should work. Fix if necessary
   case VIL_PIXEL_FORMAT_COMPLEX_FLOAT: {
       const auto* data = reinterpret_cast<const std::complex<float>*>(chunk->data());
       view_base = new vil_image_view<std::complex<float> >(chunk,data+offset,ni,nj,np,istep,jstep,pstep);
       break; }
   case VIL_PIXEL_FORMAT_COMPLEX_DOUBLE: {
       const auto* data = reinterpret_cast<const std::complex<double>*>(chunk->data());
       view_base = new vil_image_view<std::complex<double> >(chunk,data+offset,ni,nj,np,istep,jstep,pstep);
       break; }
   default: {
    std::cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, vil_image_view<T>&)\n"
             << "           Unknown version number "<< w << '\n';
    is.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
    return; }
    }
  }
//std::cerr << "warning: vsl_b_read not implemented for vil_image_view_base_sptr\n";
}

template <>
void vsl_b_write(vsl_b_ostream &  /*os*/, vil_image_resource_sptr const&  /*view*/)
{
  std::cerr << "warning: vsl_b_write not implemented for vil_image_resource_sptr\n";
}

template <>
void vsl_b_read(vsl_b_istream &  /*is*/, vil_image_resource_sptr & /*view*/)
{
  std::cerr << "warning: vsl_b_read not implemented for vil_image_resource_sptr\n";
}

#if 0
//: Binary write voxel world to stream
void vsl_b_write(vsl_b_ostream & , vil_image_view_base const&)
{
  std::cerr << "warning: vsl_b_write not implemented for vil_image_view_base\n";
}

//: Binary load voxel world from stream.
void vsl_b_read(vsl_b_istream &, vil_image_view_base &)
{
  std::cerr << "warning: vsl_b_read not implemented for vil_image_view_base\n";
}
#endif // 0
