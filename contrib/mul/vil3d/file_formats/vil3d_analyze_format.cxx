// This is mul/vil3d/file_formats/vil3d_analyze_format.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \brief Reader/Writer for analyze format images.
// \author Tim Cootes - Manchester
// This file contains classes for reading and writing analyze format images
// Three key components are
//   vil3d_analyze_header: Structure to contain header information
//   vil3d_analyze_image: Resource object which interfaces to the file,
//                        allowing reading and writing via the get_copy_image()
//                        and put_image() functions
//   vil3d_analyze_format: Object to create an appropriate vil3d_analyze_image
//
//   The main work of loading and saving happens in vil3d_analyze_image

#include <cstring>
#include <cstdio>
#include <iostream>
#include <cstddef>
#include "vil3d_analyze_format.h"
#include <vcl_compiler.h>
#include <vil/vil_stream_fstream.h>
#include <vil3d/vil3d_image_view.h>
#include <vil3d/vil3d_new.h>
#include <vil3d/vil3d_copy.h>
#include <vil3d/vil3d_property.h>
#include <vil3d/vil3d_image_resource.h>
#include <vsl/vsl_binary_explicit_io.h>
#include <vsl/vsl_indent.h>

// ---- Utility functions for dealing with byte ordering ----
// (Note: The use of this is currently guessed - need to check format)
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

/*=========== Analyze header stuff ==========*/
#define RD_AH_MAXHEADERSIZE         0xFFFF

const int ObligatorySize = sizeof(vil3d_analyze_header::Key) +
                           sizeof(vil3d_analyze_header::Dimensions),
          OptionalSize   = ObligatorySize +
                           sizeof(vil3d_analyze_header::History);


void vil3d_analyze_header::Key::reset()
{
  std::memset(this, (char)0, sizeof(*this));

  // obligatory fields
  sizeof_hdr = OptionalSize;
  extents = 16384;
  regular = 'r';
}


void vil3d_analyze_header::Dimensions::reset()
{
  std::memset(this, (char)0, sizeof(*this));
}


void vil3d_analyze_header::History::reset()
{
  std::memset(this, (char)0, sizeof(*this));
  std::sprintf(descrip," VXL generated file, some data fields may be missing");
}


void vil3d_analyze_header::reset()
{
  key.reset();
  dim.reset();
  history.reset();
}

// This function centralises the compiler warning concerning "const cast":
static inline void do_write(std::ofstream& bfs, const void* data, unsigned int sz)
{
  bfs.write((char*)data, sz);
}
#define WRITE(X) do_write(bfs, &X, sizeof(X))

bool vil3d_analyze_header::write_file(const std::string& path) const
{
  std::ofstream bfs(path.c_str(),std::ios::binary);
  if (!bfs) return false;

  if (key.sizeof_hdr != OptionalSize)
  {
    std::cerr << "vil3d_analyze_header::write_file: Header file is not the correct size.\n";
    return false;
  }
  else
  {
    WRITE(key.sizeof_hdr);
    for (int i=0; i<10; ++i)
      WRITE(key.data_type[i]);
    for (int i=0; i<18; ++i)
      WRITE(key.db_name[i]);
    WRITE(key.extents);
    WRITE(key.session_error);
    WRITE(key.regular);
    WRITE(key.hkey_un0);

    for (int i=0; i<8; ++i)
      WRITE(dim.dim[i]);
    WRITE(dim.unused8);
    WRITE(dim.unused9);
    WRITE(dim.unused10);
    WRITE(dim.unused11);
    WRITE(dim.unused12);
    WRITE(dim.unused13);
    WRITE(dim.unused14);
    WRITE(dim.datatype);
    WRITE(dim.bitpix);
    WRITE(dim.dim_un0);
    for (int i=0; i<8; ++i)
      WRITE(dim.pixdim[i]);
    WRITE(dim.funused8);
    WRITE(dim.funused9);
    WRITE(dim.funused10);
    WRITE(dim.funused11);
    WRITE(dim.funused12);
    WRITE(dim.funused13);
    WRITE(dim.compressed);
    WRITE(dim.verified);
    WRITE(dim.glmax);
    WRITE(dim.glmin);

    for (int i=0; i<80; ++i)
      WRITE(history.descrip[i]);
    for (int i=0; i<24; ++i)
      WRITE(history.aux_file[i]);
    WRITE(history.orient);
    for (int i=0; i<10; ++i)
      WRITE(history.originator[i]);
    for (int i=0; i<10; ++i)
      WRITE(history.generated[i]);
    for (int i=0; i<10; ++i)
      WRITE(history.scannum[i]);
    for (int i=0; i<10; ++i)
      WRITE(history.patient_id[i]);
    for (int i=0; i<10; ++i)
      WRITE(history.exp_date[i]);
    for (int i=0; i<10; ++i)
      WRITE(history.exp_time[i]);
    for (int i=0; i<3; ++i)
      WRITE(history.hist_un0[i]);
    WRITE(history.views);
    WRITE(history.vols_added);
    WRITE(history.start_field);
    WRITE(history.field_skip);
    WRITE(history.omax);
    WRITE(history.omin);
    WRITE(history.smax);
    WRITE(history.smin);
  }
  return true;
}

bool vil3d_analyze_header::read_file(const std::string& path)
{
  std::ifstream bfs(path.c_str(),std::ios::binary);
  if (!bfs) return false;

  bfs.read((char *)&key.sizeof_hdr, sizeof(key.sizeof_hdr));

  // Check if the bytes need swapping
  swap_bytes_ = (key.sizeof_hdr>RD_AH_MAXHEADERSIZE);

  swapBytes((char *)&key.sizeof_hdr, sizeof(key.sizeof_hdr));

  for (int i=0; i<10; ++i)
  {
    bfs.read((char *)&key.data_type[i],sizeof(key.data_type[i]));
    swapBytes((char *)&key.data_type[i],sizeof(key.data_type[i]));
  }
  for (int i=0; i<18; ++i)
  {
    bfs.read((char *)&key.db_name[i],sizeof(key.db_name[i]));
    swapBytes((char *)&key.db_name[i],sizeof(key.db_name[i]));
  }

  bfs.read((char *)&key.extents, sizeof(key.extents));
  swapBytes((char *)&key.extents,sizeof(key.extents));
  bfs.read((char *)&key.session_error,sizeof(key.session_error));
  swapBytes((char *)&key.session_error,sizeof(key.session_error));
  bfs.read((char *)&key.regular,sizeof(key.regular));
  swapBytes((char *)&key.regular,sizeof(key.regular));
  bfs.read((char *)&key.hkey_un0,sizeof(key.hkey_un0));
  swapBytes((char *)&key.hkey_un0,sizeof(key.hkey_un0));

  if (key.sizeof_hdr != ObligatorySize &&
      key.sizeof_hdr != OptionalSize)
  {
    std::cerr << "vil3d_analyze_header::load: Header file is not the correct size.\n";
    return false;
  }
  else
  {
    for (int i=0; i<8; ++i)
    {
      bfs.read((char *)&dim.dim[i],sizeof(dim.dim[i]));
      swapBytes((char *)&dim.dim[i],sizeof(dim.dim[i]));
    }

    bfs.read((char *)&dim.unused8,sizeof(dim.unused8));
    swapBytes((char *)&dim.unused8,sizeof(dim.unused8));
    bfs.read((char *)&dim.unused9,sizeof(dim.unused9));
    swapBytes((char *)&dim.unused9,sizeof(dim.unused9));
    bfs.read((char *)&dim.unused10,sizeof(dim.unused10));
    swapBytes((char *)&dim.unused10,sizeof(dim.unused10));
    bfs.read((char *)&dim.unused11,sizeof(dim.unused11));
    swapBytes((char *)&dim.unused11,sizeof(dim.unused11));
    bfs.read((char *)&dim.unused12,sizeof(dim.unused12));
    swapBytes((char *)&dim.unused12,sizeof(dim.unused12));
    bfs.read((char *)&dim.unused13,sizeof(dim.unused13));
    swapBytes((char *)&dim.unused14,sizeof(dim.unused13));
    bfs.read((char *)&dim.unused14,sizeof(dim.unused14));
    swapBytes((char *)&dim.unused14,sizeof(dim.unused14));
    bfs.read((char *)&dim.datatype,sizeof(dim.datatype));
    swapBytes((char *)&dim.datatype,sizeof(dim.datatype));
    bfs.read((char *)&dim.bitpix,sizeof(dim.bitpix));
    swapBytes((char *)&dim.bitpix,sizeof(dim.bitpix));
    bfs.read((char *)&dim.dim_un0,sizeof(dim.dim_un0));
    swapBytes((char *)&dim.dim_un0,sizeof(dim.dim_un0));
    for (int i=0; i<8; ++i)
    {
      bfs.read((char *)&dim.pixdim[i],sizeof(dim.pixdim[i]));
      swapBytes((char *)&dim.pixdim[i],sizeof(dim.pixdim[i]));
    }
    bfs.read((char *)&dim.funused8,sizeof(dim.funused8));
    swapBytes((char *)&dim.funused8,sizeof(dim.funused8));
    bfs.read((char *)&dim.funused9,sizeof(dim.funused9));
    swapBytes((char *)&dim.funused9,sizeof(dim.funused9));
    bfs.read((char *)&dim.funused10,sizeof(dim.funused10));
    swapBytes((char *)&dim.funused10,sizeof(dim.funused10));
    bfs.read((char *)&dim.funused11,sizeof(dim.funused11));
    swapBytes((char *)&dim.funused11,sizeof(dim.funused11));
    bfs.read((char *)&dim.funused12,sizeof(dim.funused12));
    swapBytes((char *)&dim.funused12,sizeof(dim.funused12));
    bfs.read((char *)&dim.funused13,sizeof(dim.funused13));
    swapBytes((char *)&dim.funused13,sizeof(dim.funused13));
    bfs.read((char *)&dim.compressed,sizeof(dim.compressed));
    swapBytes((char *)&dim.compressed,sizeof(dim.compressed));
    bfs.read((char *)&dim.verified,sizeof(dim.verified));
    swapBytes((char *)&dim.verified,sizeof(dim.verified));
    bfs.read((char *)&dim.glmax,sizeof(dim.glmax));
    swapBytes((char *)&dim.glmax,sizeof(dim.glmax));
    bfs.read((char *)&dim.glmin,sizeof(dim.glmin));
    swapBytes((char *)&dim.glmin,sizeof(dim.glmin));

    if (key.sizeof_hdr == OptionalSize)
    {
      for (int i=0; i<80; ++i)
      {
        bfs.read((char *)&history.descrip[i],
                 sizeof(history.descrip[i]));
        swapBytes((char *)&history.descrip[i],
                  sizeof(history.descrip[i]));
      }
      for (int i=0; i<24; ++i)
      {
        bfs.read((char *)&history.aux_file[i],
                 sizeof(history.aux_file[i]));
        swapBytes((char *)&history.aux_file[i],
                  sizeof(history.aux_file[i]));
      }
      bfs.read((char *)&history.orient, sizeof(history.orient));
      swapBytes((char *)&history.orient, sizeof(history.orient));
      for (int i=0; i<10; ++i)
      {
        bfs.read((char *)&history.originator[i],
                 sizeof(history.originator[i]));
        swapBytes((char *)&history.originator[i],
                  sizeof(history.originator[i]));
      }
      for (int i=0; i<10; ++i)
      {
        bfs.read((char *)&history.generated[i],
                 sizeof(history.generated[i]));
        swapBytes((char *)&history.generated[i],
                  sizeof(history.generated[i]));
      }
      for (int i=0; i<10; ++i)
      {
        bfs.read((char *)&history.scannum[i],
                 sizeof(history.scannum[i]));
        swapBytes((char *)&history.scannum[i],
                  sizeof(history.scannum[i]));
      }
      for (int i=0; i<10; ++i)
      {
        bfs.read((char *)&history.patient_id[i],
                 sizeof(history.patient_id[i]));
        swapBytes((char *)&history.patient_id[i],
                  sizeof(history.patient_id[i]));
      }
      for (int i=0; i<10; ++i)
      {
        bfs.read((char *)&history.exp_date[i],
                 sizeof(history.exp_date[i]));
        swapBytes((char *)&history.exp_date[i],
                  sizeof(history.exp_date[i]));
      }
      for (int i=0; i<10; ++i)
      {
        bfs.read((char *)&history.exp_time[i],
                 sizeof(history.exp_time[i]));
        swapBytes((char *)&history.exp_time[i],
                  sizeof(history.exp_time[i]));
      }
      for (int i=0; i<3; ++i)
      {
        bfs.read((char *)&history.hist_un0[i],
                 sizeof(history.hist_un0[i]));
        swapBytes((char *)&history.hist_un0[i],
                  sizeof(history.hist_un0[i]));
      }

      bfs.read((char *)&history.views,sizeof(history.views));
      swapBytes((char *)&history.views,sizeof(history.views));
      bfs.read((char *)&history.vols_added,sizeof(history.vols_added));
      swapBytes((char *)&history.vols_added,sizeof(history.vols_added));
      bfs.read((char *)&history.start_field,sizeof(history.start_field));
      swapBytes((char *)&history.start_field,sizeof(history.start_field));
      bfs.read((char *)&history.field_skip,sizeof(history.field_skip));
      swapBytes((char *)&history.field_skip,sizeof(history.field_skip));
      bfs.read((char *)&history.omax,sizeof(history.omax));
      swapBytes((char *)&history.omax,sizeof(history.omax));
      bfs.read((char *)&history.omin,sizeof(history.omin));
      swapBytes((char *)&history.omin,sizeof(history.omin));
      bfs.read((char *)&history.smax,sizeof(history.smax));
      swapBytes((char *)&history.smax,sizeof(history.smax));
      bfs.read((char *)&history.smin,sizeof(history.smin));
      swapBytes((char *)&history.smin,sizeof(history.smin));
    }
    else
      history.reset();
  }
  return true;
}

//: Define number of pixels in each dimension
void vil3d_analyze_header::set_image_size(unsigned ni, unsigned nj,
                                          unsigned nk, unsigned np)
{
  dim.dim[1]=(short int)(ni);
  dim.dim[2]=(short int)(nj);
  dim.dim[3]=(short int)(nk);
  dim.dim[4]=(short int)(np);
}
//: Define width of voxels in each dimension
void vil3d_analyze_header::set_voxel_size(float si, float sj, float sk)
{
  dim.pixdim[1]=si;
  dim.pixdim[2]=sj;
  dim.pixdim[3]=sk;
}

void vil3d_analyze_header::swapBytes(char *data, int size)
{
  if (needSwap())
  {
    char *temp = new char[size];
    for (int i=0; i<size; i++)  temp[(size-i)-1] = data[i];
    for (int i=0; i<size; i++)  data[i] = temp[i];
    delete [] temp;
  }
}

//: Define format of pixels
void vil3d_analyze_header::set_pixel_format(enum vil_pixel_format format)
{
  switch (format)
  {
    case VIL_PIXEL_FORMAT_BYTE :   dim.datatype=2; break;
    case VIL_PIXEL_FORMAT_INT_16 : dim.datatype=4; break;
    case VIL_PIXEL_FORMAT_INT_32 : dim.datatype=8; break;
    case VIL_PIXEL_FORMAT_FLOAT :  dim.datatype=16; break;
    case VIL_PIXEL_FORMAT_DOUBLE : dim.datatype=64; break;
    default: dim.datatype=0;
  }
}

//: Define format of pixels
enum vil_pixel_format vil3d_analyze_header::pixel_format() const
{
  switch (dim.datatype)
  {
    case 2  : return VIL_PIXEL_FORMAT_BYTE;
    case 4  : return VIL_PIXEL_FORMAT_INT_16;
    case 8  : return VIL_PIXEL_FORMAT_INT_32;
    case 16 : return VIL_PIXEL_FORMAT_FLOAT;
    case 64 : return VIL_PIXEL_FORMAT_DOUBLE;
    default : return VIL_PIXEL_FORMAT_UNKNOWN;
  }
  return VIL_PIXEL_FORMAT_UNKNOWN; // dummy return, to avoid compiler warning
}

//: Print out some parts of header
void vil3d_analyze_header::print_summary(std::ostream& os) const
{
  os<<"vil3d_analyze_header:\n";
  vsl_indent_inc(os);
  os<<vsl_indent()<<"Size: "<<dim.dim[1]<<" x "<<dim.dim[2]<<" x "<<dim.dim[3]
    <<"  Voxel widths: ("<<dim.pixdim[1]<<" x "<<dim.pixdim[2]<<" x "<<dim.pixdim[3]
    <<") Voxel type: "<<pixel_format()<<" (Code="<<dim.datatype<<')';
  vsl_indent_dec(os);
}

//: Print out some parts of header
std::ostream& operator<<(std::ostream& os, const vil3d_analyze_header& header)
{
  header.print_summary(os);
  return os;
}

// ==================================================================
// =================== vil3d_analyze_format =========================
// ==================================================================

vil3d_analyze_format::vil3d_analyze_format() {}

// The destructor must be virtual so that the memory chunk is destroyed.
vil3d_analyze_format::~vil3d_analyze_format()
{
}

vil3d_image_resource_sptr vil3d_analyze_format::make_input_image(const char *filename1) const
{
  vil3d_analyze_header header;
  std::string filename(filename1);
  std::string base_filename;
  std::size_t n=filename.size();
  if (n>=4 && (filename.substr(n-4,4)==".hdr" || filename.substr(n-4,4)==".img"))
    base_filename = filename.substr(0,n-4);
  else
    base_filename = filename;

  if (!header.read_file(std::string(base_filename)+".hdr")) return VXL_NULLPTR;
  std::cout<<"vil3d_analyze_format::make_input_image() Header: "<<header<<std::endl;

  return new vil3d_analyze_image(header,base_filename);
}


//: Make a "generic_image" on which put_section may be applied.
// The file may be opened immediately for writing so that a header can be written.
// The width/height etc are explicitly specified, so that file_format implementors
// know what they need to do...
vil3d_image_resource_sptr vil3d_analyze_format::make_output_image(const char* filename1,
                                                                  unsigned ni, unsigned nj,
                                                                  unsigned nk, unsigned nplanes,
                                                                  enum vil_pixel_format format) const
{
  if (format != VIL_PIXEL_FORMAT_BYTE   &&
      format != VIL_PIXEL_FORMAT_INT_16 &&
      format != VIL_PIXEL_FORMAT_FLOAT  && format != VIL_PIXEL_FORMAT_DOUBLE &&
      format != VIL_PIXEL_FORMAT_INT_32 )
  {
    std::cerr << "vil3d_analyze_format::make_output_image() WARNING\n"
             << "  Unable to deal with pixel format : " << format << std::endl;
    return VXL_NULLPTR;
  }

  vil3d_analyze_header header;
  header.set_image_size(ni,nj,nk,nplanes);
  header.set_pixel_format(format);
  header.set_voxel_size(1.0f,1.0f,1.0f);
  header.set_pixel_format(format);

  std::string filename(filename1);
  std::string base_filename;
  std::size_t n=filename.size();
  if (n>=4 && (filename.substr(n-4,4)==".hdr" || filename.substr(n-4,4)==".img"))
    base_filename = filename.substr(0,n-4);
  else
    base_filename = filename;
  if (!header.write_file(std::string(base_filename)+".hdr")) return VXL_NULLPTR;
  return new vil3d_analyze_image(header,base_filename);
}

// ==================================================================
// =================== vil3d_analyze_image ==========================
// ==================================================================

vil3d_analyze_image::vil3d_analyze_image(const vil3d_analyze_header& header,
                                         const std::string& base_path)
{
  header_ = header;
  base_path_ = base_path;
}

vil3d_analyze_image::~vil3d_analyze_image()
{
}

//: Number of planes (or time points in an image sequence).
unsigned vil3d_analyze_image::nplanes() const
{
  return header_.nplanes();
}

//: Dimensions:  nplanes x ni x nj x nk.
// The number of pixels in each row.
unsigned vil3d_analyze_image::ni() const
{
  return header_.dim.dim[1];
}

//: Dimensions:  nplanes x ni x nj x nk.
// The number of pixels in each column.
unsigned vil3d_analyze_image::nj() const
{
  return header_.dim.dim[2];
}

//: Dimensions:  nplanes x ni x nj x nk.
// The number of slices per image.
unsigned vil3d_analyze_image::nk() const
{
  return header_.dim.dim[3];
}

//: Pixel Format.
enum vil_pixel_format vil3d_analyze_image::pixel_format() const
{
  return header_.pixel_format();
}

//: Get some or all of the volume.
vil3d_image_view_base_sptr vil3d_analyze_image::get_copy_view(
                               unsigned i0, unsigned ni, unsigned j0, unsigned nj,
                               unsigned k0, unsigned nk) const
{
  // Can only cope with loading whole image at present.
  if (i0!=0 || int(ni)!=header_.ni() ||
      j0!=0 || int(nj)!=header_.nj() ||
      k0!=0 || int(nk)!=header_.nk()   ) return VXL_NULLPTR;

  std::string image_data_path=base_path_+".img";
  vil_smart_ptr<vil_stream> is = new vil_stream_fstream(image_data_path.c_str(),"r");
  if (!is->ok()) return VXL_NULLPTR;

// NOTE: See GIPL loader for more general data reading
#define read_data_of_type(type) \
  vil3d_image_view< type > im = \
         vil3d_new_image_view_plane_k_j_i(ni, nj, nk, nplanes(), type()); \
  is->read(&im(0,0,0,0), ni * nj * nk * nplanes() * sizeof(type));

  switch (pixel_format())
  {
   case VIL_PIXEL_FORMAT_BYTE:
   {
    read_data_of_type(vxl_byte);
    return new vil3d_image_view<vxl_byte>(im);
   }
   case VIL_PIXEL_FORMAT_INT_16:
   {
    read_data_of_type(vxl_int_16);
    if (header_.needSwap())
      swap16_for_big_endian((char *)(im.origin_ptr()), ni*nj*nk);
    return new vil3d_image_view<vxl_int_16>(im);
   }
   case VIL_PIXEL_FORMAT_INT_32:
   {
    read_data_of_type(vxl_int_32);
    if (header_.needSwap())
      swap32_for_big_endian((char *)(im.origin_ptr()), ni*nj*nk);
    return new vil3d_image_view<vxl_int_32>(im);
   }
   case VIL_PIXEL_FORMAT_FLOAT:
   {
    read_data_of_type(float);
    if (header_.needSwap())
      swap32_for_big_endian((char *)(im.origin_ptr()), ni*nj*nk);
    return new vil3d_image_view<float>(im);
   }
   case VIL_PIXEL_FORMAT_DOUBLE:
   {
    read_data_of_type(double);
    if (header_.needSwap())
      swap64_for_big_endian((char *)(im.origin_ptr()), ni*nj*nk);
    return new vil3d_image_view<double>(im);
   }
   case VIL_PIXEL_FORMAT_BOOL:
    std::cout<<"ERROR: vil3d_analyze_format::get_copy_view()"
            <<pixel_format() << " pixel type not yet implemented\n";
    return VXL_NULLPTR;
   default:
    std::cout<<"ERROR: vil3d_analyze_format::get_copy_view()\n"
            <<"Can't deal with pixel type " << pixel_format() << std::endl;
    return VXL_NULLPTR;
  }
}


//: Get the properties (of the first slice)
bool vil3d_analyze_image::get_property(char const *key, void * value) const
{
  if (std::strcmp(vil3d_property_voxel_size, key)==0)
  {
    float* array = static_cast<float*>(value);
    // analyze stores data in mm
    array[0] = header_.voxel_width_i() / 1000.0f;
    array[1] = header_.voxel_width_j() / 1000.0f;
    array[2] = header_.voxel_width_k() / 1000.0f;
    return true;
  }

  if (std::strcmp(vil3d_property_origin_offset, key)==0)
  {
    // Don't know how to get origin offset from header yet!
    float* array = static_cast<float*>(value);
    array[0] = (float)(0);
    array[1] = (float)(0);
    array[2] = (float)(0);
    return true;
  }

  return false;
}

//: Set the contents of the volume.
bool vil3d_analyze_image::put_view(const vil3d_image_view_base& view,
                                   unsigned i0=0, unsigned j0=0, unsigned k0=0)
{
  if (!view_fits(view, i0, j0, k0))
  {
    std::cerr << "ERROR: " << __FILE__ << ":\n view does not fit\n";
    return false;
  }
  if (view.ni()!=ni() || view.nj()!=nj() || view.nk()!=nk())
  {
    std::cerr<<"Can only write whole image at once.\n";
    return false;
  }

  std::string image_data_path=base_path_+".img";
  vil_smart_ptr<vil_stream> os = new vil_stream_fstream(image_data_path.c_str(),"w");
  if (!os->ok()) return 0;

  switch (pixel_format())
  {
   case VIL_PIXEL_FORMAT_BYTE:
   {
    vil3d_image_view<vxl_byte> view_copy(ni(),nj(),nk(),nplanes());
    vil3d_copy_reformat(static_cast<const vil3d_image_view<vxl_byte>&>(view),view_copy);
    os->write(view_copy.origin_ptr(),ni()*nj()*nk()*nplanes());
    // Should check that write was successful
    return true;
   }
   case VIL_PIXEL_FORMAT_INT_16:
   {
    vil3d_image_view<vxl_int_16> view_copy(ni(),nj(),nk(),nplanes());
    vil3d_copy_reformat(static_cast<const vil3d_image_view<vxl_int_16>&>(view),view_copy);
    if (header_.needSwap())
      swap16_for_big_endian((char *)(view_copy.origin_ptr()), ni()*nj()*nk()*nplanes());
    os->write(view_copy.origin_ptr(),ni()*nj()*nk()*nplanes()*sizeof(vxl_int_16));
    // Should check that write was successful
    return true;
   }
   case VIL_PIXEL_FORMAT_INT_32:
   {
    vil3d_image_view<vxl_int_32> view_copy(ni(),nj(),nk(),nplanes());
    vil3d_copy_reformat(static_cast<const vil3d_image_view<vxl_int_32>&>(view),view_copy);
    if (header_.needSwap())
      swap32_for_big_endian((char *)(view_copy.origin_ptr()), ni()*nj()*nk()*nplanes());
    os->write(view_copy.origin_ptr(),ni()*nj()*nk()*nplanes()*sizeof(vxl_int_32));
    // Should check that write was successful
    return true;
   }
   case VIL_PIXEL_FORMAT_FLOAT:
   {
    vil3d_image_view<float> view_copy(ni(),nj(),nk(),nplanes());
    vil3d_copy_reformat(static_cast<const vil3d_image_view<float>&>(view),view_copy);
    if (header_.needSwap())
      swap32_for_big_endian((char *)(view_copy.origin_ptr()), ni()*nj()*nk()*nplanes());
    os->write(view_copy.origin_ptr(),ni()*nj()*nk()*nplanes()*sizeof(float));
    // Should check that write was successful
    return true;
   }
   case VIL_PIXEL_FORMAT_DOUBLE:
   {
     vil3d_image_view<double> view_copy(ni(),nj(),nk(),nplanes());
     vil3d_copy_reformat(static_cast<const vil3d_image_view<double>&>(view),view_copy);
     if (header_.needSwap())
       swap32_for_big_endian((char *)(view_copy.origin_ptr()), ni()*nj()*nk()*nplanes());
     os->write(view_copy.origin_ptr(),ni()*nj()*nk()*nplanes()*sizeof(double));
     // Should check that write was successful
     return true;
   }
   default:
    std::cout<<"ERROR: vil3d_analyze_format::put_view()\n"
            <<"Can't deal with pixel type " << pixel_format() << std::endl;
  }

  return false;
}

//: Set the size of the each voxel in the i,j,k directions.
// You can get the voxel sizes via get_properties().
// \return false if underlying image doesn't store pixel sizes.
bool vil3d_analyze_image::set_voxel_size_mm(float si,float sj,float sk)
{
  header_.set_voxel_size(si,sj,sk);
  // Overwrite the header in the file
  if (!header_.write_file(base_path_+".hdr")) return false;
  return true;
}

