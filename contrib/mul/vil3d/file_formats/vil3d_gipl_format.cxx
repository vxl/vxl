// This is mul/vil3d/file_formats/vil3d_gipl_format.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \brief Reader/Writer for GIPL format images.
// \author Tim Cootes - Manchester

#include "vil3d_gipl_format.h"
#include <vcl_cstdlib.h> // for vcl_abort()
#include <vcl_cstring.h> // for vcl_strcmp()
#include <vil/vil_stream_read.h>
#include <vil/vil_stream_fstream.h>
#include <vil3d/vil3d_image_view.h>
#include <vil3d/vil3d_new.h>
#include <vil3d/vil3d_property.h>

// GIPL magic number
const unsigned GIPL_MAGIC1 = 719555000;
const unsigned GIPL_MAGIC2 = 4026526128U;

// GIPL header size
#define GIPL_HEADERSIZE   256

// GIPL filter types
#define GIPL_BINARY       1
#define GIPL_CHAR         7
#define GIPL_U_CHAR       8
#define GIPL_SHORT        15
#define GIPL_U_SHORT      16
#define GIPL_U_INT        31
#define GIPL_INT          32
#define GIPL_FLOAT        64
#define GIPL_DOUBLE       65
#define GIPL_C_SHORT      144
#define GIPL_C_INT        160
#define GIPL_C_FLOAT      192
#define GIPL_C_DOUBLE     193


#if VXL_LITTLE_ENDIAN
inline void swap16_for_big_endian(char *a, unsigned n)
{
  for (unsigned i = 0; i < n * 2; i += 2)
  {
    char c = a[i]; a[i] = a[i+1]; a[i+1] = c;
  }
}

inline void swap32_for_big_endian(char *a, unsigned n)
{
  for (unsigned i = 0; i < n * 4; i += 4)
  {
    char c = a[i];   a[i] =   a[i+3]; a[i+3] = c;
         c = a[i+1]; a[i+1] = a[i+2]; a[i+2] = c;
  }
}

inline void swap64_for_big_endian(char *a, unsigned n)
{
  for (unsigned i = 0; i < n * 8; i += 8)
  {
    char c = a[i];   a[i]   = a[i+7]; a[i+7] = c;
         c = a[i+1]; a[i+1] = a[i+6]; a[i+6] = c;
         c = a[i+2]; a[i+2] = a[i+5]; a[i+5] = c;
         c = a[i+3]; a[i+3] = a[i+4]; a[i+4] = c;
  }
}
#endif //VXL_LITTLE_ENDIAN


vil3d_gipl_format::vil3d_gipl_format() {}

// The destructor must be virtual so that the memory chunk is destroyed.
vil3d_gipl_format::~vil3d_gipl_format()
{
}


vil3d_image_resource_sptr vil3d_gipl_format::make_input_image(const char *filename) const
{
  vil_smart_ptr<vil_stream> is = new vil_stream_fstream(filename,"r");
  if (!is->ok()) return 0;

  is->seek(252);
  unsigned magic_number = vil_stream_read_big_endian_uint_32(is.as_pointer());
  if (!(magic_number==GIPL_MAGIC1 || magic_number==GIPL_MAGIC2)) return 0;

  return new vil3d_gipl_image(is.as_pointer());
}


//: Make a "generic_image" on which put_section may be applied.
// The file may be opened immediately for writing so that a header can be written.
// The width/height etc are explicitly specified, so that file_format implementors
// know what they need to do...
vil3d_image_resource_sptr vil3d_gipl_format::make_output_image
                   (const char* /*filename*/, unsigned /*ni*/, unsigned /*nj*/,
                    unsigned /*nk*/, unsigned /*nplanes*/, enum vil_pixel_format) const
{
  vcl_cerr <<"vil3d_gipl_format::make_output_image() NYI\n";
  vcl_abort();
  return 0;
}


vil3d_gipl_image::vil3d_gipl_image(vil_stream *is): is_(is)
{
  read_header(is);
}

vil3d_gipl_image::~vil3d_gipl_image() {}

  //: Dimensions:  nplanes x ni x nj x nk.
  // This concept is treated as a synonym to components.
unsigned vil3d_gipl_image::nplanes() const
{
  return 1;
}
  //: Dimensions:  nplanes x ni x nj x nk.
  // The number of pixels in each row.
unsigned vil3d_gipl_image::ni() const
{
  return dim1_;
}
  //: Dimensions:  nplanes x ni x nj x nk.
  // The number of pixels in each column.
unsigned vil3d_gipl_image::nj() const
{
  return dim2_;
}
  //: Dimensions:  nplanes x ni x nj x nk.
  // The number of slices per image.
unsigned vil3d_gipl_image::nk() const
{
  return dim3_;
}

  //: Pixel Format.
enum vil_pixel_format vil3d_gipl_image::pixel_format() const
{
  return pixel_format_;
}


//: Read header from given stream if possible
bool vil3d_gipl_image::read_header(vil_stream *is)
{
  // Return to start
  is->seek(0);
  dim1_ = vil_stream_read_big_endian_uint_16(is);
  dim2_ = vil_stream_read_big_endian_uint_16(is);
  dim3_ = vil_stream_read_big_endian_uint_16(is);

  is->seek(8);

  unsigned short gipl_pixel_type = vil_stream_read_big_endian_uint_16(is);

  switch (gipl_pixel_type)
  {
   case 1  : pixel_format_ = VIL_PIXEL_FORMAT_BOOL;    break;
   case 7  : pixel_format_ = VIL_PIXEL_FORMAT_SBYTE;   break;
   case 8  : pixel_format_ = VIL_PIXEL_FORMAT_BYTE;    break;
   case 15 : pixel_format_ = VIL_PIXEL_FORMAT_UINT_16; break;
   case 16 : pixel_format_ = VIL_PIXEL_FORMAT_INT_16;  break;
   case 31 : pixel_format_ = VIL_PIXEL_FORMAT_UINT_32; break;
   case 32 : pixel_format_ = VIL_PIXEL_FORMAT_INT_16;  break;
   case 64 : pixel_format_ = VIL_PIXEL_FORMAT_FLOAT;   break;
   case 65 : pixel_format_ = VIL_PIXEL_FORMAT_DOUBLE;  break;
   case 144: // C.Short I don't want to support complex types.
   case 160: // C.Int   Could maybe reimplement them as a 2-plane image
   case 192: // C.Float
   case 193: // C.Double
   default : pixel_format_ = VIL_PIXEL_FORMAT_UNKNOWN;
  }

  vox_width1_ = vil_stream_read_big_endian_float(is);
  vox_width2_ = vil_stream_read_big_endian_float(is);
  vox_width3_ = vil_stream_read_big_endian_float(is);
  // vcl_cout<<"Voxel widths: "<<vox_width1<<" x "<<vox_width2<<" x "<<vox_width3<<vcl_endl;

  return pixel_format_ != VIL_PIXEL_FORMAT_UNKNOWN;
}


//: Get some or all of the volume.
vil3d_image_view_base_sptr vil3d_gipl_image::get_copy_view(
  unsigned i0, unsigned ni, unsigned j0, unsigned nj,
  unsigned k0, unsigned nk) const
{
  if (i0+ni > this->ni() || j0+nj > this->nj() || k0+nk > this->nk()) return 0;

#define macro(type) \
  vil3d_image_view< type > im = \
    vil3d_new_image_view_plane_k_j_i(ni, nj, nk, 1, type()); \
  for (unsigned k=0; k<nk; ++k) \
  { \
    if (ni == this->ni()) \
    { \
      is_->seek(GIPL_HEADERSIZE + ((k+k0)*this->nj()*ni + j0*ni) * sizeof(type)); \
      is_->read(&im(0,0,k), nj*ni * sizeof(type)); \
    } \
    else \
      for (unsigned j=0; j<nj; ++j) \
      { \
        is_->seek(GIPL_HEADERSIZE + ((k+k0)*this->nj()*this->ni() + (j+j0)*this->ni() + i0) * sizeof(type)); \
        is_->read(&im(0,j,k), ni * sizeof(type)); \
      } \
  }


  switch (pixel_format())
  {
    case VIL_PIXEL_FORMAT_SBYTE:
    {
      macro(vxl_sbyte);
      return new vil3d_image_view<vxl_sbyte>(im);
    }
    case VIL_PIXEL_FORMAT_BYTE:
    {
      macro(vxl_byte);
      return new vil3d_image_view<vxl_byte>(im);
    }
    case VIL_PIXEL_FORMAT_INT_16:
    {
      macro(vxl_int_16);
#if VXL_LITTLE_ENDIAN
      swap16_for_big_endian((char *)(im.origin_ptr()), ni*nj*nk);
#endif //VXL_LITTLE_ENDIAN
      return new vil3d_image_view<vxl_int_16>(im);
    }
    case VIL_PIXEL_FORMAT_UINT_16:
    {
      macro(vxl_uint_16);
#if VXL_LITTLE_ENDIAN
      swap16_for_big_endian((char *)(im.origin_ptr()), ni*nj*nk);
#endif //VXL_LITTLE_ENDIAN
      return new vil3d_image_view<vxl_uint_16>(im);
    }
    case VIL_PIXEL_FORMAT_UINT_32:
    {
      macro(vxl_uint_32);
#if VXL_LITTLE_ENDIAN
      swap32_for_big_endian((char *)(im.origin_ptr()), ni*nj*nk);
#endif //VXL_LITTLE_ENDIAN
      return new vil3d_image_view<vxl_uint_32>(im);
    }
    case VIL_PIXEL_FORMAT_INT_32:
    {
      macro(vxl_int_32);
#if VXL_LITTLE_ENDIAN
      swap32_for_big_endian((char *)(im.origin_ptr()), ni*nj*nk);
#endif //VXL_LITTLE_ENDIAN
      return new vil3d_image_view<vxl_int_32>(im);
    }
    case VIL_PIXEL_FORMAT_FLOAT:
    {
      macro(float);
#if VXL_LITTLE_ENDIAN
      swap32_for_big_endian((char *)(im.origin_ptr()), ni*nj*nk);
#endif //VXL_LITTLE_ENDIAN
      return new vil3d_image_view<float>(im);
    }
    case VIL_PIXEL_FORMAT_DOUBLE:
    {
      macro(double);
#if VXL_LITTLE_ENDIAN
      swap64_for_big_endian((char *)(im.origin_ptr()), ni*nj*nk);
#endif //VXL_LITTLE_ENDIAN
      return new vil3d_image_view<double>(im);
    }
    case VIL_PIXEL_FORMAT_BOOL:
      vcl_cout<<"ERROR: vil3d_gipl_format::get_image_data()"
              <<pixel_format() << " pixel type not yet implemented\n";
      return 0;
    default:
      vcl_cout<<"ERROR: vil3d_gipl_format::get_image_data()\n"
              <<"Can't deal with pixel type " << pixel_format() << vcl_endl;
      return 0;
  }
}


//: Get the properties (of the first slice)
bool vil3d_gipl_image::get_property(char const *key, void * value) const
{
  if (vcl_strcmp(vil3d_property_voxel_size, key)==0)
  {
    float* array =  static_cast<float*>(value);
    // gipl stores data in mm
    array[0] = vox_width1_ / 1000.0f;
    array[1] = vox_width2_ / 1000.0f;
    array[2] = vox_width3_ / 1000.0f;
    return true;
  }

  if (vcl_strcmp(vil3d_property_origin_offset, key)==0)
  {
    float* array =  static_cast<float*>(value);
    array[0] = (float)(dim1_ * 0.5);
    array[1] = (float)(dim2_ * 0.5);
    array[2] = (float)(dim3_ * 0.5);
    return true;
  }

  return false;
}

//: Set the contents of the volume.
bool vil3d_gipl_image::put_view(const vil3d_image_view_base& /*vv*/,
                                unsigned /*i0*/, unsigned /*j0*/, unsigned /*k0*/)
{
  vcl_cerr << "ERROR: vil3d_gipl_image::put_view NYI\n\n";
  return false;
}
