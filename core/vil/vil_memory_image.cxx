// This is mul/vil2/vil2_memory_image.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author Ian Scott

#include "vil2_memory_image.h"
#include <vcl_cassert.h>
#include <vcl_cstdlib.h>
#include <vil2/vil2_byte.h>
#include <vil2/vil2_image_view.h>
#include <vil2/vil2_image_view_functions.h>
#include <vil2/vil2_pixel_format.h>

class vil2_image_view_base;

vil2_memory_image::vil2_memory_image():
   view_(new vil2_image_view<vil2_byte>()) {}

vil2_memory_image::vil2_memory_image(unsigned ni,
  unsigned nj, unsigned nplanes, vil2_pixel_format format)
{
  // format should be a scalar type
  assert (vil2_pixel_format_num_components(format)==1);

  switch (format)
  {
  case VIL2_PIXEL_FORMAT_BYTE:
    view_ = new vil2_image_view<vil2_byte>(ni, nj, nplanes);
    break;
  case VIL2_PIXEL_FORMAT_INT_8:
    view_ = new vil2_image_view<vxl_int_8>(ni, nj, nplanes);
    break;
  case VIL2_PIXEL_FORMAT_UINT_32:
    view_ = new vil2_image_view<vxl_uint_32>(ni, nj, nplanes);
    break;
  case VIL2_PIXEL_FORMAT_UINT_16:
    view_ = new vil2_image_view<vxl_uint_16>(ni, nj, nplanes);
    break;
  case VIL2_PIXEL_FORMAT_INT_32:
    view_ = new vil2_image_view<vxl_int_32>(ni, nj, nplanes);
    break;
  case VIL2_PIXEL_FORMAT_INT_16:
    view_ = new vil2_image_view<vxl_int_16>(ni, nj, nplanes);
    break;
  case VIL2_PIXEL_FORMAT_BOOL:
    view_ = new vil2_image_view<bool>(ni, nj, nplanes);
    break;
  case VIL2_PIXEL_FORMAT_FLOAT:
    view_ = new vil2_image_view<float>(ni, nj, nplanes);
    break;
  case VIL2_PIXEL_FORMAT_DOUBLE:
    view_ = new vil2_image_view<double>(ni, nj, nplanes);
    break;
  default:
    vcl_cerr << "ERROR: vil2_memory_image::vil2_memory_image\n"
                "\t unknown format " << format << vcl_endl;
    vcl_abort();
  }
}


//: Create a read/write view of a copy of this data.
// Currently not yet implemented.
// \return 0 if unable to get view of correct size.
vil2_image_view_base_sptr vil2_memory_image::get_copy_view(unsigned i0, unsigned ni,
                                                           unsigned j0, unsigned nj) const
{
  switch(view_->pixel_format())
  {
  case  VIL2_PIXEL_FORMAT_BYTE:
    {
      const vil2_image_view<vil2_byte> &v =
        static_cast<const vil2_image_view<vil2_byte> &>(*view_);
      vil2_image_view<vil2_byte> w(v.memory_chunk(), &v(i0,j0),
                                   ni, nj, v.nplanes(),
                                   v.istep(), v.jstep(), v.planestep());
      return new vil2_image_view<vil2_byte>(vil2_deep_copy(w));
    }
  case  VIL2_PIXEL_FORMAT_INT_8:
    {
      const vil2_image_view<vxl_int_8> &v =
        static_cast<const vil2_image_view<vxl_int_8> &>(*view_);
      vil2_image_view<vxl_int_8> w(v.memory_chunk(), &v(i0,j0),
                                   ni, nj, v.nplanes(),
                                   v.istep(), v.jstep(), v.planestep());
      return new vil2_image_view<vxl_int_8>(vil2_deep_copy(w));
    }
  case  VIL2_PIXEL_FORMAT_UINT_16:
    {
      const vil2_image_view<vxl_uint_16> &v =
        static_cast<const vil2_image_view<vxl_uint_16> &>(*view_);
      vil2_image_view<vxl_uint_16> w(v.memory_chunk(), &v(i0,j0),
                                     ni, nj, v.nplanes(),
                                     v.istep(), v.jstep(), v.planestep());
      return new vil2_image_view<vxl_uint_16>(vil2_deep_copy(w));
    }
  case  VIL2_PIXEL_FORMAT_INT_16:
    {
      const vil2_image_view<vxl_int_16> &v =
        static_cast<const vil2_image_view<vxl_int_16> &>(*view_);
      vil2_image_view<vxl_int_16> w(v.memory_chunk(), &v(i0,j0),
                                    ni, nj, v.nplanes(),
                                    v.istep(), v.jstep(), v.planestep());
      return new vil2_image_view<vxl_int_16>(vil2_deep_copy(w));
    }
  case  VIL2_PIXEL_FORMAT_UINT_32:
    {
      const vil2_image_view<vxl_uint_32> &v =
        static_cast<const vil2_image_view<vxl_uint_32> &>(*view_);
      vil2_image_view<vxl_uint_32> w(v.memory_chunk(), &v(i0,j0),
                                     ni, nj, v.nplanes(),
                                     v.istep(), v.jstep(), v.planestep());
      return new vil2_image_view<vxl_uint_32>(vil2_deep_copy(w));
    }
  case  VIL2_PIXEL_FORMAT_INT_32:
    {
      const vil2_image_view<vxl_int_32> &v =
        static_cast<const vil2_image_view<vxl_int_32> &>(*view_);
      vil2_image_view<vxl_int_32> w(v.memory_chunk(), &v(i0,j0),
                                    ni, nj, v.nplanes(),
                                    v.istep(), v.jstep(), v.planestep());
      return new vil2_image_view<vxl_int_32>(vil2_deep_copy(w));
    }
  case  VIL2_PIXEL_FORMAT_FLOAT:
    {
      const vil2_image_view<float> &v =
        static_cast<const vil2_image_view<float> &>(*view_);
      vil2_image_view<float> w(v.memory_chunk(), &v(i0,j0),
                               ni, nj, v.nplanes(),
                               v.istep(), v.jstep(), v.planestep());
      return new vil2_image_view<float>(vil2_deep_copy(w));
    }
  case  VIL2_PIXEL_FORMAT_DOUBLE:
    {
      const vil2_image_view<double> &v =
        static_cast<const vil2_image_view<double> &>(*view_);
      vil2_image_view<double> w(v.memory_chunk(), &v(i0,j0),
                                ni, nj, v.nplanes(),
                                v.istep(), v.jstep(), v.planestep());
      return new vil2_image_view<double>(vil2_deep_copy(w));
    }
  case  VIL2_PIXEL_FORMAT_BOOL:
    {
      const vil2_image_view<bool> &v =
        static_cast<const vil2_image_view<bool> &>(*view_);
      vil2_image_view<bool> w(v.memory_chunk(), &v(i0,j0),
                              ni, nj, v.nplanes(),
                              v.istep(), v.jstep(), v.planestep());
      return new vil2_image_view<bool>(vil2_deep_copy(w));
    }
  default:
    return 0;
  }
}

//: Create a read/write view of a copy of this data.
// \return 0 if unable to get view of correct size.
vil2_image_view_base_sptr vil2_memory_image::get_view(unsigned i0, unsigned ni,
                                                      unsigned j0, unsigned nj) const
{
  switch(view_->pixel_format())
  {
  case  VIL2_PIXEL_FORMAT_BYTE:
    {
      const vil2_image_view<vil2_byte> &v =
        static_cast<const vil2_image_view<vil2_byte> &>(*view_);
      return new vil2_image_view<vil2_byte>(v.memory_chunk(), &v(i0,j0),
                                            ni, nj, v.nplanes(),
                                            v.istep(), v.jstep(), v.planestep());
    }
  case  VIL2_PIXEL_FORMAT_INT_8:
    {
      const vil2_image_view<vxl_int_8> &v =
        static_cast<const vil2_image_view<vxl_int_8> &>(*view_);
      return new vil2_image_view<vxl_int_8>(v.memory_chunk(), &v(i0,j0),
                                            ni, nj, v.nplanes(),
                                            v.istep(), v.jstep(), v.planestep());
    }
  case  VIL2_PIXEL_FORMAT_BOOL:
    {
      const vil2_image_view<bool> &v =
        static_cast<const vil2_image_view<bool> &>(*view_);
      return new vil2_image_view<bool>(v.memory_chunk(), &v(i0,j0),
                                       ni, nj, v.nplanes(),
                                       v.istep(), v.jstep(), v.planestep());
    }
  case  VIL2_PIXEL_FORMAT_UINT_32:
    {
      const vil2_image_view<vxl_uint_32> &v =
        static_cast<const vil2_image_view<vxl_uint_32> &>(*view_);
      return new vil2_image_view<vxl_uint_32>(v.memory_chunk(), &v(i0,j0),
                                             ni, nj, v.nplanes(),
                                             v.istep(), v.jstep(), v.planestep());
    }
  case  VIL2_PIXEL_FORMAT_UINT_16:
    {
      const vil2_image_view<vxl_uint_16> &v =
        static_cast<const vil2_image_view<vxl_uint_16> &>(*view_);
      return new vil2_image_view<vxl_uint_16>(v.memory_chunk(), &v(i0,j0),
                                              ni, nj, v.nplanes(),
                                              v.istep(), v.jstep(), v.planestep());
    }
  case  VIL2_PIXEL_FORMAT_INT_32:
    {
      const vil2_image_view<vxl_int_32> &v =
        static_cast<const vil2_image_view<vxl_int_32> &>(*view_);
      return new vil2_image_view<vxl_int_32>(v.memory_chunk(), &v(i0,j0),
                                             ni, nj, v.nplanes(),
                                             v.istep(), v.jstep(), v.planestep());
    }
  case  VIL2_PIXEL_FORMAT_INT_16:
    {
      const vil2_image_view<vxl_int_16> &v =
        static_cast<const vil2_image_view<vxl_int_16> &>(*view_);
      return new vil2_image_view<vxl_int_16>(v.memory_chunk(), &v(i0,j0),
                                             ni, nj, v.nplanes(),
                                             v.istep(), v.jstep(), v.planestep());
    }
  case  VIL2_PIXEL_FORMAT_FLOAT:
    {
      const vil2_image_view<float> &v =
        static_cast<const vil2_image_view<float> &>(*view_);
      return new vil2_image_view<float>(v.memory_chunk(), &v(i0,j0),
                                        ni, nj, v.nplanes(),
                                        v.istep(), v.jstep(), v.planestep());
    }
  case  VIL2_PIXEL_FORMAT_DOUBLE:
    {
      const vil2_image_view<double> &v =
        static_cast<const vil2_image_view<double> &>(*view_);
      return new vil2_image_view<double>(v.memory_chunk(), &v(i0,j0),
                                         ni, nj, v.nplanes(),
                                         v.istep(), v.jstep(), v.planestep());
    }
  default:
    return 0;
  }
}


//: Put the data in this view back into the image source.
// Currently not yet implemented.
// \return true on success.
bool vil2_memory_image::put_view(const vil2_image_view_base& im,unsigned i0, unsigned j0)
{
  if (view_-> pixel_format() != im.pixel_format()) return false;
  if (!view_fits(im, i0, j0)) return false;

  switch(view_->pixel_format())
  {
  case  VIL2_PIXEL_FORMAT_BYTE:
    {
      vil2_image_view<vil2_byte> &v =
        static_cast<vil2_image_view<vil2_byte> &>(*view_);
      const vil2_image_view<vil2_byte> &w =
        static_cast<const vil2_image_view<vil2_byte> &>(im);
      if (v.memory_chunk() == w.memory_chunk())
      {
        if (&v(i0,j0) != w.top_left_ptr())
          assert("ERROR: vil2_memory_image::put_view()\n"
                 "different window from that used in get_view()");
        else return true; // The user has already modified the data in place.
      }
      vil2_copy_to_window(w, v, i0, j0);
      return true;
    }
  case  VIL2_PIXEL_FORMAT_INT_8:
    {
      vil2_image_view<vil2_byte> &v =
        static_cast<vil2_image_view<vil2_byte> &>(*view_);
      const vil2_image_view<vil2_byte> &w =
        static_cast<const vil2_image_view<vil2_byte> &>(im);
      if (v.memory_chunk() == w.memory_chunk())
      {
        if (&v(i0,j0) != w.top_left_ptr())
          assert("ERROR: vil2_memory_image::put_view()\n"
                 "different window from that used in get_view()");
        else return true; // The user has already modified the data in place.
      }
      vil2_copy_to_window(w, v, i0, j0);
      return true;
    }
  case  VIL2_PIXEL_FORMAT_BOOL:
    {
      vil2_image_view<bool> &v =
        static_cast<vil2_image_view<bool> &>(*view_);
      const vil2_image_view<bool> &w =
        static_cast<const vil2_image_view<bool> &>(im);
      if (v.memory_chunk() == w.memory_chunk())
      {
        if (&v(i0,j0) != w.top_left_ptr())
          assert("ERROR: vil2_memory_image::put_view()\n"
                 "different window from that used in get_view()");
        else return true; // The user has already modified the data in place.
      }
      vil2_copy_to_window(w, v, i0, j0);
      return true;
    }
  case  VIL2_PIXEL_FORMAT_UINT_32:
    {
      vil2_image_view<vxl_uint_32> &v =
        static_cast<vil2_image_view<vxl_uint_32> &>(*view_);
      const vil2_image_view<vxl_uint_32> &w =
        static_cast<const vil2_image_view<vxl_uint_32> &>(im);
      if (v.memory_chunk() == w.memory_chunk())
      {
        if (&v(i0,j0) != w.top_left_ptr())
          assert("ERROR: vil2_memory_image::put_view()\n"
                 "different window from that used in get_view()");
        else return true; // The user has already modified the data in place.
      }
      vil2_copy_to_window(w, v, i0, j0);
      return true;
    }
  case  VIL2_PIXEL_FORMAT_UINT_16:
    {
      vil2_image_view<vxl_uint_16> &v =
        static_cast<vil2_image_view<vxl_uint_16> &>(*view_);
      const vil2_image_view<vxl_uint_16> &w =
        static_cast<const vil2_image_view<vxl_uint_16> &>(im);
      if (v.memory_chunk() == w.memory_chunk())
      {
        if (&v(i0,j0) != w.top_left_ptr())
          assert("ERROR: vil2_memory_image::put_view()\n"
                 "different window from that used in get_view()");
        else return true; // The user has already modified the data in place.
      }
      vil2_copy_to_window(w, v, i0, j0);
      return true;
    }
  case  VIL2_PIXEL_FORMAT_INT_32:
    {
      vil2_image_view<vxl_int_32> &v =
        static_cast<vil2_image_view<vxl_int_32> &>(*view_);
      const vil2_image_view<vxl_int_32> &w =
        static_cast<const vil2_image_view<vxl_int_32> &>(im);
      if (v.memory_chunk() == w.memory_chunk())
      {
        if (&v(i0,j0) != w.top_left_ptr())
          assert("ERROR: vil2_memory_image::put_view()\n"
                 "different window from that used in get_view()");
        else return true; // The user has already modified the data in place.
      }
      vil2_copy_to_window(w, v, i0, j0);
      return true;
    }
  case  VIL2_PIXEL_FORMAT_INT_16:
    {
      vil2_image_view<vxl_int_16> &v =
        static_cast<vil2_image_view<vxl_int_16> &>(*view_);
      const vil2_image_view<vxl_int_16> &w =
        static_cast<const vil2_image_view<vxl_int_16> &>(im);
      if (v.memory_chunk() == w.memory_chunk())
      {
        if (&v(i0,j0) != w.top_left_ptr())
          assert("ERROR: vil2_memory_image::put_view()\n"
                 "different window from that used in get_view()");
        else return true; // The user has already modified the data in place.
      }
      vil2_copy_to_window(w, v, i0, j0);
      return true;
    }
  case  VIL2_PIXEL_FORMAT_FLOAT:
    {
      vil2_image_view<float> &v =
        static_cast<vil2_image_view<float> &>(*view_);
      const vil2_image_view<float> &w =
        static_cast<const vil2_image_view<float> &>(im);
      if (v.memory_chunk() == w.memory_chunk())
      {
        if (&v(i0,j0) != w.top_left_ptr())
          assert("ERROR: vil2_memory_image::put_view()\n"
                 "different window from that used in get_view()");
        else return true; // The user has already modified the data in place.
      }
      vil2_copy_to_window(w, v, i0, j0);
      return true;
    }
  case  VIL2_PIXEL_FORMAT_DOUBLE:
    {
      vil2_image_view<double> &v =
        static_cast<vil2_image_view<double> &>(*view_);
      const vil2_image_view<double> &w =
        static_cast<const vil2_image_view<double> &>(im);
      if (v.memory_chunk() == w.memory_chunk())
      {
        if (&v(i0,j0) != w.top_left_ptr())
          assert("ERROR: vil2_memory_image::put_view()\n"
                 "different window from that used in get_view()");
        else return true; // The user has already modified the data in place.
      }
      vil2_copy_to_window(w, v, i0, j0);
      return true;
    }
  default:
    vcl_cerr << "WARNING: vil2_memory_image::put_view()\n"
                "\t Unexpected pixel type" << view_->pixel_format() << vcl_endl;
    return 0;
  }

}

