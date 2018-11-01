// This is mul/vimt3d/vimt3d_vil3d_v3i.cxx
//:
// \file
// \brief Reader/Writer for v3i format images.
// \author Ian Scott - Manchester

#include <cstdlib>
#include <cstring>
#include <sstream>
#include <ios>
#include <iostream>
#include <algorithm>
#include "vimt3d_vil3d_v3i.h"
//
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vsl/vsl_binary_loader.h>
#include <vsl/vsl_block_binary.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <vil3d/vil3d_image_view.h>
#include <vil3d/vil3d_copy.h>
#include <vil3d/vil3d_property.h>
#include <vimt3d/vimt3d_image_3d_of.h>
#include <vil/vil_exception.h>

//: The magic number to identify a vsl stream as a v3i image.
// You can create/read a v3i image using vsl by opening the stream,
// reading/writing magic_number(), then reading/writing a pointer to a vimt_image.
unsigned vimt3d_vil3d_v3i_format::magic_number()
{
  return 987123873U;
}

vil3d_image_resource_sptr vimt3d_vil3d_v3i_format::make_input_image(const char *filename) const
{
  std::unique_ptr<std::fstream> file(new std::fstream(filename, std::ios::in | std::ios::binary ));
  if (!file.get() || !file->is_open())
    return nullptr;

  // Check file is a v3i file
  {
    vsl_b_istream is(file.get());
    if (!is) return nullptr;
    unsigned magic;
    vsl_b_read(is, magic);
    if (magic != vimt3d_vil3d_v3i_format::magic_number()) return nullptr;
  }
  return new vimt3d_vil3d_v3i_image(std::move(file));
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
    std::cerr << "vimt3d_vil3d_v3i_format::make_output_image() WARNING\n"
             << "  Unable to deal with file format : " << format << std::endl;
    return nullptr;
  }

  std::unique_ptr<std::fstream> of(
    new std::fstream(filename, std::ios::out | std::ios::binary | std::ios::trunc) );
  if (!of.get() || !of->is_open())
  {
    std::cerr << "vimt3d_vil3d_v3i_format::make_output_image() WARNING\n"
             << "  Unable to open file: " << filename << std::endl;
    return nullptr;
  }

  return new vimt3d_vil3d_v3i_image(std::move(of), ni, nj, nk, nplanes, format);
}


//: Skip the reading of a vil_memory_chunk
bool vimt3d_vil3d_v3i_image::skip_b_read_vil_memory_chunk(vsl_b_istream& is, unsigned sizeof_T) const
{ // Copy of vsl_b_read(vsl_b_istream &is, vil_memory_chunk& chunk)
  short vil_memory_chunk_version;
  vsl_b_read(is, vil_memory_chunk_version);
  int int_format;
  vsl_b_read(is, int_format);
  if (vil_pixel_format_component_format(header_.pixel_format) != vil_pixel_format(int_format))
  {
    std::cerr << "I/O ERROR: vimt3d_vil3d_v3i_image::vimt3d_vil3d_v3i_image()\n"
             << "           chunk pixel format is incompatible with image\n";
    is.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
    return false;
  }
  unsigned n;
  vsl_b_read(is, n);
  switch (vil_memory_chunk_version)
  {
   case 1:
    if (vil_pixel_format_component_format(header_.pixel_format) == VIL_PIXEL_FORMAT_DOUBLE ||
        vil_pixel_format_component_format(header_.pixel_format) == VIL_PIXEL_FORMAT_FLOAT)
      is.is().seekg(n*sizeof_T, std::ios::cur); // skip image pixel data.
    else
      // Give up trying to load header - it can't be done efficiently.
      return false;
    break;
   case 2:
    if (vil_pixel_format_component_format(header_.pixel_format) == VIL_PIXEL_FORMAT_SBYTE ||
        vil_pixel_format_component_format(header_.pixel_format) == VIL_PIXEL_FORMAT_BYTE )
      vsl_block_binary_read_confirm_specialisation(is, false);
    else
      vsl_block_binary_read_confirm_specialisation(is, true);
    if (!is) return false;
    if (vil_pixel_format_component_format(header_.pixel_format) == VIL_PIXEL_FORMAT_DOUBLE ||
        vil_pixel_format_component_format(header_.pixel_format) == VIL_PIXEL_FORMAT_FLOAT ||
        vil_pixel_format_component_format(header_.pixel_format) == VIL_PIXEL_FORMAT_SBYTE ||
        vil_pixel_format_component_format(header_.pixel_format) == VIL_PIXEL_FORMAT_BYTE )
      is.is().seekg(n*sizeof_T, std::ios::cur); // skip image pixel data.
    else
    {
      std::size_t n_bytes;
      vsl_b_read(is, n_bytes);
      is.is().seekg(n_bytes, std::ios::cur); // skip image pixel data.
    }
    break;
   case 3:
    vsl_block_binary_read_confirm_specialisation(is, true);
    if (!is) return false;
    if (vil_pixel_format_component_format(header_.pixel_format) == VIL_PIXEL_FORMAT_DOUBLE ||
        vil_pixel_format_component_format(header_.pixel_format) == VIL_PIXEL_FORMAT_FLOAT ||
        vil_pixel_format_component_format(header_.pixel_format) == VIL_PIXEL_FORMAT_SBYTE ||
        vil_pixel_format_component_format(header_.pixel_format) == VIL_PIXEL_FORMAT_BYTE )
      is.is().seekg(n*sizeof_T, std::ios::cur); // skip image pixel data.
    else
    {
      std::size_t n_bytes;
      vsl_b_read(is, n_bytes);
      is.is().seekg(n_bytes, std::ios::cur); // skip image pixel data.
    }
    break;
   default:
    std::cerr << "I/O ERROR: vimt3d_vil3d_v3i_image::vimt3d_vil3d_v3i_image\n"
             << "           Unknown vil_memory_chunk version number "<< vil_memory_chunk_version << '\n';
    is.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
    return false;
  }
  return true;
}


bool vimt3d_vil3d_v3i_image::header_t::operator==(const header_t& rhs) const
{
  return this->ni == rhs.ni
    && this->nj == rhs.nj
    && this->nk == rhs.nk
    && this->nplanes == rhs.nplanes
    && this->pixel_format == rhs.pixel_format
    && this->w2i == rhs.w2i;
}

//: Load full image on demand.
void vimt3d_vil3d_v3i_image::load_full_image() const
{
  file_->seekg(0);
  vsl_b_istream is(file_);
  unsigned magic;
  vsl_b_read(is, magic);
  if (magic != vimt3d_vil3d_v3i_format::magic_number())
  {
    im_ =nullptr;
    is.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
    vil_exception_warning(vil_exception_corrupt_image_file(
      "vimt3d_vil3d_v3i_image::load_full_image", "vimt3d_vil3d_v3i_image", "", "Incorrect V3I magic number detected"));
    return;
  }
  short version;
  vsl_b_read(is, version);
  vimt_image *p_im=nullptr;

  switch (version)
  {
    case 1:

    vsl_b_read(is, p_im);
    im_ = dynamic_cast<vimt3d_image_3d *>(p_im);
    break;

    default:
    im_ =nullptr;
    is.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
    std::ostringstream oss;
    oss << "I/O ERROR: vimt3d_vil3d_v3i_image::load_full_image()\n"
        << "           Unknown version number "<< version << '\n';
    vil_exception_warning(vil_exception_invalid_version(
      "vimt3d_vil3d_v3i_image::load_full_image", "vimt3d_vil3d_v3i_image", "", oss.str()));
    return;
  }

  header_t my_header;
  my_header.pixel_format = im_->image_base().pixel_format();
  my_header.ni = im_->image_base().ni();
  my_header.nj = im_->image_base().nj();
  my_header.nk = im_->image_base().nk();
  my_header.nplanes = im_->image_base().nplanes();
  my_header.w2i = im_->world2im();
  if (!(my_header == header_) && ! dirty_)
  {
    im_ =nullptr;
    is.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
    std::ostringstream oss;
    oss << "I/O ERROR: vimt3d_vil3d_v3i_image::load_full_image\n"
        << "           Header is not consistent with previously calculated version.";
    vil_exception_warning(vil_exception_corrupt_image_file(
      "vimt3d_vil3d_v3i_image::load_full_image", "vimt3d_vil3d_v3i_image", "", oss.str()));
    return;
  }
}


//: Private constructor, use vil3d_load instead.
// This object takes ownership of the file, for reading.
vimt3d_vil3d_v3i_image::vimt3d_vil3d_v3i_image(std::unique_ptr<std::fstream> file):
  file_(file.release()), im_(nullptr), dirty_(false)
{
  file_->seekg(0);
  vsl_b_istream is(file_);

  unsigned magic;
  vsl_b_read(is, magic);
  if (magic != vimt3d_vil3d_v3i_format::magic_number())
  {
    is.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
    vil_exception_warning(vil_exception_corrupt_image_file(
      "vimt3d_vil3d_v3i_image constructor", "vimt3d_vil3d_v3i_image", "", "Incorrect V3I magic number detected"));
    return;
  }

  short version;
  vsl_b_read(is, version);

  switch (version)
  {
    case 1:
#if 0
    {
      vimt_image *p_im=0;
      vsl_b_read(is, p_im);
      im_ = dynamic_cast<vimt3d_image_3d *>(p_im);
    }
#else
    { // Copy vimt_image* loader
      vsl_binary_loader<vimt_image>& instance = vsl_binary_loader<vimt_image>::instance();


      if (!is) return;

      std::string name;
      vsl_b_read(is,name);

      if (name=="VSL_NULL_PTR")
      {
        // a v3i image should never have a null pointer.
        is.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
        std::ostringstream oss;
        oss << "I/O ERROR: vimt3d_vil3d_v3i_image::vimt3d_vil3d_v3i_image()\n"
            << "           vimt_image ptr load failure\n";
        vil_exception_warning(vil_exception_corrupt_image_file(
          "vimt3d_vil3d_v3i_image constructor", "vimt3d_vil3d_v3i_image", "", oss.str()));
        return;
      }

      unsigned int i = 0;
      while (i<instance.object().size() && !(instance.object()[i]->is_a()==name)) ++i;

      if (i>=instance.object().size())
      {
        std::ostringstream oss;
        oss << "\n I/O ERROR: vimt3d_vil3d_v3i_image::vimt3d_vil3d_v3i_image()\n"
            << "class name <" << name << "> not in list of loaders\n"
            << instance.object().size()<<" valid loaders:\n";
        for (auto j : instance.object())
          std::cerr << j->is_a() << std::endl;
        is.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
        vil_exception_warning(vil_exception_corrupt_image_file(
          "vimt3d_vil3d_v3i_image constructor", "vimt3d_vil3d_v3i_image", "", oss.str()));
        return;
      }

      header_.pixel_format = dynamic_cast<vimt3d_image_3d&>(*instance.object()[i]).image_base().pixel_format();

      unsigned sizeof_T = vil_pixel_format_sizeof_components(header_.pixel_format);

      { // Copy vimt3d_image_3d loader
        short vimt3d_image_3d_of_version;
        vsl_b_read(is, vimt3d_image_3d_of_version);
        switch (vimt3d_image_3d_of_version)
        {
          case 1:
          { // Copy of vil3d_image_view loader.
            std::ptrdiff_t dummy_step;
            vil_memory_chunk_sptr chunk;

            short vil3d_image_view_version;
            vsl_b_read(is, vil3d_image_view_version);
            switch (vil3d_image_view_version)
            {
             case 1:

              vsl_b_read(is, header_.ni);
              vsl_b_read(is, header_.nj);
              vsl_b_read(is, header_.nk);
              vsl_b_read(is, header_.nplanes);
              vsl_b_read(is, dummy_step /*istep*/);
              vsl_b_read(is, dummy_step /*jstep*/);
              vsl_b_read(is, dummy_step /*kstep*/);
              vsl_b_read(is, dummy_step /*pstep*/);
              if (header_.ni*header_.nj*header_.nk!=0)
              { // Copy of smart_ptr loader
                short vil_smart_ptr_version;
                vsl_b_read(is, vil_smart_ptr_version);
                switch (vil_smart_ptr_version)
                {
                 case 1:
                 case 2:
                  {
                    bool first_time; // true if the object is about to be loaded
                    vsl_b_read(is, first_time);
                    unsigned long id; // Unique serial number indentifying object
                    vsl_b_read(is, id);
                    if (!first_time || id == 0)
                    {
                      // We are in a v3i file there should only be one image, and it should not be a null ptr
                      std::ostringstream oss;
                      oss << "I/O ERROR: vimt3d_vil3d_v3i_image::vimt3d_vil3d_v3i_image()\n"
                          << "           Smart ptr De-serialisation failure\n";
                      is.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
                      vil_exception_warning(vil_exception_corrupt_image_file(
                        "vimt3d_vil3d_v3i_image constructor", "vimt3d_vil3d_v3i_image", "", oss.str()));
                      return;
                    }
                    { // Copy of vsl_b_read(vsl_b_istream &is, vil_memory_chunk*& p)
                      bool not_null_ptr;
                      vsl_b_read(is, not_null_ptr);
                      if (!not_null_ptr)
                      {
                        // We are in a v3i file there should not be a null ptr
                        std::ostringstream oss;
                        oss << "I/O ERROR: vimt3d_vil3d_v3i_image::vimt3d_vil3d_v3i_image()\n"
                            << "           Ptr read failure\n";
                        is.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
                        vil_exception_warning(vil_exception_corrupt_image_file(
                          "vimt3d_vil3d_v3i_image constructor", "vimt3d_vil3d_v3i_image", "", oss.str()));
                        return;
                      }
                      bool success = skip_b_read_vil_memory_chunk(is, sizeof_T);
                      if (!is)
                      {
                        vil_exception_warning(vil_exception_image_io(
                          "vimt3d_vil3d_v3i_image constructor", "vimt3d_vil3d_v3i_image", ""));
                        return;
                      }
                      if (!success)
                      {
                        // Give up trying to load just the header, and load the whole image.
                        load_full_image();
                        return;
                      }
                    }
                    break;
                  }
                 default:
                  std::ostringstream oss;
                  oss << "I/O ERROR: vimt3d_vil3d_v3i_image::vimt3d_vil3d_v3i_image()\n"
                      << "           Unknown vil_smart_ptr version number "<< vil_smart_ptr_version << '\n';
                  is.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
                  vil_exception_warning(vil_exception_invalid_version(
                    "vimt3d_vil3d_v3i_image constructor", "vimt3d_vil3d_v3i_image", "", oss.str()));
                  return;
                }
                vsl_b_read(is, dummy_step /*offset*/);
              }
              break;

             default:
              std::ostringstream oss;
              oss << "I/O ERROR: vimt3d_vil3d_v3i_image::vimt3d_vil3d_v3i_image()\n"
                  << "           Unknown vil3d_image_view version number "<< vil3d_image_view_version << '\n';
              is.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
              vil_exception_warning(vil_exception_invalid_version(
                "vimt3d_vil3d_v3i_image constructor", "vimt3d_vil3d_v3i_image", "", oss.str()));
              return;
            }
            vsl_b_read(is, header_.w2i);
          } // end of vil3d_image_view loader
          break;

          default:
          std::ostringstream oss;
          oss << "I/O ERROR: vimt3d_vil3d_v3i_image::vimt3d_vil3d_v3i_image()\n"
              << "           Unknown vimt3d_image_3d_of version number "<< vimt3d_image_3d_of_version << '\n';
          is.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
          vil_exception_warning(vil_exception_invalid_version("vimt3d_vil3d_v3i_image constructor", "vimt3d_vil3d_v3i_image", "", oss.str()));
          return;
        }
      } // End of  vimt3d_image_3d loader
    }

#endif
    break;

    default:
    std::ostringstream oss;
    oss << "I/O ERROR: vimt3d_vil3d_v3i_image::vimt3d_vil3d_v3i_image()\n"
        << "           Unknown version number "<< version << '\n';
    vil_exception_warning(vil_exception_invalid_version("vimt3d_vil3d_v3i_image constructor", "vimt3d_vil3d_v3i_image", "", oss.str()));
    return;
  }
}

//: Private constructor, use vil3d_save instead.
// This object takes ownership of the file, for writing.
vimt3d_vil3d_v3i_image::vimt3d_vil3d_v3i_image(std::unique_ptr<std::fstream> file, unsigned ni,
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
macro(VIL_PIXEL_FORMAT_DOUBLE , double )
#undef macro
   default:
    vil_exception_error(vil_exception_unsupported_pixel_format(
      format, "vimt3d_vil3d_v3i_image constructor"));
  }
}

vimt3d_vil3d_v3i_image::~vimt3d_vil3d_v3i_image()
{
  if (dirty_)
  {
    file_->seekp(0);
    vsl_b_ostream os(file_);

    vsl_b_write(os, vimt3d_vil3d_v3i_format::magic_number());

    constexpr short version = 1;
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
  return header_.nplanes;
}

//: Dimensions:  nplanes x ni x nj x nk.
// The number of pixels in each row.
unsigned vimt3d_vil3d_v3i_image::ni() const
{
  return header_.ni;
}

//: Dimensions:  nplanes x ni x nj x nk.
// The number of pixels in each column.
unsigned vimt3d_vil3d_v3i_image::nj() const
{
  return header_.nj;
}

//: Dimensions:  nplanes x ni x nj x nk.
// The number of slices per image.
unsigned vimt3d_vil3d_v3i_image::nk() const
{
  return header_.nk;
}

//: Pixel Format.
enum vil_pixel_format vimt3d_vil3d_v3i_image::pixel_format() const
{
  return header_.pixel_format;
}


//: Get the properties (of the first slice)
bool vimt3d_vil3d_v3i_image::get_property(char const *key, void * value) const
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
bool vimt3d_vil3d_v3i_image::set_voxel_size_mm(float si, float sj, float sk)
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

const vimt3d_transform_3d & vimt3d_vil3d_v3i_image::world2im() const
{
  return header_.w2i;
}

void vimt3d_vil3d_v3i_image::set_world2im(const vimt3d_transform_3d & tr)
{
  header_.w2i=tr;
  if (im_)
    im_->set_world2im(header_.w2i);
  dirty_ = true;
}


//: Create a read/write view of a copy of this data.
// Currently not yet implemented.
// \return 0 if unable to get view of correct size.
vil3d_image_view_base_sptr vimt3d_vil3d_v3i_image::get_copy_view(unsigned i0, unsigned ni,
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
macro(VIL_PIXEL_FORMAT_DOUBLE , double )
#undef macro
   default:
     vil_exception_warning(vil_exception_unsupported_pixel_format(
       view.pixel_format(), "vimt3d_vil3d_v3i_image::get_copy_view"));
    return nullptr;
  }
}

//: Create a read/write view of a copy of this data.
// \return 0 if unable to get view of correct size.
vil3d_image_view_base_sptr vimt3d_vil3d_v3i_image::get_view(unsigned i0, unsigned ni,
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
   case  F : { \
    const vil3d_image_view< T > &v = \
      static_cast<const vil3d_image_view< T > &>(view); \
      return new vil3d_image_view< T >(v.memory_chunk(), v.size() ? &v(i0,j0,k0) : 0, \
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
macro(VIL_PIXEL_FORMAT_DOUBLE , double )
#undef macro
   default:
     vil_exception_warning(vil_exception_unsupported_pixel_format(
       view.pixel_format(), "vimt3d_vil3d_v3i_image::get_view"));
    return nullptr;
  }
}


//: Set the contents of the volume.
bool vimt3d_vil3d_v3i_image::put_view(const vil3d_image_view_base& vv,
                                      unsigned i0, unsigned j0, unsigned k0)
{
  if (!im_)
    load_full_image();
  if (!im_) return false; // If load full image failed then im_ will remain null


  if (!view_fits(vv, i0, j0, k0))
  {
    vil_exception_warning(vil_exception_out_of_bounds("vimt3d_vil3d_v3i_image::put_view"));
    return false;
  }

  if (vv.pixel_format() != im_->image_base().pixel_format())
  {
    vil_exception_warning(vil_exception_pixel_formats_incompatible(
      vv.pixel_format(), im_->image_base().pixel_format(), "vimt3d_vil3d_v3i_image::put_view"));
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
    macro(VIL_PIXEL_FORMAT_DOUBLE , double )
#undef macro
   default:
     vil_exception_warning(vil_exception_unsupported_pixel_format(
       vv.pixel_format(), "vimt3d_vil3d_v3i_image::put_view"));
    return false;
  }
}
