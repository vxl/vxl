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
#include <vil3d/vil3d_image_resource.h>
#include <vcl_vector.h>
#include <vil/vil_open.h>
#include <vsl/vsl_binary_explicit_io.h>
#include <vcl_cassert.h>

// GIPL magic number
const unsigned GIPL_MAGIC1 = 719555000;
const unsigned GIPL_MAGIC2 = 4026526128U;

// GIPL header size
#define GIPL_HEADERSIZE 256

// GIPL filter types
#define GIPL_BINARY       1
#define GIPL_CHAR         7
#define GIPL_U_CHAR       8
#define GIPL_U_SHORT     15
#define GIPL_SHORT       16
#define GIPL_U_INT       31
#define GIPL_INT         32
#define GIPL_FLOAT       64
#define GIPL_DOUBLE      65
#define GIPL_C_SHORT    144
#define GIPL_C_INT      160
#define GIPL_C_FLOAT    192
#define GIPL_C_DOUBLE   193

// ORIENTATION DEFINITIONS (flag1)
#define GIPL_UNDEFINED_ORIENTATION   0
#define GIPL_UNDEFINED_PROJECTION    1
#define GIPL_AP_PROJECTION           2
#define GIPL_LATERAL_PROJECTION      3
#define GIPL_OBLIQUE_PROJECTION      4
#define GIPL_UNDEFINED_TOMO          8
#define GIPL_AXIAL                   9
#define GIPL_CORONAL                10
#define GIPL_SAGITTAL               11
#define GIPL_OBLIQUE_TOMO           12


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
    char c= a[i]; a[i] = a[i+3]; a[i+3] = c;
    c = a[i+1]; a[i+1] = a[i+2]; a[i+2] = c;
  }
}

inline void swap64_for_big_endian(char *a, unsigned n)
{
  for (unsigned i = 0; i < n * 8; i += 8)
  {
    char c= a[i]; a[i] = a[i+7]; a[i+7] = c;
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
  if (magic_number!=GIPL_MAGIC1 && magic_number!=GIPL_MAGIC2) return 0;
  else return new vil3d_gipl_image(is.as_pointer());
}


//: Make a "generic_image" on which put_section may be applied.
// The file may be opened immediately for writing so that a header can be written.
// The width/height etc are explicitly specified, so that file_format implementors
// know what they need to do...
vil3d_image_resource_sptr vil3d_gipl_format::make_output_image(const char* filename,
                                 unsigned ni, unsigned nj,
                                 unsigned nk, unsigned nplanes,
                                 enum vil_pixel_format format) const
{
  if (format != VIL_PIXEL_FORMAT_BOOL   && format != VIL_PIXEL_FORMAT_SBYTE &&
      format != VIL_PIXEL_FORMAT_BYTE   && format != VIL_PIXEL_FORMAT_UINT_16 &&
      format != VIL_PIXEL_FORMAT_INT_16 && format != VIL_PIXEL_FORMAT_UINT_32 &&
      format != VIL_PIXEL_FORMAT_FLOAT  && format != VIL_PIXEL_FORMAT_DOUBLE &&
      format != VIL_PIXEL_FORMAT_INT_32 )
  {
    vcl_cerr << "vil3d_gipl_format::make_output_image() WARNING\n"
             << "  Unable to deal with file format : " << format << vcl_endl;
    return 0;
  }

  // vil_smart_ptr<vil_stream> os = new vil_stream_fstream(filename,"w");

  vil_stream* os = vil_open(filename, "w");
  if (!os || !os->ok()) {
    vcl_cerr << __FILE__ ": Invalid stream for \"" << filename << "\"\n";
    return 0;
  }

  if (!os->ok()) return 0;

  return new vil3d_gipl_image(os, ni, nj, nk, nplanes, format);
}


vil3d_gipl_image::vil3d_gipl_image(vil_stream *is): is_(is)
{
  read_header(is);
  os_ = 0;
}

vil3d_gipl_image::~vil3d_gipl_image()
{
  // delete os_
  if (os_)
    os_->unref();
}

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
  case 32 : pixel_format_ = VIL_PIXEL_FORMAT_INT_32;  break;
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
    float* array = static_cast<float*>(value);
    // gipl stores data in mm
    array[0] = vox_width1_ / 1000.0f;
    array[1] = vox_width2_ / 1000.0f;
    array[2] = vox_width3_ / 1000.0f;
    return true;
  }

  if (vcl_strcmp(vil3d_property_origin_offset, key)==0)
  {
    float* array = static_cast<float*>(value);
    array[0] = (float)(dim1_ * 0.5);
    array[1] = (float)(dim2_ * 0.5);
    array[2] = (float)(dim3_ * 0.5);
    return true;
  }

  return false;
}

vil3d_gipl_image::vil3d_gipl_image(vil_stream* os,
                   unsigned ni,
                   unsigned nj,
                   unsigned nk,
                   unsigned nplanes,
                   enum vil_pixel_format format,
                   float vox_width1,
                   float vox_width2,
                   float vox_width3,
                   char orientation_flag,
                   double min_val,
                   double max_val,
                   double origin1,
                   double origin2,
                   double origin3,
                   float interslice_gap
                   ) : os_(os), dim1_(ni), dim2_(nj), dim3_(nk), nplanes_(nplanes),
                   pixel_format_(format), vox_width1_(vox_width1),
                   vox_width2_(vox_width2), vox_width3_(vox_width3),
                   orientation_flag_(orientation_flag),
                   min_val_(min_val), max_val_(max_val),
                   origin1_(origin1), origin2_(origin2),
                   origin3_(origin3), interslice_gap_(interslice_gap)
{
  os_->ref();

  write_header();
}

#if 0
vil3d_gipl_image::vil3d_gipl_image(vil_stream* os,
                                   unsigned ni,
                                   unsigned nj,
                                   unsigned nk,
                                   unsigned nplanes,
                                   enum vil_pixel_format format) : os_(os)
{
  os_->ref();
  dim1_ = ni;
  dim2_ = nj;
  dim3_ = nk;
  vox_width1_ = vox_width2_ = vox_width3_ = 1;
  pixel_format_ = format;
  write_header();
}
#endif // 0

// need to write out as big endian
inline void ConvertHostToMSB(char * ptr, int nbyte, int nelem=1)
{
  char temp;
  char *ptr1, *ptr2;

  int nbyte2 = nbyte/2;
  for (int n = 0; n < nelem; n++ ) {
    ptr1 = ptr;
    ptr2 = ptr1 + nbyte - 1;
    for (int i = 0; i < nbyte2; i++ ) {
      temp = *ptr1;
      *ptr1++ = *ptr2;
      *ptr2-- = temp;
    }
    ptr += nbyte;
  }
}


bool vil3d_gipl_image::write_header(void)
{
  os_->seek(0L);
  char buf[GIPL_HEADERSIZE];
  int temp;

  // image dimensions X Y Z T - 8 bytes (0 - 7)
  if (VXL_BIG_ENDIAN)
  {
    os_->write((char*)&dim1_,sizeof(vxl_uint_16));
    os_->write((char*)&dim2_,sizeof(vxl_uint_16));
    os_->write((char*)&dim3_,sizeof(vxl_uint_16));
    os_->write((char*)&nplanes_,sizeof(vxl_uint_16));
  }
  else
  {
    vcl_vector<vxl_byte> tempbuf(sizeof(vxl_uint_16));

    vcl_memcpy(&tempbuf[0], &dim1_, sizeof(vxl_uint_16));
    ConvertHostToMSB((char*)&tempbuf[0],sizeof(vxl_uint_16));
    os_->write((char*)&tempbuf[0],sizeof(vxl_uint_16));

    vcl_memcpy(&tempbuf[0], &dim2_, sizeof(vxl_uint_16));
    ConvertHostToMSB((char*)&tempbuf[0],sizeof(vxl_uint_16));
    os_->write((char*)&tempbuf[0],sizeof(vxl_uint_16));

    vcl_memcpy(&tempbuf[0], &dim3_, sizeof(vxl_uint_16));
    ConvertHostToMSB((char*)&tempbuf[0],sizeof(vxl_uint_16));
    os_->write((char*)&tempbuf[0],sizeof(vxl_uint_16));

    vcl_memcpy(&tempbuf[0], &nplanes_, sizeof(vxl_uint_16));
    ConvertHostToMSB((char*)&tempbuf[0],sizeof(vxl_uint_16));
    os_->write((char*)&tempbuf[0],sizeof(vxl_uint_16));
  }

  // file format - 2 bytes (8 - 9)
  switch (pixel_format_)
  {
  case VIL_PIXEL_FORMAT_BOOL   : temp = GIPL_BINARY;  break;
  case VIL_PIXEL_FORMAT_SBYTE  : temp = GIPL_CHAR;  break;
  case VIL_PIXEL_FORMAT_BYTE   : temp = GIPL_U_CHAR;  break;
  case VIL_PIXEL_FORMAT_UINT_16: temp = GIPL_U_SHORT;  break;
  case VIL_PIXEL_FORMAT_INT_16 : temp = GIPL_SHORT;  break;
  case VIL_PIXEL_FORMAT_UINT_32: temp = GIPL_U_INT;  break;
  case VIL_PIXEL_FORMAT_INT_32 : temp = GIPL_INT;    break;
  case VIL_PIXEL_FORMAT_FLOAT  : temp = GIPL_FLOAT;  break;
  case VIL_PIXEL_FORMAT_DOUBLE : temp = GIPL_DOUBLE;  break;
  case 144: // C.Short We don't want to support complex types.
  case 160: // C.Int   Could maybe reimplement them as a 2-plane image
  case 192: // C.Float
  case 193: // C.Double
  default :
    vcl_cerr << "vil3d_gipl_format::write_header() WARNING\n"
             << "  Unable to deal with file format : " << pixel_format_ << vcl_endl;
    return false;
  }
  if (VXL_BIG_ENDIAN)
    os_->write((char*)&temp,sizeof(vxl_uint_16));
  else
  {
    vcl_vector<vxl_byte> tempbuf(sizeof(vxl_uint_16));

    vcl_memcpy(&tempbuf[0], &temp, sizeof(vxl_uint_16));
    ConvertHostToMSB((char*)&tempbuf[0],sizeof(vxl_uint_16));
    os_->write((char*)&tempbuf[0],sizeof(vxl_uint_16));
  }

  // voxel dimensions X Y Z T - default to x,y,z=1,t=0. 16 bytes (10 - 25)
  if (VXL_BIG_ENDIAN)
  {
    os_->write((char*)&vox_width1_,sizeof(float));
    os_->write((char*)&vox_width2_,sizeof(float));
    os_->write((char*)&vox_width3_,sizeof(float));
  }
  else
  {
    vcl_vector<float> tempbuf(sizeof(float));

    vcl_memcpy(&tempbuf[0],&vox_width1_,sizeof(float));
    ConvertHostToMSB((char*)&tempbuf[0],sizeof(float));
    os_->write((char*)&tempbuf[0],sizeof(float));

    vcl_memcpy(&tempbuf[0],&vox_width2_,sizeof(float));
    ConvertHostToMSB((char*)&tempbuf[0],sizeof(float));
    os_->write((char*)&tempbuf[0],sizeof(float));

    vcl_memcpy(&tempbuf[0],&vox_width3_,sizeof(float));
    ConvertHostToMSB((char*)&tempbuf[0],sizeof(float));
    os_->write((char*)&tempbuf[0],sizeof(float));
  }
  float temp_float = 0.0;
  os_->write((char*)&temp_float,sizeof(float)); // write out 0 for size in t dimension

  // Patient/Text field next 80 characters - 80 bytes (26 - 105)
  int i;
  for (i=0;i<80;i++)
    buf[i]=' ';
  os_->write((char*)&buf,80);

  // float matrix[20]. No description. 80 bytes (106 - 185)
  for (i=0;i<20;i++)
    os_->write((char*)&temp_float,sizeof(float));

  // orientation flag (below). 1 byte (186)
  if (VXL_BIG_ENDIAN)
    os_->write((char*)&orientation_flag_,sizeof(vxl_byte));
  else
  {
    vcl_vector<vxl_byte> tempbuf(sizeof(vxl_byte));
    vcl_memcpy(&tempbuf[0],&orientation_flag_,sizeof(vxl_byte));
    ConvertHostToMSB((char*)&tempbuf[0],sizeof(vxl_byte));
    os_->write(&tempbuf[0],sizeof(vxl_byte));
  }

  // flag2. No description. 1 byte (187)
  temp = 0; os_->write((char*)&temp,sizeof(vxl_byte));

  // min. 8 bytes (188 - 195)
  if (VXL_BIG_ENDIAN)
    os_->write((char*)&min_val_,sizeof(double));
  else
  {
    vcl_vector<double> tempbuf(sizeof(double));
    vcl_memcpy(&tempbuf[0],&min_val_,sizeof(double));
    ConvertHostToMSB((char*)&tempbuf[0],sizeof(double));
    os_->write((char*)&tempbuf[0],sizeof(double));
  }


  // max. 8 bytes (196 - 203)
  if (VXL_BIG_ENDIAN)
    os_->write((char*)&max_val_,sizeof(double));
  else
  {
    vcl_vector<double> tempbuf(sizeof(double));
    vcl_memcpy(&tempbuf[0],&max_val_,sizeof(double));
    ConvertHostToMSB((char*)&tempbuf[0],sizeof(double));
    os_->write((char*)&tempbuf[0],sizeof(double));
  }

  if (VXL_BIG_ENDIAN)
  {
    // Origin offsets for X Y Z T from top left. 32 bytes (204 - 235)
    os_->write((char*)&origin1_,sizeof(double));
    os_->write((char*)&origin2_,sizeof(double));
    os_->write((char*)&origin3_,sizeof(double));
  }
  else
  {
    vcl_vector<double> tempbuf(sizeof(double));

    vcl_memcpy(&tempbuf[0],&origin1_,sizeof(double));
    ConvertHostToMSB((char*)&tempbuf[0],sizeof(double));
    os_->write((char*)&tempbuf[0],sizeof(double));

    vcl_memcpy(&tempbuf[0],&origin2_,sizeof(double));
    ConvertHostToMSB((char*)&tempbuf[0],sizeof(double));
    os_->write((char*)&tempbuf[0],sizeof(double));

    vcl_memcpy(&tempbuf[0],&origin3_,sizeof(double));
    ConvertHostToMSB((char*)&tempbuf[0],sizeof(double));
    os_->write((char*)&tempbuf[0],sizeof(double));
  }
  double temp_double = 0.0;
  os_->write((char*)&temp_double,sizeof(double)); // origin for t dimension is always 0

  // pixval_offset. 4 bytes (236 - 239)
  temp_float = 0.0;
  os_->write((char*)&temp_float,sizeof(float));

  // pixval_cal. 4 bytes (240 - 243)
  os_->write((char*)&temp_float,sizeof(float));

  // Inter-slice gap 4 bytes (244 - 247)
  if (VXL_BIG_ENDIAN)
    os_->write((char*)&interslice_gap_,sizeof(float));
  else
  {
    vcl_vector<float> tempbuf(sizeof(float));
    vcl_memcpy(&tempbuf[0],&interslice_gap_,sizeof(float));
    ConvertHostToMSB((char*)&tempbuf[0],sizeof(float));
    os_->write(&tempbuf[0],sizeof(float));
  }

  // User defined field. 4 bytes (248 - 251)
  temp_float = 0.0;
  os_->write((char*)&temp_float,sizeof(float));

  // Magic number. 4 bytes (252 - 255)
  vxl_uint_32 temp_magic1 = GIPL_MAGIC1;

  if (VXL_BIG_ENDIAN)
    os_->write((char*)&temp_magic1,sizeof(vxl_uint_32));
  else
  {
    ConvertHostToMSB((char*)&temp_magic1,sizeof(vxl_uint_32));
    os_->write((char*)&temp_magic1,sizeof(vxl_uint_32));
  }

  start_of_data_ = os_->tell();
  return true;
}

//: Set the contents of the volume.
bool vil3d_gipl_image::put_view(const vil3d_image_view_base& view,
                unsigned i0=0, unsigned j0=0, unsigned k0=0)
{
  if (!view_fits(view, i0, j0, k0))
  {
    vcl_cerr << "ERROR: " << __FILE__ << ":\n view does not fit\n";
    return false;
  }

  const vil3d_image_view<bool>* bool_im=0;
  const vil3d_image_view<vxl_sbyte>* sbyte_im=0;
  const vil3d_image_view<vxl_byte>* byte_im=0;
  const vil3d_image_view<vxl_uint_16>* uint_16_im=0;
  const vil3d_image_view<vxl_int_16>* int_16_im=0;
  const vil3d_image_view<vxl_uint_32>* uint_32_im=0;
  const vil3d_image_view<vxl_int_32>* int_32_im=0;
  const vil3d_image_view<float>* float_im=0;
  const vil3d_image_view<double>* double_im=0;

  unsigned bytes_per_pixel=0;

  if (view.pixel_format() == VIL_PIXEL_FORMAT_BOOL)
    bool_im = &static_cast<const vil3d_image_view<bool>& >(view);
  else if (view.pixel_format() == VIL_PIXEL_FORMAT_SBYTE)
  {
    sbyte_im = &static_cast<const vil3d_image_view<vxl_sbyte>& >(view);
    bytes_per_pixel=sizeof(vxl_sbyte);
  }
  else if (view.pixel_format() == VIL_PIXEL_FORMAT_BYTE)
  {
    byte_im = &static_cast<const vil3d_image_view<vxl_byte>& >(view);
    bytes_per_pixel=sizeof(vxl_byte);
  }
  else if (view.pixel_format() == VIL_PIXEL_FORMAT_UINT_16)
  {
    uint_16_im = &static_cast<const vil3d_image_view<vxl_uint_16>& >(view);
    bytes_per_pixel=sizeof(vxl_uint_16);
  }
  else if (view.pixel_format() == VIL_PIXEL_FORMAT_INT_16)
  {
    int_16_im = &static_cast<const vil3d_image_view<vxl_int_16>& >(view);
    bytes_per_pixel=sizeof(vxl_int_16);
  }
  else if (view.pixel_format() == VIL_PIXEL_FORMAT_UINT_32)
  {
    uint_32_im = &static_cast<const vil3d_image_view<vxl_uint_32>& >(view);
    bytes_per_pixel=sizeof(vxl_uint_32);
  }
  else if (view.pixel_format() == VIL_PIXEL_FORMAT_INT_32)
  {
    int_32_im = &static_cast<const vil3d_image_view<vxl_int_32>& >(view);
    bytes_per_pixel=sizeof(vxl_int_32);
  }
  else if (view.pixel_format() == VIL_PIXEL_FORMAT_FLOAT)
  {
    float_im = &static_cast<const vil3d_image_view<float>& >(view);
    bytes_per_pixel=sizeof(float);
  }
  else if (view.pixel_format() == VIL_PIXEL_FORMAT_DOUBLE)
  {
    double_im = &static_cast<const vil3d_image_view<double>& >(view);
    bytes_per_pixel=sizeof(double);
  }
  else
  {
    vcl_cerr << "ERROR: " << __FILE__ << ":\n Do not support putting "
             << view.is_a() << " views into gipl image_resource objects\n";
    return false;
  }

  // write out actual image
  vil_streampos byte_start = start_of_data_ + (k0*dim2_ + j0 * dim1_ + i0) * bytes_per_pixel;
  unsigned byte_width = dim1_ * bytes_per_pixel;
  unsigned byte_out_width = view.ni() * bytes_per_pixel;

  if (view.pixel_format() == VIL_PIXEL_FORMAT_BOOL)
  {
    vcl_cerr << "GIPL writer for bool format is not yet implemented" ;
    return false;
  }

  else if (view.pixel_format() == VIL_PIXEL_FORMAT_SBYTE)
  {
    assert(sbyte_im!=0);
      os_->seek(byte_start);
      for (unsigned k = 0; k < view.nk(); ++k)
      {
        for (unsigned j = 0; j < view.nj(); ++j)
        {
          os_->write(sbyte_im->origin_ptr()+(k*view.nj()*view.ni())+ (j * view.ni()), byte_out_width);
          //byte_start += byte_width;
          //os_->seek(byte_start);
        }
      }
    return true;
  }

  else if (view.pixel_format() == VIL_PIXEL_FORMAT_BYTE)
  {
    assert(byte_im!=0);
    os_->seek(byte_start);
    for (unsigned k = 0; k < view.nk(); ++k)
    {
      for (unsigned j = 0; j < view.nj(); ++j)
      {
        os_->write(byte_im->origin_ptr()+(k*view.nj()*view.ni())+ (j * view.ni()), byte_out_width);
        //byte_start += byte_width;
        //os_->seek(byte_start);
      }
    }
    return true;
  }

  else if (view.pixel_format() == VIL_PIXEL_FORMAT_UINT_16)
  {
    if (VXL_BIG_ENDIAN)
    {
      assert(uint_16_im!=0);
      os_->seek(byte_start);
      for (unsigned k = 0; k < view.nk(); ++k)
      {
        for (unsigned j = 0; j < view.nj(); ++j)
        {
          os_->write(uint_16_im->origin_ptr()+(k*view.nj()*view.ni())+ (j * view.ni()), byte_out_width);
          //byte_start += byte_width;
          //os_->seek(byte_start);
        }
      }
    }
    else
    {
      // Little endian host; must convert words to have MSB first.
      //
      // Convert line by line to avoid duplicating a potentially large image.
      vcl_vector<vxl_byte> tempbuf(byte_out_width);
      assert(uint_16_im!=0);
      os_->seek(byte_start);
      for (unsigned k = 0; k < view.nk(); ++k)
      {
        for (unsigned j = 0; j < view.nj(); ++j)
        {
          vcl_memcpy(&tempbuf[0], uint_16_im->origin_ptr()+(k*view.nj()*view.ni())+ (j * view.ni()), byte_out_width);
          ConvertHostToMSB((char*)&tempbuf[0], sizeof(vxl_uint_16), view.ni());
          os_->write(&tempbuf[0], byte_out_width);
          //byte_start += byte_width;
          //os_->seek(byte_start);
        }
      }
    }

    return true;
  }

  else if (view.pixel_format() == VIL_PIXEL_FORMAT_INT_16)
  {
    if (VXL_BIG_ENDIAN)
    {
      assert(int_16_im!=0);
      os_->seek(byte_start);
      for (unsigned k = 0; k < view.nk(); ++k)
      {
        for (unsigned j = 0; j < view.nj(); ++j)
        {
          os_->write(int_16_im->origin_ptr()+(k*view.nj()*view.ni())+ (j * view.ni()), byte_out_width);
          //byte_start += byte_width;
          //os_->seek(byte_start);
        }
      }
    }
    else
    {
      // Little endian host; must convert words to have MSB first.
      //
      // Convert line by line to avoid duplicating a potentially large image.
      vcl_vector<vxl_byte> tempbuf(byte_out_width);
      assert(int_16_im!=0);
      os_->seek(byte_start);
      for (unsigned k = 0; k < view.nk(); ++k)
      {
        for (unsigned j = 0; j < view.nj(); ++j)
        {
          vcl_memcpy(&tempbuf[0], int_16_im->origin_ptr()+(k*view.nj()*view.ni())+ (j * view.ni()), byte_out_width);
          ConvertHostToMSB((char*)&tempbuf[0], sizeof(vxl_int_16), view.ni());
          os_->write(&tempbuf[0], byte_out_width);
          //byte_start += byte_width;
          //os_->seek(byte_start);
        }
      }
    }

    return true;
  }

  else if (view.pixel_format() == VIL_PIXEL_FORMAT_UINT_32)
  {
    if (VXL_BIG_ENDIAN)
    {
      assert(uint_32_im!=0);
      os_->seek(byte_start);
      for (unsigned k = 0; k < view.nk(); ++k)
      {
        for (unsigned j = 0; j < view.nj(); ++j)
        {
          os_->write(uint_32_im->origin_ptr()+(k*view.nj()*view.ni())+ (j * view.ni()), byte_out_width);
          //byte_start += byte_width;
          //os_->seek(byte_start);
        }
      }
    }
    else
    {
      // Little endian host; must convert words to have MSB first.
      //
      // Convert line by line to avoid duplicating a potentially large image.
      vcl_vector<vxl_byte> tempbuf(byte_out_width);
      assert(uint_32_im!=0);
      os_->seek(byte_start);
      for (unsigned k = 0; k < view.nk(); ++k)
      {
        for (unsigned j = 0; j < view.nj(); ++j)
        {
          vcl_memcpy(&tempbuf[0], uint_32_im->origin_ptr()+(k*view.nj()*view.ni())+ (j * view.ni()), byte_out_width);
          ConvertHostToMSB((char*)&tempbuf[0], sizeof(vxl_uint_32), view.ni());
          os_->write(&tempbuf[0], byte_out_width);
          //byte_start += byte_width;
          //os_->seek(byte_start);
        }
      }
    }

    return true;
  }

  else if (view.pixel_format() == VIL_PIXEL_FORMAT_INT_32)
  {
    if (VXL_BIG_ENDIAN)
    {
      assert(int_32_im!=0);
      os_->seek(byte_start);
      for (unsigned k = 0; k < view.nk(); ++k)
      {
        for (unsigned j = 0; j < view.nj(); ++j)
        {
          os_->write(int_32_im->origin_ptr()+(k*view.nj()*view.ni())+ (j * view.ni()), byte_out_width);
          //byte_start += byte_width;
          //os_->seek(byte_start);
        }
      }
    }
    else
    {
      // Little endian host; must convert words to have MSB first.
      //
      // Convert line by line to avoid duplicating a potentially large image.
      vcl_vector<vxl_byte> tempbuf(byte_out_width);
      assert(int_32_im!=0);
      os_->seek(byte_start);
      for (unsigned k = 0; k < view.nk(); ++k)
      {
        for (unsigned j = 0; j < view.nj(); ++j)
        {
          vcl_memcpy(&tempbuf[0], int_32_im->origin_ptr()+(k*view.nj()*view.ni())+ (j * view.ni()), byte_out_width);
          ConvertHostToMSB((char*)&tempbuf[0], sizeof(vxl_int_32), view.ni());
          os_->write(&tempbuf[0], byte_out_width);
          //byte_start += byte_width;
          //os_->seek(byte_start);
        }
      }
    }

    return true;
  }

  else if (view.pixel_format() == VIL_PIXEL_FORMAT_FLOAT)
  {
    if (VXL_BIG_ENDIAN)
    {
      assert(float_im!=0);
      os_->seek(byte_start);
      for (unsigned k = 0; k < view.nk(); ++k)
      {
        for (unsigned j = 0; j < view.nj(); ++j)
        {
          os_->write(float_im->origin_ptr()+(k*view.nj()*view.ni())+ (j * view.ni()), byte_out_width);
          //byte_start += byte_width;
          //os_->seek(byte_start);
        }
      }
    }
    else
    {
      // Little endian host; must convert words to have MSB first.
      //
      // Convert line by line to avoid duplicating a potentially large image.
      vcl_vector<vxl_byte> tempbuf(byte_out_width);
      assert(float_im!=0);
      os_->seek(byte_start);
      for (unsigned k = 0; k < view.nk(); ++k)
      {
        for (unsigned j = 0; j < view.nj(); ++j)
        {
          vcl_memcpy(&tempbuf[0], float_im->origin_ptr()+(k*view.nj()*view.ni())+ (j * view.ni()), byte_out_width);
          ConvertHostToMSB((char*)&tempbuf[0], sizeof(float), view.ni());
          os_->write(&tempbuf[0], byte_out_width);
          //byte_start += byte_width;
          //os_->seek(byte_start);
        }
      }
    }

    return true;
  }

  else if (view.pixel_format() == VIL_PIXEL_FORMAT_DOUBLE)
  {
    if (VXL_BIG_ENDIAN)
    {
      assert(double_im!=0);
      os_->seek(byte_start);
      for (unsigned k = 0; k < view.nk(); ++k)
      {
        for (unsigned j = 0; j < view.nj(); ++j)
        {
          os_->write(double_im->origin_ptr()+(k*view.nj()*view.ni())+ (j * view.ni()), byte_out_width);
          //byte_start += byte_width;
          //os_->seek(byte_start);
        }
      }
    }
    else
    {
      // Little endian host; must convert words to have MSB first.
      //
      // Convert line by line to avoid duplicating a potentially large image.
      vcl_vector<vxl_byte> tempbuf(byte_out_width);
      assert(double_im!=0);
      os_->seek(byte_start);
      for (unsigned k = 0; k < view.nk(); ++k)
      {
        for (unsigned j = 0; j < view.nj(); ++j)
        {
          vcl_memcpy(&tempbuf[0], double_im->origin_ptr()+(k*view.nj()*view.ni())+ (j * view.ni()), byte_out_width);
          ConvertHostToMSB((char*)&tempbuf[0], sizeof(double), view.ni());
          os_->write(&tempbuf[0], byte_out_width);
          //byte_start += byte_width;
          //os_->seek(byte_start);
        }
      }
    }

    return true;
  }
  else
  {
    vcl_cerr << "ERROR: " << __FILE__ << ":\n Do not support putting "
             << view.is_a() << " views into gipl image_resource objects\n";
    return false;
  }
}
