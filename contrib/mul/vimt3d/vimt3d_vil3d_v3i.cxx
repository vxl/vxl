// This is mul/vimt3d/vimt3d_vil3d_v3i.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \brief Reader/Writer for v3i format images.
// \author Ian Scott - Manchester

#include "vimt3d_vil3d_v3i.h"
#include <vcl_cstdlib.h> // for vcl_abort()
#include <vcl_cstring.h> // for vcl_strcmp()
#include <vcl_cassert.h>
#include <vsl/vsl_binary_loader.h>
#include <vil3d/vil3d_image_view.h>
#include <vil3d/vil3d_copy.h>
#include <vil3d/vil3d_property.h>
#include <vimt3d/vimt3d_image_3d_of.h>
// v3i magic number
const unsigned V3I_MAGIC = 987123873U;


vil3d_image_resource_sptr vimt3d_vil3d_v3i_format::make_input_image(const char *filename) const
{
  vcl_fstream *file = new vcl_fstream(filename, vcl_ios_in | vcl_ios_binary );
  if (!file)
  {
    vcl_cerr << "vimt3d_vil3d_v3i_format::make_output_image() WARNING\n"
             << "  Unable to open file: " << filename << vcl_endl;
    return 0;
  }

  // Check file is a v3i file
  vsl_b_istream is(file);
  if (!is) return 0;
  unsigned magic;
  vsl_b_read(is, magic);
  if (magic != V3I_MAGIC) return 0;

  return new vimt3d_vil3d_v3i_image(file);
}


//: Make a "generic_image" on which put_section may be applied.
// The file may be opened immediately for writing so that a header can be written.
// The width/height etc are explicitly specified, so that file_format implementors
// know what they need to do...
vil3d_image_resource_sptr vimt3d_vil3d_v3i_format::make_output_image
                   (const char* filename, unsigned ni, unsigned nj,
                    unsigned nk, unsigned nplanes, vil_pixel_format format) const
{
  if ( format != VIL_PIXEL_FORMAT_BYTE && format != VIL_PIXEL_FORMAT_SBYTE &&
       format != VIL_PIXEL_FORMAT_UINT_32 && format != VIL_PIXEL_FORMAT_INT_32 &&
       format != VIL_PIXEL_FORMAT_UINT_16 && format != VIL_PIXEL_FORMAT_INT_16 &&
       format != VIL_PIXEL_FORMAT_FLOAT && format != VIL_PIXEL_FORMAT_DOUBLE &&
       format != VIL_PIXEL_FORMAT_BOOL)
  {
    vcl_cerr << "vimt3d_vil3d_v3i_format::make_output_image() WARNING\n"
             << "  Unable to deal with file format : " << format << vcl_endl;
    return 0;
  }

  vcl_fstream *of = new vcl_fstream(filename, vcl_ios_out | vcl_ios_binary | vcl_ios_trunc);
  if (!of)
  {
    vcl_cerr << "vimt3d_vil3d_v3i_format::make_output_image() WARNING\n"
             << "  Unable to open file: " << filename << vcl_endl;
    return 0;
  }

  return new vimt3d_vil3d_v3i_image(of, ni, nj, nk, nplanes, format);
}


//: Private constructor, use vil3d_load instead.
// This object takes ownership of the file, for reading.
vimt3d_vil3d_v3i_image::vimt3d_vil3d_v3i_image(vcl_fstream *file):
  file_(file), im_(0), dirty_(false)
{
  file->seekg(0);
  vsl_b_istream is(file_);

  unsigned magic;
  vsl_b_read(is, magic);
  assert(magic == V3I_MAGIC);

  short version;
  vsl_b_read(is, version);

  switch (version)
  {
   case 1:
   {
    vimt_image *p_im=0;
    vsl_b_read(is, p_im);
    im_ = dynamic_cast<vimt3d_image_3d *>(p_im);
    break;
   }
   default:
    vcl_cerr << "I/O ERROR: vimt3d_vil3d_v3i_image::vimt3d_vil3d_v3i_image()\n"
             << "           Unknown version number "<< version << '\n';
    return;
  }
}

//: Private constructor, use vil3d_save instead.
// This object takes ownership of the file, for writing.
vimt3d_vil3d_v3i_image::vimt3d_vil3d_v3i_image(vcl_fstream *file, unsigned ni,
                                               unsigned nj, unsigned nk,
                                               unsigned nplanes,
                                               vil_pixel_format format):
  file_(file), im_(0), dirty_(true)
{
  switch (format)
  {
#define macro( F , T ) \
   case  F : \
    im_ = new vimt3d_image_3d_of< T > (ni, nj, nk, nplanes); \
    break;
macro(VIL_PIXEL_FORMAT_BYTE , vxl_byte )
//macro(VIL_PIXEL_FORMAT_SBYTE , vxl_sbyte )
//macro(VIL_PIXEL_FORMAT_UINT_32 , vxl_uint_32 )
//macro(VIL_PIXEL_FORMAT_UINT_16 , vxl_uint_16 )
macro(VIL_PIXEL_FORMAT_INT_32 , vxl_int_32 )
macro(VIL_PIXEL_FORMAT_INT_16 , vxl_int_16 )
//macro(VIL_PIXEL_FORMAT_BOOL , bool )
macro(VIL_PIXEL_FORMAT_FLOAT , float )
//macro(VIL_PIXEL_FORMAT_DOUBLE , double )
#undef macro
   default:
    vcl_cerr << "I/O ERROR: vimt3d_vil3d_v3i_image::vimt3d_vil3d_v3i_image()\n"
             << "           Unknown vil_pixel_format "<< format << '\n';
    vcl_abort();
  }
}

vimt3d_vil3d_v3i_image::~vimt3d_vil3d_v3i_image()
{
  if (dirty_)
  {
    file_->seekp(0);
    vsl_b_ostream os(file_);

    vsl_b_write(os, V3I_MAGIC);

    const short version = 1;
    vsl_b_write(os, version);


    vimt_image *p_im=im_;
    vsl_b_write(os, p_im);
  }
  delete file_;
  delete im_;
}


//: Dimensions:  nplanes x ni x nj x nk.
// This concept is treated as a synonym to components.
unsigned vimt3d_vil3d_v3i_image::nplanes() const
{
  return im_->image_base().nplanes();
}

//: Dimensions:  nplanes x ni x nj x nk.
// The number of pixels in each row.
unsigned vimt3d_vil3d_v3i_image::ni() const
{
  return im_->image_base().ni();
}

//: Dimensions:  nplanes x ni x nj x nk.
// The number of pixels in each column.
unsigned vimt3d_vil3d_v3i_image::nj() const
{
  return im_->image_base().nj();
}

//: Dimensions:  nplanes x ni x nj x nk.
// The number of slices per image.
unsigned vimt3d_vil3d_v3i_image::nk() const
{
  return im_->image_base().nk();
}

//: Pixel Format.
enum vil_pixel_format vimt3d_vil3d_v3i_image::pixel_format() const
{
  return im_->image_base().pixel_format();
}


//: Get the properties (of the first slice)
bool vimt3d_vil3d_v3i_image::get_property(char const *key, void * value) const
{
  const vimt3d_transform_3d &tr = im_->world2im();

  if (vcl_strcmp(vil3d_property_voxel_size, key)==0)
  {
    vgl_vector_3d<double> p111 = tr.inverse()(1.0, 1.0, 1.0) - tr.inverse().origin();
    //Assume no rotation or shearing.

    float* array =  static_cast<float*>(value);
    array[0] = (float) p111.x();
    array[1] = (float) p111.y();
    array[2] = (float) p111.z();
    return true;
  }

  if (vcl_strcmp(vil3d_property_origin_offset, key)==0)
  {
    vgl_point_3d<double> origin = tr.origin();
    float* array =  static_cast<float*>(value);
    array[0] = (float)(origin.x());
    array[1] = (float)(origin.y());
    array[2] = (float)(origin.z());
    return true;
  }

  return false;
}


//: Set the size of the each pixel in the i,j,k directions.
// Return false if underlying image doesn't store pixel sizes.
bool vimt3d_vil3d_v3i_image::set_voxel_size(float si, float sj, float sk)
{
  const vimt3d_transform_3d &tr = im_->world2im();

// Try to adjust pixel size without modifying rest of transform
  vgl_vector_3d<double> w111 = tr(1.0, 1.0, 1.0) - tr.origin();

  vimt3d_transform_3d zoom;
  zoom.set_zoom_only (w111.x()/si, w111.y()/sj, w111.z()/sk, 0.0, 0.0, 0.0);

  im_->set_world2im(tr*zoom);
  return true;
}

const vimt3d_transform_3d & vimt3d_vil3d_v3i_image::world2im() const
{
  return im_->world2im();
}

void vimt3d_vil3d_v3i_image::set_world2im(const vimt3d_transform_3d & tr)
{
  im_->set_world2im(tr);
  dirty_ = true;
}


//: Create a read/write view of a copy of this data.
// Currently not yet implemented.
// \return 0 if unable to get view of correct size.
vil3d_image_view_base_sptr vimt3d_vil3d_v3i_image::get_copy_view(unsigned i0, unsigned ni,
                                                                 unsigned j0, unsigned nj,
                                                                 unsigned k0, unsigned nk) const
{
  const vil3d_image_view_base &view = im_->image_base();

  if (i0 + ni > view.ni() || j0 + nj > view.nj() ||
      k0 + nk > view.nk()) return 0;

  switch (view.pixel_format())
  {
#define macro( F , T ) \
   case  F : { \
    const vil3d_image_view< T > &v = \
      static_cast<const vil3d_image_view< T > &>(view); \
    vil3d_image_view< T > w(v.memory_chunk(), &v(i0,j0,k0), \
                            ni, nj, nk, v.nplanes(), \
                            v.istep(), v.jstep(), v.kstep(), v.planestep()); \
    return new vil3d_image_view< T >(vil3d_copy_deep(w)); }
macro(VIL_PIXEL_FORMAT_BYTE, vxl_byte )
//macro(VIL_PIXEL_FORMAT_SBYTE , vxl_sbyte )
//macro(VIL_PIXEL_FORMAT_UINT_32 , vxl_uint_32 )
//macro(VIL_PIXEL_FORMAT_UINT_16 , vxl_uint_16 )
macro(VIL_PIXEL_FORMAT_INT_32 , vxl_int_32 )
macro(VIL_PIXEL_FORMAT_INT_16 , vxl_int_16 )
//macro(VIL_PIXEL_FORMAT_BOOL , bool )
macro(VIL_PIXEL_FORMAT_FLOAT , float )
//macro(VIL_PIXEL_FORMAT_DOUBLE , double )
#undef macro
   default:
    return 0;
  }
}

//: Create a read/write view of a copy of this data.
// \return 0 if unable to get view of correct size.
vil3d_image_view_base_sptr vimt3d_vil3d_v3i_image::get_view(unsigned i0, unsigned ni,
                                                            unsigned j0, unsigned nj,
                                                            unsigned k0, unsigned nk) const
{
  const vil3d_image_view_base &view = im_->image_base();


  if (i0 + ni > view.ni() || j0 + nj > view.nj() ||
      k0 + nk > view.nk()) return 0;

  switch (view.pixel_format())
  {
#define macro( F , T ) \
   case  F : { \
    const vil3d_image_view< T > &v = \
      static_cast<const vil3d_image_view< T > &>(view); \
    return new vil3d_image_view< T >(v.memory_chunk(), &v(i0,j0,k0), \
                                     ni, nj, nk, v.nplanes(), \
                                     v.istep(), v.jstep(), v.kstep(), \
                                     v.planestep()); }
macro(VIL_PIXEL_FORMAT_BYTE , vxl_byte )
//macro(VIL_PIXEL_FORMAT_SBYTE , vxl_sbyte )
//macro(VIL_PIXEL_FORMAT_UINT_32 , vxl_uint_32 )
//macro(VIL_PIXEL_FORMAT_UINT_16 , vxl_uint_16 )
macro(VIL_PIXEL_FORMAT_INT_32 , vxl_int_32 )
macro(VIL_PIXEL_FORMAT_INT_16 , vxl_int_16 )
//macro(VIL_PIXEL_FORMAT_BOOL , bool )
macro(VIL_PIXEL_FORMAT_FLOAT , float )
//macro(VIL_PIXEL_FORMAT_DOUBLE , double )
#undef macro
   default:
    return 0;
  }
}


//: Set the contents of the volume.
bool vimt3d_vil3d_v3i_image::put_view(const vil3d_image_view_base& vv,
                                      unsigned i0, unsigned j0, unsigned k0)
{
  if (!view_fits(vv, i0, j0, k0))
  {
    vcl_cerr << "ERROR: " << __FILE__ << ":\n view does not fit\n";
    return false;
  }

  if (vv.pixel_format() != im_->image_base().pixel_format())
  {
    vcl_cerr << "ERROR: vimt3d_vil3d_v3i_image::put_view(). Pixel formats do not match\n";
    return false;
  }


  dirty_ = true;

  switch (vv.pixel_format())
  {
#define macro( F , T ) \
   case  F : \
    vil3d_copy_to_window(static_cast<vil3d_image_view<T >const&>(vv), \
                         static_cast<vimt3d_image_3d_of<T >&>(*im_).image(), \
                         i0, j0, k0); \
    return true;

    macro(VIL_PIXEL_FORMAT_BYTE , vxl_byte )
//  macro(VIL_PIXEL_FORMAT_SBYTE , vxl_sbyte )
//  macro(VIL_PIXEL_FORMAT_UINT_32 , vxl_uint_32 )
//  macro(VIL_PIXEL_FORMAT_UINT_16 , vxl_uint_16 )
    macro(VIL_PIXEL_FORMAT_INT_32 , vxl_int_32 )
    macro(VIL_PIXEL_FORMAT_INT_16 , vxl_int_16 )
//  macro(VIL_PIXEL_FORMAT_BOOL , bool )
    macro(VIL_PIXEL_FORMAT_FLOAT , float )
//  macro(VIL_PIXEL_FORMAT_DOUBLE , double )
#undef macro
   default:
    return false;
  }
}
