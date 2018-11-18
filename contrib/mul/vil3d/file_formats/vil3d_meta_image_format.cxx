// This is mul/vil3d/file_formats/vil3d_meta_image_format.cxx
//:
// \file
// \brief Reader/Writer for meta image format images.
// \author Chris Wolstenholme - Imorphics Ltd
// This file contains classes for reading and writing meta image format images
// Three key components are
// * vil3d_meta_image_header: Structure to contain header information
// * vil3d_meta_image: Resource object which interfaces to the file,
//                        allowing reading and writing via the get_copy_image()
//                        and put_image() functions
// * vil3d_meta_image_format: Object to create an appropriate vil3d_meta_image
//
// The main work of loading and saving happens in vil3d_meta_image

#include <algorithm>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <utility>
#include "vil3d_meta_image_format.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil/vil_stream_fstream.h>
#include <vil3d/vil3d_copy.h>
#include <vil3d/vil3d_image_resource.h>
#include <vil3d/vil3d_image_view.h>
#include <vil3d/vil3d_new.h>
#include <vil3d/vil3d_property.h>
#include <vul/vul_file.h>

//
// Helper functions
//
inline void vil3d_meta_image_swap16(char *a, unsigned n)
{
  for (unsigned i = 0; i < n * 2; i += 2)
  {
    char c = a[i]; a[i] = a[i+1]; a[i+1] = c;
  }
}

inline void vil3d_meta_image_swap32(char *a, unsigned n)
{
  for (unsigned i = 0; i < n * 4; i += 4)
  {
    char c= a[i]; a[i] = a[i+3]; a[i+3] = c;
    c = a[i+1]; a[i+1] = a[i+2]; a[i+2] = c;
  }
}

inline void vil3d_meta_image_swap64(char *a, unsigned n)
{
  for (unsigned i = 0; i < n * 2; i += 2)
  {
    char c = a[i]; a[i] = a[i+7]; a[i+7] = c;
    c = a[i+1]; a[i+1] = a[i+6]; a[i+6] = c;
    c = a[i+2]; a[i+2] = a[i+5]; a[i+5] = c;
    c = a[i+3]; a[i+3] = a[i+4]; a[i+4] = c;
  }
}

//
// Header stuff
//

//===================================================================
// Header constructor
//===================================================================
vil3d_meta_image_header::vil3d_meta_image_header(void) :
header_valid_(false),
byte_order_msb_(false),
offset_i_(0.0), offset_j_(0.0), offset_k_(0.0),
vox_size_i_(1.0), vox_size_j_(1.0), vox_size_k_(1.0),
dim_size_i_(0), dim_size_j_(0), dim_size_k_(0), nplanes_(1),
need_swap_(false)
{
  // No construction code
}

//===================================================================
// Header destructor
//===================================================================
vil3d_meta_image_header::~vil3d_meta_image_header(void)
{
  // No destructor code
}

//===================================================================
// Set/get byte order
//===================================================================
void vil3d_meta_image_header::set_byte_order_msb(const bool is_msb)
{
  byte_order_msb_ = is_msb;
}

bool vil3d_meta_image_header::byte_order_is_msb(void) const
{
  return byte_order_msb_;
}

//===================================================================
// Set/get offset
//===================================================================
void vil3d_meta_image_header::set_offset(const double off_i,
                                         const double off_j,
                                         const double off_k)
{
  offset_i_ = off_i;
  offset_j_ = off_j;
  offset_k_ = off_k;
}

double vil3d_meta_image_header::offset_i(void) const
{
  return offset_i_;
}

double vil3d_meta_image_header::offset_j(void) const
{
  return offset_j_;
}

double vil3d_meta_image_header::offset_k(void) const
{
  return offset_k_;
}

//===================================================================
// Set/get voxel sizes
//===================================================================
void vil3d_meta_image_header::set_vox_size(const double vox_i,
                                           const double vox_j,
                                           const double vox_k)
{
  vox_size_i_ = vox_i;
  vox_size_j_ = vox_j;
  vox_size_k_ = vox_k;
}

double vil3d_meta_image_header::vox_size_i(void) const
{
  return vox_size_i_;
}

double vil3d_meta_image_header::vox_size_j(void) const
{
  return vox_size_j_;
}

double vil3d_meta_image_header::vox_size_k(void) const
{
  return vox_size_k_;
}

//===================================================================
// Set/get image dimensions
//===================================================================
void vil3d_meta_image_header::set_dim_size(const unsigned int ni,
                                           const unsigned int nj,
                                           const unsigned int nk,
                                           const unsigned int np)
{
  dim_size_i_ = ni;
  dim_size_j_ = nj;
  dim_size_k_ = nk;
  nplanes_ = np;
}

unsigned int vil3d_meta_image_header::ni(void) const
{
  return dim_size_i_;
}

unsigned int vil3d_meta_image_header::nj(void) const
{
  return dim_size_j_;
}

unsigned int vil3d_meta_image_header::nk(void) const
{
  return dim_size_k_;
}

unsigned int vil3d_meta_image_header::nplanes(void) const
{
  return nplanes_;
}

//===================================================================
// Set/get element type
//===================================================================
void vil3d_meta_image_header::set_element_type(const std::string &elem_type)
{
  elem_type_ = elem_type;
}

const std::string &vil3d_meta_image_header::element_type(void) const
{
  return elem_type_;
}

//===================================================================
// Set/get image file name
//===================================================================
void vil3d_meta_image_header::set_image_fname(const std::string &image_fname)
{
  im_file_ = image_fname;
}

const std::string &vil3d_meta_image_header::image_fname(void) const
{
  return im_file_;
}

//===================================================================
// Set/get the pixel format
//===================================================================
void vil3d_meta_image_header::set_pixel_format(const vil_pixel_format format)
{
  pformat_ = format;
}

vil_pixel_format vil3d_meta_image_header::pixel_format(void) const
{
  return pformat_;
}

//===================================================================
// Set the header back to defaults
//===================================================================
void vil3d_meta_image_header::clear(void)
{
  header_valid_ = false;
  byte_order_msb_ = false;
  offset_i_ = offset_j_ = offset_k_ = 0.0;
  vox_size_i_ = vox_size_j_ = vox_size_k_ = 1.0;
  dim_size_i_ = dim_size_j_ = dim_size_k_ = 0;
  elem_type_ = im_file_ = "";
  need_swap_ = false;
}

//===================================================================
// Read the header
//===================================================================
bool vil3d_meta_image_header::read_header(const std::string &header_fname)
{
  std::ifstream hfs(header_fname.c_str());

  if (!hfs)
    return false;

  std::string nxt_line;
  std::getline(hfs,nxt_line);
  while (hfs.good() && !hfs.eof())
  {
    if (!check_next_header_line(nxt_line))
    {
      hfs.close();
      return false;
    }
    std::getline(hfs,nxt_line);
  }
  hfs.close();
  if (header_valid_)
  {
    std::string pth = vul_file::dirname(header_fname);
    pth += "/" + im_file_;
    im_file_ = pth;
    return true;
  }
  else
    return false;
}

//===================================================================
// Write the header
//===================================================================
bool vil3d_meta_image_header::write_header(const std::string &header_fname) const
{
  std::ofstream ofs(header_fname.c_str());
  if (!ofs)
    return false;

  ofs << "ObjectType = Image\n"
      << "NDims = 3\n"
      << "BinaryData = True\n"
      << "BinaryDataByteOrderMSB = " << ((byte_order_msb_) ? "True" : "False") << '\n'
      << "CompressedData = False\n"
      << "TransformMatrix = 1 0 0 0 1 0 0 0 1\n"
      << "Offset = " << offset_i_ << ' ' << offset_j_ << ' ' << offset_k_ << '\n'
      << "CenterOfRotation = 0 0 0\n"
      << "AnatomicalOrientation = RAI\n"
      << "ElementSpacing = " << vox_size_i_ << ' ' << vox_size_j_ << ' ' << vox_size_k_ << '\n'
      << "DimSize = " << dim_size_i_ << ' ' << dim_size_j_ << ' ' << dim_size_k_ << '\n'
      << "ElementType = " << elem_type_ << '\n'
      << "ElementDataFile = " << vul_file::strip_directory(im_file_) << std::endl;

  ofs.close();
  return true;
}

//===================================================================
// Display header elements
//===================================================================
void vil3d_meta_image_header::print_header(std::ostream &os) const
{
  os << "\n============= Meta Image Header Summary Begin =============\n"
     << "vil3d_meta_image_header - byte order is msb: " << ((byte_order_msb_) ? "true" : "false") << '\n'
     << "vil3d_meta_image_header - offset " << offset_i_ << ", " << offset_j_ << ", " << offset_k_ << '\n'
     << "vil3d_meta_image_header - voxel size: " << vox_size_i_ << ", " << vox_size_j_ << ", " << vox_size_k_ << '\n'
     << "vil3d_meta_image_header - dimension size: " << dim_size_i_ << ", " << dim_size_j_ << ", " << dim_size_k_ << '\n'
     << "vil3d_meta_image_header - nplanes: " << nplanes_ << '\n'
     << "vil3d_meta_image_header - element type: " << elem_type_ << '\n'
     << "vil3d_meta_image_header - image file: " << im_file_ << '\n'
     << "============= Meta Image Header Summary End =============\n" << std::endl;
}

//===================================================================
// Check if swapping is needed
//===================================================================
void vil3d_meta_image_header::check_need_swap(void)
{
  union short_char
  {
    short short_val;
    char char_val[2];
  } test_swap;

  test_swap.short_val = 1;

  if (test_swap.char_val[0]==1 && byte_order_msb_) // System little endian, file big endian
    need_swap_ = true;
  else if (test_swap.char_val[1]==1 && (!byte_order_msb_)) // System big endian, file little endian
    need_swap_ = true;
  else
    need_swap_ = false;
}

//===================================================================
// Return whether swap is needed
//===================================================================
bool vil3d_meta_image_header::need_swap(void) const
{
  return need_swap_;
}

//===================================================================
// Check the next line in the header
//===================================================================
bool vil3d_meta_image_header::check_next_header_line(const std::string &nxt_line)
{
  // Look for each element we're interested in
  std::string val = get_header_value(nxt_line);
  if (val=="")
    return false;

  if (nxt_line.find("ObjectType")!= std::string::npos)
  {
    if (val != "Image")
    {
      std::cerr << "Loader only handles Image Types.\n";
      return false;
    }
  }
  else if (nxt_line.find("NDims")!= std::string::npos)
  {
    unsigned int nd = std::atoi(val.c_str());
    if (nd != 3)
    {
      std::cerr << "Loader only handles 3D Images.\n";
      return false;
    }
  }
  else if (nxt_line.find("BinaryDataByteOrderMSB")!= std::string::npos)
  {
    byte_order_msb_ = (val=="True") ? true : false;
    header_valid_ = true;
    check_need_swap();
  }
  else if (nxt_line.find("CompressedData")!= std::string::npos)
  {
    if (val=="True")
    {
      std::cerr << "Loader does not handle compressed data\n";
      return false;
    }
  }
  else if (nxt_line.find("TransformMatrix")!= std::string::npos)
  {
    if (val != "1 0 0 0 1 0 0 0 1")
    {
      std::cout << "Loader only handles identity in TransformMatrix.\n"
               << "Transformation ignored." << std::endl;
    }
  }
  else if (nxt_line.find("Offset")!= std::string::npos) // If there is another field at some point with Offset in the name check them before this one!
  {
    return set_header_offset(val);
  }
  else if (nxt_line.find("DimSize")!= std::string::npos)
  {
    return set_header_dim_size(val);
  }
  else if (nxt_line.find("ElementSpacing")!= std::string::npos)
  {
    return set_header_voxel_size(val);
  }
  else if (nxt_line.find("ElementSize")!= std::string::npos)
  {
    return set_header_voxel_size(val);
  }
  else if (nxt_line.find("ElementType")!= std::string::npos)
  {
    elem_type_ = val;
    if (elem_type_ == "MET_SHORT")
      pformat_ = VIL_PIXEL_FORMAT_INT_16;
    else if (elem_type_ == "MET_UCHAR")
      pformat_ = VIL_PIXEL_FORMAT_BYTE;
    else if (elem_type_ == "MET_CHAR")
      pformat_ = VIL_PIXEL_FORMAT_SBYTE;
    else if (elem_type_ == "MET_DOUBLE")
      pformat_ = VIL_PIXEL_FORMAT_DOUBLE;
    else if (elem_type_ == "MET_FLOAT")
      pformat_ = VIL_PIXEL_FORMAT_FLOAT;
    else
    {
      std::cerr << "Unsupported element type specified: " << val << "\n";
      return false;
    }
    header_valid_ = true;
  }
  else if (nxt_line.find("ElementDataFile")!= std::string::npos)
  {
    im_file_ = val;
    header_valid_ = true;
  }
  return true;
}

//===================================================================
// Get the value associated with a header element
//===================================================================
std::string vil3d_meta_image_header::get_header_value(const std::string &nxt_line)
{
  std::string::size_type pos, epos;
  pos = nxt_line.find('=');
  if (pos == std::string::npos || pos == nxt_line.size()-1)
  {
    return "";
  }

  pos = nxt_line.find_first_not_of(' ', pos+1);
  epos = nxt_line.find_last_not_of(' ');
  if (pos == std::string::npos || epos == std::string::npos)
  {
    return "";
  }

  return nxt_line.substr(pos, (epos-pos)+1);
}

//===================================================================
// Set the header offset
//===================================================================
bool vil3d_meta_image_header::set_header_offset(const std::string &offs)
{
  std::string::size_type pos,epos;
  epos=offs.find_first_of(' ');
  if (epos==std::string::npos)
  {
    std::cerr << "Offset does not contain three values.\n";
    return false;
  }

  offset_i_=std::atof(offs.substr(0,epos).c_str());
  pos=offs.find_first_not_of(' ',epos);
  epos=offs.find_first_of(' ',pos);
  if (pos==std::string::npos || epos==std::string::npos)
  {
    std::cerr << "Offset does not contain three values.\n";
    return false;
  }

  offset_j_=std::atof(offs.substr(pos,epos).c_str());
  pos=offs.find_first_not_of(' ',epos);
  if (pos==std::string::npos)
  {
    std::cerr << "Offset does not contain three values.\n";
    return false;
  }
  offset_k_=std::atof(offs.substr(pos).c_str());
  epos = offs.find_first_of(' ',pos);
  pos=offs.find_first_not_of(' ',epos);
  if (pos != std::string::npos)
  {
     std::cerr << "Offset contains more than three values.\n";
     return false;
  }
  header_valid_ = true;
  return true;
}

//===================================================================
// Set the dimensions from the header
//===================================================================
bool vil3d_meta_image_header::set_header_dim_size(const std::string &dims)
{
  std::string::size_type pos,epos;
  epos=dims.find_first_of(' ');
  if (epos==std::string::npos)
  {
    std::cerr << "Dim Size does not contain three values.\n";
    return false;
  }
  dim_size_i_=std::atoi(dims.substr(0,epos).c_str());
  pos=dims.find_first_not_of(' ',epos);
  epos=dims.find_first_of(' ',pos);
  if (pos==std::string::npos || epos==std::string::npos)
  {
    std::cerr << "Dim Size does not contain three values.\n";
    return false;
  }
  dim_size_j_=std::atoi(dims.substr(pos,epos).c_str());
  pos=dims.find_first_not_of(' ',epos);
  if (pos==std::string::npos)
  {
    std::cerr << "Dim Size does not contain three values.\n";
    return false;
  }
  dim_size_k_=std::atoi(dims.substr(pos).c_str());
  epos = dims.find_first_of(' ',pos);
  pos=dims.find_first_not_of(' ',epos);
  if (pos != std::string::npos)
  {
     std::cerr << "Dim Size contains more than three values.\n";
     return false;
  }
  // For now only deal with 1 plane
  nplanes_=1;
  header_valid_=true;
  return true;
}

//===================================================================
// Set the header voxel size
//===================================================================
bool vil3d_meta_image_header::set_header_voxel_size(const std::string &vsize)
{
  std::string::size_type pos,epos;
  epos=vsize.find_first_of(' ');
  if (epos==std::string::npos)
  {
    std::cerr << "Element Spacing/Size does not contain three values.\n";
    return false;
  }
  vox_size_i_=std::atof(vsize.substr(0,epos).c_str());
  pos=vsize.find_first_not_of(' ',epos);
  epos=vsize.find_first_of(' ',pos);
  if (pos==std::string::npos || epos==std::string::npos)
  {
    std::cerr << "Element Spacing/Size does not contain three values.\n";
    return false;
  }
  vox_size_j_=std::atof(vsize.substr(pos,epos).c_str());
  pos=vsize.find_first_not_of(' ',epos);
  if (pos==std::string::npos)
  {
    std::cerr << "Element Spacing/Size does not contain three values.\n";
    return false;
  }
  vox_size_k_=std::atof(vsize.substr(pos).c_str());
  epos = vsize.find_first_of(' ',pos);
  pos=vsize.find_first_not_of(' ',epos);
  if (pos != std::string::npos)
  {
     std::cerr << "Element Spacing/Size contains more than three values.\n";
     return false;
  }
  header_valid_ = true;
  return true;
}

//===================================================================
// Display the header
//===================================================================
std::ostream& operator<<(std::ostream& os, const vil3d_meta_image_header& header)
{
  header.print_header(os);
  return os;
}

/*
 * Format stuff
 */

//===================================================================
// Construct image format
//===================================================================
vil3d_meta_image_format::vil3d_meta_image_format()
{
  // Nothing to be done on construction
}

//===================================================================
// Destruct image format
//===================================================================
vil3d_meta_image_format::~vil3d_meta_image_format()
{
  // Nothing to be done on destruction
}

//===================================================================
// Create an input image
//===================================================================
vil3d_image_resource_sptr vil3d_meta_image_format::make_input_image(const char *fname) const
{
  vil3d_meta_image_header header;
  std::string filename(fname);

  if (!header.read_header(fname)) return nullptr;
  //std::cout<<"vil3d_meta_image_format::make_input_image() Header: "<<header<<std::endl;

  return new vil3d_meta_image(header,filename);
}

//===================================================================
// Create an output image
//===================================================================
vil3d_image_resource_sptr vil3d_meta_image_format::make_output_image(const char *filename,
                                                                     unsigned int ni,
                                                                     unsigned int nj,
                                                                     unsigned int nk,
                                                                     unsigned int nplanes,
                                                                     vil_pixel_format format) const
{
  if (format != VIL_PIXEL_FORMAT_BYTE   &&
      format != VIL_PIXEL_FORMAT_SBYTE &&
      format != VIL_PIXEL_FORMAT_INT_16 &&
      format != VIL_PIXEL_FORMAT_DOUBLE &&
      format != VIL_PIXEL_FORMAT_FLOAT)
  {
    std::cerr << "vil3d_meta_image_format::make_output_image() WARNING\n"
             << "  Unable to deal with pixel format : " << format << std::endl;
    return nullptr;
  }

  vil3d_meta_image_header header;
  header.clear();
  header.set_dim_size(ni,nj,nk,nplanes);
  header.set_pixel_format(format);

  switch (format)
  {
  case VIL_PIXEL_FORMAT_BYTE: header.set_element_type("MET_UCHAR");
                              break;
  case VIL_PIXEL_FORMAT_SBYTE: header.set_element_type("MET_CHAR");
                              break;
  case VIL_PIXEL_FORMAT_INT_16: header.set_element_type("MET_SHORT");
                              break;
  case VIL_PIXEL_FORMAT_DOUBLE: header.set_element_type("MET_DOUBLE");
                              break;
  case VIL_PIXEL_FORMAT_FLOAT: header.set_element_type("MET_FLOAT");
                              break;
  default:
      std::cerr << "vil3d_meta_image_format::make_output_image() WARNING\n"
               << "  Unable to deal with pixel format : " << format << std::endl;
      return nullptr;
  }

  std::string str_fname(filename);
  std::string base_filename;
  std::size_t n=str_fname.size();
  if (n>=4 && (str_fname.substr(n-4,4)==".mhd" || str_fname.substr(n-4,4)==".raw"))
    base_filename = str_fname.substr(0,n-4);
  else
    base_filename = str_fname;
  std::string im_file = vul_file::strip_directory(base_filename);
  header.set_image_fname(im_file + ".raw");
  if (!header.write_header(base_filename+".mhd")) return nullptr;
  return new vil3d_meta_image(header,base_filename);
}

/*
 * Image stuff
 */

//===================================================================
// Construct an image
//===================================================================
vil3d_meta_image::vil3d_meta_image(const vil3d_meta_image_header &header,
                                   std::string fname) :
header_(header),
fpath_(std::move(fname))
{
  // No code necessary
}

//===================================================================
// Destruct
//===================================================================
vil3d_meta_image::~vil3d_meta_image(void)
{
  // No code necessary
}

//===================================================================
// Get the image dimension details
//===================================================================
unsigned int vil3d_meta_image::nplanes(void) const
{
  return header_.nplanes();
}

unsigned int vil3d_meta_image::ni(void) const
{
  return header_.ni();
}

unsigned int vil3d_meta_image::nj(void) const
{
  return header_.nj();
}

unsigned int vil3d_meta_image::nk(void) const
{
  return header_.nk();
}

//===================================================================
// Get the current header
//===================================================================
const vil3d_meta_image_header &vil3d_meta_image::header(void) const
{
  return header_;
}

//===================================================================
// Get the pixel format
//===================================================================
vil_pixel_format vil3d_meta_image::pixel_format(void) const
{
  return header_.pixel_format();
}

//===================================================================
// Set the voxel size
//===================================================================
bool vil3d_meta_image::set_voxel_size_mm(float vi, float vj, float vk)
{
  header_.set_vox_size(vi,vj,vk);
  if (!header_.write_header(fpath_+".mhd")) return false;
  return true;
}

//===================================================================
// Set the offet
//===================================================================
void vil3d_meta_image::set_offset(const double i, const double j, const double k,
                                  const double vx_i, const double vx_j, const double vx_k)
{
  double os_i, os_j, os_k;
  os_i = (-(i*vx_i));
  os_j = (-(j*vx_j));
  os_k = (-(k*vx_k));
  header_.set_offset(os_i,os_j,os_k);
  header_.set_vox_size(vx_i,vx_j,vx_k);
  header_.write_header(fpath_+".mhd");
}

//===================================================================
// Create a read/write view of a copy of this data
//===================================================================
vil3d_image_view_base_sptr vil3d_meta_image::get_copy_view(unsigned int i0, unsigned int ni,
                                                           unsigned int j0, unsigned int nj,
                                                           unsigned int k0, unsigned int nk) const
{
  // Can only cope with loading whole image at present.
  if (i0!=0 || ni!=header_.ni() ||
      j0!=0 || nj!=header_.nj() ||
      k0!=0 || nk!=header_.nk()   )
    return nullptr;

  std::string image_data_path=header_.image_fname();
  vil_smart_ptr<vil_stream> is = new vil_stream_fstream(image_data_path.c_str(),"r");
  if (!is->ok()) return nullptr;

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
   case VIL_PIXEL_FORMAT_SBYTE:
   {
     read_data_of_type(vxl_sbyte);
     return new vil3d_image_view<vxl_sbyte>(im);
   }
   case VIL_PIXEL_FORMAT_INT_16:
   {
    read_data_of_type(vxl_int_16);
    if (header_.need_swap())
      vil3d_meta_image_swap16((char *)(im.origin_ptr()), ni*nj*nk);
    return new vil3d_image_view<vxl_int_16>(im);
   }
   case VIL_PIXEL_FORMAT_DOUBLE:
   {
    read_data_of_type(double);
    if (header_.need_swap())
      vil3d_meta_image_swap64((char *)(im.origin_ptr()), ni*nj*nk);
    return new vil3d_image_view<double>(im);
   }
   case VIL_PIXEL_FORMAT_FLOAT:
   {
    read_data_of_type(float);
    if (header_.need_swap())
      vil3d_meta_image_swap32((char *)(im.origin_ptr()), ni*nj*nk);
    return new vil3d_image_view<float>(im);
   }
   default:
    std::cout<<"ERROR: vil3d_meta_image_format::get_copy_view()\n"
            <<"Can't deal with pixel type " << pixel_format() << std::endl;
    return nullptr;
  }
}

//===================================================================
// Put view
//===================================================================
bool vil3d_meta_image::put_view(const vil3d_image_view_base &im,
                                unsigned int i0,
                                unsigned int j0,
                                unsigned int k0)
{
  if (!view_fits(im, i0, j0, k0))
  {
    std::cerr << "ERROR: " << __FILE__ << ":\n view does not fit\n";
    return false;
  }
  if (im.ni()!=ni() || im.nj()!=nj() || im.nk()!=nk())
  {
    std::cerr<<"Can only write whole image at once.\n";
    return false;
  }

  std::string image_data_path=fpath_+".raw";
  vil_smart_ptr<vil_stream> os = new vil_stream_fstream(image_data_path.c_str(),"w");
  if (!os->ok()) return false;

  switch (pixel_format())
  {
   case VIL_PIXEL_FORMAT_BYTE:
   {
    vil3d_image_view<vxl_byte> view_copy(ni(),nj(),nk(),nplanes());
    vil3d_copy_reformat(static_cast<const vil3d_image_view<vxl_byte>&>(im),view_copy);
    os->write(view_copy.origin_ptr(),ni()*nj()*nk()*nplanes());
    // Should check that write was successful
    return true;
   }
   case VIL_PIXEL_FORMAT_SBYTE:
   {
    vil3d_image_view<vxl_sbyte> view_copy(ni(),nj(),nk(),nplanes());
    vil3d_copy_reformat(static_cast<const vil3d_image_view<vxl_sbyte>&>(im),view_copy);
    os->write(view_copy.origin_ptr(),ni()*nj()*nk()*nplanes());
    // Should check that write was successful
    return true;
   }
   case VIL_PIXEL_FORMAT_INT_16:
   {
     header_.check_need_swap();
    vil3d_image_view<vxl_int_16> view_copy(ni(),nj(),nk(),nplanes());
    vil3d_copy_reformat(static_cast<const vil3d_image_view<vxl_int_16>&>(im),view_copy);
    if (header_.need_swap())
      vil3d_meta_image_swap16((char *)(view_copy.origin_ptr()), ni()*nj()*nk()*nplanes());
    os->write(view_copy.origin_ptr(),ni()*nj()*nk()*nplanes()*sizeof(vxl_int_16));
    // Should check that write was successful
    return true;
   }
   case VIL_PIXEL_FORMAT_DOUBLE:
   {
     header_.check_need_swap();
    vil3d_image_view<double> view_copy(ni(),nj(),nk(),nplanes());
    vil3d_copy_reformat(static_cast<const vil3d_image_view<double>&>(im),view_copy);
    if (header_.need_swap())
      vil3d_meta_image_swap64((char *)(view_copy.origin_ptr()), ni()*nj()*nk()*nplanes());
    os->write(view_copy.origin_ptr(),ni()*nj()*nk()*nplanes()*sizeof(double));
    // Should check that write was successful
    return true;
   }
   case VIL_PIXEL_FORMAT_FLOAT:
   {
     header_.check_need_swap();
    vil3d_image_view<float> view_copy(ni(),nj(),nk(),nplanes());
    vil3d_copy_reformat(static_cast<const vil3d_image_view<float>&>(im),view_copy);
    if (header_.need_swap())
      vil3d_meta_image_swap32((char *)(view_copy.origin_ptr()), ni()*nj()*nk()*nplanes());
    os->write(view_copy.origin_ptr(),ni()*nj()*nk()*nplanes()*sizeof(float));
    // Should check that write was successful
    return true;
   }
   default:
    std::cout<<"ERROR: vil3d_analyze_format::put_view()\n"
            <<"Can't deal with pixel type " << pixel_format() << std::endl;
  }

  return false;
}

//===================================================================
// Get an image property
//===================================================================
bool vil3d_meta_image::get_property(const char *label, void *property_value) const
{
  if (std::strcmp(vil3d_property_voxel_size, label)==0)
  {
    auto* array = static_cast<float*>(property_value);
    // meta image stores data in mm
    array[0] = static_cast<float>(header_.vox_size_i() / 1000.0);
    array[1] = static_cast<float>(header_.vox_size_j() / 1000.0);
    array[2] = static_cast<float>(header_.vox_size_k() / 1000.0);
    return true;
  }

  if (std::strcmp(vil3d_property_origin_offset, label)==0)
  {
    auto* array = static_cast<float*>(property_value);
    array[0] = static_cast<float>((-header_.offset_i())/header_.vox_size_i());
    array[1] = static_cast<float>((-header_.offset_j())/header_.vox_size_j());
    array[2] = static_cast<float>((-header_.offset_k())/header_.vox_size_k());
    return true;
  }

  return false;
}
