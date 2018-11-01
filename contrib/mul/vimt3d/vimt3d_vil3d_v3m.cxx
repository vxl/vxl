// This is mul/vimt3d/vimt3d_vil3d_v3m.cxx
//:
// \file
// \brief Reader/Writer for v3m format images.
// \author Ian Scott - Manchester

#include <cstdlib>
#include <cstring>
#include <sstream>
#include <ios>
#include <iostream>
#include <algorithm>
#include "vimt3d_vil3d_v3m.h"
//
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vsl/vsl_binary_loader.h>
#include <vsl/vsl_block_binary_rle.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <vil3d/vil3d_image_view.h>
#include <vil3d/vil3d_copy.h>
#include <vil3d/vil3d_property.h>
#include <vimt3d/vimt3d_image_3d_of.h>
#include <vil/vil_exception.h>

//: The magic number to identify a vsl stream as a v3m image.
unsigned vimt3d_vil3d_v3m_format::magic_number()
{
  return 987123875U;
}

vil3d_image_resource_sptr vimt3d_vil3d_v3m_format::make_input_image(const char *filename) const
{
  std::unique_ptr<std::fstream> file(new std::fstream(filename, std::ios::in | std::ios::binary ));
  if (!file.get() || !file->is_open())
    return nullptr;

  // Check file is a v3m file
  {
    vsl_b_istream is(file.get());
    if (!is) return nullptr;
    unsigned magic;
    vsl_b_read(is, magic);
    if (magic != vimt3d_vil3d_v3m_format::magic_number()) return nullptr;
  }
  return new vimt3d_vil3d_v3m_image(std::move(file));
}


//: Make a "generic_image" on which put_section may be applied.
// The file may be opened immediately for writing so that a header can be written.
// The width/height etc are explicitly specified, so that file_format implementors
// know what they need to do...
vil3d_image_resource_sptr vimt3d_vil3d_v3m_format::make_output_image
                   (const char* filename, unsigned ni, unsigned nj,
                    unsigned nk, unsigned nplanes, vil_pixel_format format) const
{
  if ( format != VIL_PIXEL_FORMAT_BYTE && format != VIL_PIXEL_FORMAT_SBYTE &&
       format != VIL_PIXEL_FORMAT_UINT_32 && format != VIL_PIXEL_FORMAT_INT_32 &&
       format != VIL_PIXEL_FORMAT_UINT_16 && format != VIL_PIXEL_FORMAT_INT_16 &&
       format != VIL_PIXEL_FORMAT_FLOAT && format != VIL_PIXEL_FORMAT_DOUBLE &&
       format != VIL_PIXEL_FORMAT_BOOL)
  {
    std::cerr << "vimt3d_vil3d_v3m_format::make_output_image() WARNING\n"
             << "  Unable to deal with file format : " << format << std::endl;
    return nullptr;
  }

  std::unique_ptr<std::fstream> of(
    new std::fstream(filename, std::ios::out | std::ios::binary | std::ios::trunc) );
  if (!of.get() || !of->is_open())
  {
    std::cerr << "vimt3d_vil3d_v3m_format::make_output_image() WARNING\n"
             << "  Unable to open file: " << filename << std::endl;
    return nullptr;
  }

  return new vimt3d_vil3d_v3m_image(std::move(of), ni, nj, nk, nplanes, format);
}


bool vimt3d_vil3d_v3m_image::header_t::operator==(const header_t& rhs) const
{
  return this->ni == rhs.ni
    && this->nj == rhs.nj
    && this->nk == rhs.nk
    && this->nplanes == rhs.nplanes
    && this->pixel_format == rhs.pixel_format
    && this->w2i == rhs.w2i;
}

//: Load full image on demand.
void vimt3d_vil3d_v3m_image::load_full_image() const
{
  file_->seekg(0);
  vsl_b_istream is(file_);
  unsigned magic;
  vsl_b_read(is, magic);
  if (magic != vimt3d_vil3d_v3m_format::magic_number())
  {
    im_ =nullptr;
    is.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
    vil_exception_warning(vil_exception_corrupt_image_file(
      "vimt3d_vil3d_v3m_image::load_full_image", "vimt3d_vil3d_v3m_image", "", "Incorrect v3m magic number detected"));
    return;
  }
  short version;
  vsl_b_read(is, version);

  switch (version)
  {
   case 1:
    {
      vsl_b_read(is, header_.ni);
      vsl_b_read(is, header_.nj);
      vsl_b_read(is, header_.nk);
      vsl_b_read(is, header_.nplanes);
      int v;
      vsl_b_read(is, v);
      header_.pixel_format = static_cast<vil_pixel_format>(v);
      vsl_b_read(is, header_.w2i);
      std::size_t size = static_cast<std::size_t>(header_.ni) * header_.nj *
        header_.nk * header_.nplanes;
      vil_memory_chunk_sptr chunk_ptr( new vil_memory_chunk(
        size * vil_pixel_format_sizeof_components(header_.pixel_format), header_.pixel_format) );

      switch (header_.pixel_format)
      {
#define macro( F , T ) \
       case F : \
        { \
          std::ptrdiff_t istep=0, jstep=0, kstep=0, pstep=0; \
          T* origin_ptr=static_cast<T*>(chunk_ptr->data()); \
          if (size) \
          { \
            vsl_block_binary_rle_read(is, static_cast<T *>(chunk_ptr->data()), size); \
            vsl_b_read(is, istep); \
            vsl_b_read(is, jstep); \
            vsl_b_read(is, kstep); \
            vsl_b_read(is, pstep); \
            std::ptrdiff_t offset; \
            vsl_b_read(is, offset); \
            origin_ptr += offset; \
          } \
          im_ = new vimt3d_image_3d_of<T>( \
                      vil3d_image_view<T>(chunk_ptr, origin_ptr, \
                                          header_.ni, header_.nj, header_.nk, header_.nplanes, \
                                          istep, jstep, kstep, pstep), header_.w2i ); \
         } \
        break;
macro(VIL_PIXEL_FORMAT_BYTE , vxl_byte )
//macro(VIL_PIXEL_FORMAT_SBYTE , vxl_sbyte )
//macro(VIL_PIXEL_FORMAT_UINT_32 , vxl_uint_32 )
//macro(VIL_PIXEL_FORMAT_UINT_16 , vxl_uint_16 )
/*macro(VIL_PIXEL_FORMAT_INT_32 , vxl_int_32 )*/
macro(VIL_PIXEL_FORMAT_INT_16 , vxl_int_16 )
//macro(VIL_PIXEL_FORMAT_BOOL , bool )
macro(VIL_PIXEL_FORMAT_FLOAT , float )
macro(VIL_PIXEL_FORMAT_DOUBLE , double )
#undef macro
       case  VIL_PIXEL_FORMAT_INT_32 :
        {
          auto* origin_ptr=static_cast<vxl_int_32*>(chunk_ptr->data());
          std::ptrdiff_t istep=0, jstep=0, kstep=0, pstep=0;
          if (size)
          {
            vsl_block_binary_rle_read(is, static_cast<vxl_int_32 *>(chunk_ptr->data()), size);
            vsl_b_read(is, istep);
            vsl_b_read(is, jstep);
            vsl_b_read(is, kstep);
            vsl_b_read(is, pstep);
            std::ptrdiff_t offset;
            vsl_b_read(is, offset);
            origin_ptr += offset;
          }
          im_ = new vimt3d_image_3d_of<vxl_int_32>(
                      vil3d_image_view<vxl_int_32>(chunk_ptr, origin_ptr,
                                                   header_.ni, header_.nj, header_.nk, header_.nplanes,
                                                   istep, jstep, kstep, pstep),
                      header_.w2i );
         }
        break;
       default:
        vil_exception_error(vil_exception_unsupported_pixel_format(
          im_->image_base().pixel_format(), "vimt3d_vil3d_v3m_image constructor"));
      }
    }
    break;

   default:
    im_ =nullptr;
    is.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
    std::ostringstream oss;
    oss << "I/O ERROR: vimt3d_vil3d_v3m_image::load_full_image()\n"
        << "           Unknown version number "<< version << '\n';
    vil_exception_warning(vil_exception_invalid_version(
      "vimt3d_vil3d_v3m_image::load_full_image", "vimt3d_vil3d_v3m_image", "", oss.str()));
    return;
  }
  if (!is)
  {
    vil_exception_warning(vil_exception_corrupt_image_file(
      "vimt3d_vil3d_v3m_image::load_full_image", "vimt3d_vil3d_v3m_image", "", "Failed to read all expected values."));
    im_=nullptr;
  }
}


//: Private constructor, use vil3d_load instead.
// This object takes ownership of the file, for reading.
vimt3d_vil3d_v3m_image::vimt3d_vil3d_v3m_image(std::unique_ptr<std::fstream> file):
  file_(file.release()), im_(nullptr), dirty_(false)
{
  file_->seekg(0);
  vsl_b_istream is(file_);

  unsigned magic;
  vsl_b_read(is, magic);
  if (magic != vimt3d_vil3d_v3m_format::magic_number())
  {
    is.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
    vil_exception_warning(vil_exception_corrupt_image_file(
      "vimt3d_vil3d_v3m_image constructor", "vimt3d_vil3d_v3m_image", "", "Incorrect v3m magic number detected"));
    return;
  }

  short version;
  vsl_b_read(is, version);

  switch (version)
  {
    case 1:
      {
        vsl_b_read(is, header_.ni);
        vsl_b_read(is, header_.nj);
        vsl_b_read(is, header_.nk);
        vsl_b_read(is, header_.nplanes);
        int v;
        vsl_b_read(is, v);
        header_.pixel_format = static_cast<vil_pixel_format>(v);
        vsl_b_read(is, header_.w2i);

        break;
      }
    default:
      std::ostringstream oss;
      oss << "I/O ERROR: vimt3d_vil3d_v3m_image::vimt3d_vil3d_v3m_image()\n"
          << "           Unknown version number "<< version << '\n';
      vil_exception_warning(vil_exception_invalid_version("vimt3d_vil3d_v3m_image constructor", "vimt3d_vil3d_v3m_image", "", oss.str()));
      return;
  }
  if (!is)
  {
    vil_exception_warning(vil_exception_image_io(
      "vimt3d_vil3d_v3m_image constructor", "vimt3d_vil3d_v3m_image", ""));
  }
  return;
}

//: Private constructor, use vil3d_save instead.
// This object takes ownership of the file, for writing.
vimt3d_vil3d_v3m_image::vimt3d_vil3d_v3m_image(std::unique_ptr<std::fstream> file, unsigned ni,
                                               unsigned nj, unsigned nk,
                                               unsigned nplanes,
                                               vil_pixel_format format):
  file_(file.release()), im_(nullptr), dirty_(true)
{
  header_.ni = ni;
  header_.nj = nj;
  header_.nk = nk;
  header_.nplanes = nplanes;
  header_.pixel_format = format;
  switch (format)
  {
#define macro( F , T ) \
   case F : \
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
macro(VIL_PIXEL_FORMAT_DOUBLE , double )
#undef macro
   default:
    vil_exception_error(vil_exception_unsupported_pixel_format(
      format, "vimt3d_vil3d_v3m_image constructor"));
  }
}


vimt3d_vil3d_v3m_image::~vimt3d_vil3d_v3m_image()
{
  if (dirty_)
  {
    file_->seekp(0);
    vsl_b_ostream os(file_);

    vsl_b_write(os, vimt3d_vil3d_v3m_format::magic_number());

    constexpr short version = 1;
    vsl_b_write(os, version);

    vsl_b_write(os, im_->image_base().ni());
    vsl_b_write(os, im_->image_base().nj());
    vsl_b_write(os, im_->image_base().nk());
    vsl_b_write(os, im_->image_base().nplanes());
    vsl_b_write(os, im_->image_base().pixel_format());
    vsl_b_write(os, im_->world2im());

    switch (im_->image_base().pixel_format())
    {
#define macro( F , T ) \
     case F : \
      { \
        const vil3d_image_view<T> &image = static_cast<vil3d_image_view<T> >(im_->image_base()); \
        if (image.size()) \
        { \
          /* Data should be stored compactly, since this class controls im_ */ \
          assert(!image || image.size()*sizeof(T) == image.memory_chunk()->size()); \
          /* Data should be stored with non-compound type, since this class controls im_ */ \
          assert(!image || image.pixel_format() == image.memory_chunk()->pixel_format()); \
          vsl_block_binary_rle_write(os, static_cast<const T *>(image.memory_chunk()->const_data()), \
                                     image.size()); \
          vsl_b_write(os, image.istep()); \
          vsl_b_write(os, image.jstep()); \
          vsl_b_write(os, image.kstep()); \
          vsl_b_write(os, image.planestep()); \
          std::ptrdiff_t offset = (image.origin_ptr() - \
                                  reinterpret_cast<const T*>(image.memory_chunk()->data())); \
          vsl_b_write(os, offset); \
        } \
       } \
      break;
macro(VIL_PIXEL_FORMAT_BYTE , vxl_byte )
//macro(VIL_PIXEL_FORMAT_SBYTE , vxl_sbyte )
//macro(VIL_PIXEL_FORMAT_UINT_32 , vxl_uint_32 )
//macro(VIL_PIXEL_FORMAT_UINT_16 , vxl_uint_16 )
/*macro(VIL_PIXEL_FORMAT_INT_32 , vxl_int_32 )*/
macro(VIL_PIXEL_FORMAT_INT_16 , vxl_int_16 )
//macro(VIL_PIXEL_FORMAT_BOOL , bool )
macro(VIL_PIXEL_FORMAT_FLOAT , float )
macro(VIL_PIXEL_FORMAT_DOUBLE , double )
#undef macro
     case VIL_PIXEL_FORMAT_INT_32 :
      {
        const vil3d_image_view<vxl_int_32> &image = static_cast<vil3d_image_view<vxl_int_32> >(im_->image_base());
        if (image.size())
        {
          /* Data should be stored compactly, since this class controls im_ */
          assert(!image || image.size()*sizeof(vxl_int_32) == image.memory_chunk()->size());
          /* Data should be stored with non-compound type, since this class controls im_ */
          assert(!image || image.pixel_format() == image.memory_chunk()->pixel_format());
          vsl_block_binary_rle_write(os, static_cast<const vxl_int_32 *>(image.memory_chunk()->const_data()),
            image.size());
          vsl_b_write(os, image.istep());
          vsl_b_write(os, image.jstep());
          vsl_b_write(os, image.kstep());
          vsl_b_write(os, image.planestep());
          std::ptrdiff_t offset = (image.origin_ptr() -
                                  reinterpret_cast<const vxl_int_32*>(image.memory_chunk()->data()));
          vsl_b_write(os, offset);
        }
       }
      break;
     default:
      vil_exception_error(vil_exception_unsupported_pixel_format(
        im_->image_base().pixel_format(), "vimt3d_vil3d_v3m_image constructor"));
    }

// Write view properties.
  }
  delete file_;
  delete im_;
}


//: Dimensions:  nplanes x ni x nj x nk.
// This concept is treated as a synonym to components.
unsigned vimt3d_vil3d_v3m_image::nplanes() const
{
  return header_.nplanes;
}

//: Dimensions:  nplanes x ni x nj x nk.
// The number of pixels in each row.
unsigned vimt3d_vil3d_v3m_image::ni() const
{
  return header_.ni;
}

//: Dimensions:  nplanes x ni x nj x nk.
// The number of pixels in each column.
unsigned vimt3d_vil3d_v3m_image::nj() const
{
  return header_.nj;
}

//: Dimensions:  nplanes x ni x nj x nk.
// The number of slices per image.
unsigned vimt3d_vil3d_v3m_image::nk() const
{
  return header_.nk;
}

//: Pixel Format.
enum vil_pixel_format vimt3d_vil3d_v3m_image::pixel_format() const
{
  return header_.pixel_format;
}


//: Get the properties (of the first slice)
bool vimt3d_vil3d_v3m_image::get_property(char const *key, void * value) const
{
  if (std::strcmp(vil3d_property_voxel_size, key)==0)
  {
    vimt3d_transform_3d i2w=header_.w2i.inverse();

    auto* array =  static_cast<float*>(value);
    array[0] = (float) (i2w(1,0,0)-i2w(0,0,0)).length();
    array[1] = (float) (i2w(0,1,0)-i2w(0,0,0)).length();
    array[2] = (float) (i2w(0,0,1)-i2w(0,0,0)).length();
    return true;
  }

  if (std::strcmp(vil3d_property_origin_offset, key)==0)
  {
    vgl_point_3d<double> origin = header_.w2i.origin();
    auto* array =  static_cast<float*>(value);
    array[0] = (float)(origin.x());
    array[1] = (float)(origin.y());
    array[2] = (float)(origin.z());
    return true;
  }

  return false;
}


//: Set the size of the each pixel in the i,j,k directions.
// Return false if underlying image doesn't store pixel sizes.
bool vimt3d_vil3d_v3m_image::set_voxel_size_mm(float si, float sj, float sk)
{
  const vimt3d_transform_3d &tr = im_->world2im();

// Try to adjust pixel size without modifying rest of transform
  vgl_vector_3d<double> w111 = header_.w2i(1.0, 1.0, 1.0) - header_.w2i.origin();

  vimt3d_transform_3d zoom;
  zoom.set_zoom_only (1000*w111.x()/si, 1000*w111.y()/sj, 1000*w111.z()/sk, 0.0, 0.0, 0.0);

  header_.w2i = header_.w2i * zoom;
  if (im_)
    im_->set_world2im(tr*zoom);
  dirty_ = true;

  return true;
}

const vimt3d_transform_3d & vimt3d_vil3d_v3m_image::world2im() const
{
  return header_.w2i;
}

void vimt3d_vil3d_v3m_image::set_world2im(const vimt3d_transform_3d & tr)
{
  header_.w2i=tr;
  if (im_)
    im_->set_world2im(header_.w2i);
  dirty_ = true;
}


//: Create a read/write view of a copy of this data.
// Currently not yet implemented.
// \return 0 if unable to get view of correct size.
vil3d_image_view_base_sptr vimt3d_vil3d_v3m_image::get_copy_view(unsigned i0, unsigned ni,
                                                                 unsigned j0, unsigned nj,
                                                                 unsigned k0, unsigned nk) const
{
  if (!im_)
    load_full_image();
  if (!im_) return nullptr; // If load full image failed then im_ will remain null

  const vil3d_image_view_base &view = im_->image_base();

  if (i0 + ni > view.ni() || j0 + nj > view.nj() ||
      k0 + nk > view.nk()) return nullptr;

  switch (view.pixel_format())
  {
#define macro( F , T ) \
   case F : { \
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
macro(VIL_PIXEL_FORMAT_DOUBLE , double )
#undef macro
   default:
     vil_exception_warning(vil_exception_unsupported_pixel_format(
       view.pixel_format(), "vimt3d_vil3d_v3m_image::get_copy_view"));
    return nullptr;
  }
}

//: Create a read/write view of a copy of this data.
// \return 0 if unable to get view of correct size.
vil3d_image_view_base_sptr vimt3d_vil3d_v3m_image::get_view(unsigned i0, unsigned ni,
                                                            unsigned j0, unsigned nj,
                                                            unsigned k0, unsigned nk) const
{
  if (!im_)
    load_full_image();
  if (!im_) return nullptr; // If load full image failed then im_ will remain null

  const vil3d_image_view_base &view = im_->image_base();

  if (i0 + ni > view.ni() || j0 + nj > view.nj() ||
      k0 + nk > view.nk()) return nullptr;

  switch (view.pixel_format())
  {
#define macro( F , T ) \
   case F : { \
    const vil3d_image_view< T > &v = \
      static_cast<const vil3d_image_view< T > &>(view); \
      return new vil3d_image_view< T >(v.memory_chunk(), (!v)?0:&v(i0,j0,k0), \
                                       ni, nj, nk, v.nplanes(), \
                                       v.istep(), v.jstep(), v.kstep(), \
                                       v.planestep()); }
macro(VIL_PIXEL_FORMAT_BYTE , vxl_byte )
//macro(VIL_PIXEL_FORMAT_SBYTE , vxl_sbyte )
//macro(VIL_PIXEL_FORMAT_UINT_32 , vxl_uint_32 )
//macro(VIL_PIXEL_FORMAT_UINT_16 , vxl_uint_16 )
/*macro(VIL_PIXEL_FORMAT_INT_32 , vxl_int_32 )*/
macro(VIL_PIXEL_FORMAT_INT_16 , vxl_int_16 )
//macro(VIL_PIXEL_FORMAT_BOOL , bool )
macro(VIL_PIXEL_FORMAT_FLOAT , float )
macro(VIL_PIXEL_FORMAT_DOUBLE , double )
#undef macro
   case VIL_PIXEL_FORMAT_INT_32 : {
    const auto &v =
      static_cast<const vil3d_image_view< vxl_int_32 > &>(view);
    return new vil3d_image_view< vxl_int_32 >(v.memory_chunk(), v.size()?&v(i0,j0,k0):nullptr,
                                              ni, nj, nk, v.nplanes(),
                                              v.istep(), v.jstep(), v.kstep(),
                                              v.planestep()); }
   default:
     vil_exception_warning(vil_exception_unsupported_pixel_format(
       view.pixel_format(), "vimt3d_vil3d_v3m_image::get_view"));
    return nullptr;
  }
}


//: Set the contents of the volume.
bool vimt3d_vil3d_v3m_image::put_view(const vil3d_image_view_base& vv,
                                      unsigned i0, unsigned j0, unsigned k0)
{
  if (!im_)
    load_full_image();
  if (!im_) return false; // If load full image failed then im_ will remain null

  if (!view_fits(vv, i0, j0, k0))
  {
    vil_exception_warning(vil_exception_out_of_bounds("vimt3d_vil3d_v3m_image::put_view"));
    return false;
  }

  if (vv.pixel_format() != im_->image_base().pixel_format())
  {
    vil_exception_warning(vil_exception_pixel_formats_incompatible(
      vv.pixel_format(), im_->image_base().pixel_format(), "vimt3d_vil3d_v3m_image::put_view"));
    return false;
  }

  dirty_ = true;

  switch (vv.pixel_format())
  {
#define macro( F , T ) \
   case F : \
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
    macro(VIL_PIXEL_FORMAT_DOUBLE , double )
#undef macro
   default:
     vil_exception_warning(vil_exception_unsupported_pixel_format(
       vv.pixel_format(), "vimt3d_vil3d_v3m_image::put_view"));
    return false;
  }
}
