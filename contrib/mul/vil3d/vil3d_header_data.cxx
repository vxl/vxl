// This is mul/vil3d/vil3d_header_data.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \brief A base class for objects containing information from image headers.
// \author Tim Cootes - Manchester

#include <vil3d/vil3d_header_data.h>

vil3d_header_data::vil3d_header_data()
  : ni_(0), nj_(0), nk_(0), nplanes_(0), reference_count_(0)
{
}

//: Indicate size is ni x nj * nk
void vil3d_header_data::set_size(unsigned ni, unsigned nj, unsigned nk)
{
  set_size(ni,nj,nk,1);
}

//: resize to ni x nj * nk with nplanes planes.
// If already correct size, this function returns quickly
void vil3d_header_data::set_size(unsigned ni, unsigned nj,
                                 unsigned nk, unsigned nplanes)
{
  ni_ = ni;
  nj_ = nj;
  nk_ = nk;
  nplanes_ = nplanes;
}

//: Define voxel size
void vil3d_header_data::set_voxel_widths(double wi, double wj, double wk)
{
  voxel_width_i_ = wi;
  voxel_width_j_ = wj;
  voxel_width_k_ = wk;
}

//: Define pixel format
void vil3d_header_data::set_pixel_format(vil2_pixel_format f)
{
  pixel_format_ = f;
}

//: Print a 1-line summary of contents
void vil3d_header_data::print(vcl_ostream& os) const
{
  os<<nplanes_<<" planes of size: "<<ni_<<" x "<<nj_<<" x "<<nk_<<vcl_endl;
}

//: Return class name
vcl_string vil3d_header_data::is_a() const
{
  return vcl_string("vil3d_header_data");
}

//: True if this is (or is derived from) class s
bool vil3d_header_data::is_class(vcl_string const& s) const
{
  return s=="vil3d_header_data";
}

