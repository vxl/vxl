// This is mul/vil3d/vil3d_header_data.h
#ifndef vil3d_header_data_h_
#define vil3d_header_data_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief A base class for objects containing information from image headers.
// \author Tim Cootes - Manchester

#include <vcl_iosfwd.h>
#include <vcl_string.h>
#include <vcl_cassert.h>
#include <vil2/vil2_pixel_format.h>
#include <vil2/vil2_smart_ptr.h>

//: A base class for objects containing information from image headers.
//  This base contains data likely to be present in all headers
//  Derived classes will encapsulate extra data recorded in different
//  formats.
//
//  Design note: This is somewhat experimental at present.
class vil3d_header_data
{
protected:
  //: Number of columns.
  unsigned ni_;
  //: Number of rasters.
  unsigned nj_;
  //: Number of slices.
  unsigned nk_;
  //: Number of planes.
  unsigned nplanes_;

	//: Width of voxels in i direction
	double voxel_width_i_;

	//: Width of voxels in j direction
	double voxel_width_j_;

	//: Width of voxels in k direction
	double voxel_width_k_;

	//: Type of data in image
	vil2_pixel_format pixel_format_;

public:
  vil3d_header_data();

  // The destructor must be virtual so that the memory chunk is destroyed.
  virtual ~vil3d_header_data() {};

  //: Width
  unsigned ni()  const {return ni_;}
  //: Height
  unsigned nj()  const {return nj_;}
  //: Depth
  unsigned nk()  const {return nk_;}
  //: Number of planes
  unsigned nplanes() const {return nplanes_;}

	//: Width of voxels in i direction
	double voxel_width_i() const { return voxel_width_i_; }

	//: Width of voxels in j direction
	double voxel_width_j() const { return voxel_width_j_; }

	//: Width of voxels in k direction
	double voxel_width_k() const { return voxel_width_k_; }


  //: The number of voxels.
  unsigned long size() const { return ni_ * nj_ * nk_ * nplanes_; }

  //: Indicate size is ni x nj * nk
	void set_size(unsigned ni, unsigned nj, unsigned nk);

  //: resize to ni x nj * nk with nplanes planes.
  // If already correct size, this function returns quickly
  void set_size(unsigned ni, unsigned nj, unsigned nk, unsigned nplanes);

	//: Define voxel size
	void set_voxel_widths(double wi, double wj, double wk);

  //: Print a 1-line summary of contents
  virtual void print(vcl_ostream&) const;

  //: Return class name
  virtual vcl_string is_a() const;

  //: Return a description of the data pixel type.
  vil2_pixel_format pixel_format() const { return pixel_format_; }

	//: Define pixel format
	void set_pixel_format(vil2_pixel_format f);

  //: True if this is (or is derived from) class s
  virtual bool is_class(vcl_string const& s) const;

 private:
  // You probably should not use a vil3d_image_view in a vbl_smart_ptr, so the
  // ref functions are private
  friend class vil2_smart_ptr<vil3d_header_data>;
  void ref() { ++reference_count_; }
  void unref() {
    assert(reference_count_>0);
    if (--reference_count_<=0) delete this;}
  int reference_count_;
};

typedef vil2_smart_ptr<vil3d_header_data> vil3d_header_data_sptr;

//: Print a summary of contents
inline
vcl_ostream& operator<<(vcl_ostream& s, vil3d_header_data const& data) {
  data.print(s); return s;
}

#endif

