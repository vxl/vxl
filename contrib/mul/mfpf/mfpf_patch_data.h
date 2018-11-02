#ifndef mfpf_patch_data_h_
#define mfpf_patch_data_h_
//:
// \file
// \brief Defines region size, shape, and form of model to use
// \author Tim Cootes

#include <iostream>
#include <iosfwd>
#include <mfpf/mfpf_point_finder_builder.h>
#include <mfpf/mfpf_region_definer.h>
#include <mbl/mbl_cloneable_ptr.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Defines region size, shape, and form of model to use
//  Object to hold information about how to set up an
//  mfpf_point_finder (or multi-res. version) given sets
//  of points on each image.  The position/size of the region
//  is defined relative to the points.  The type of finder
//  to use, and details of how to set it up are also included.
//
//  The main use of the object is to parse text files defining
//  sets of patches for more complex models.
//
//  Example of text than can be parsed by set_from_stream:
// \code
// {
//   name: Test1
//   min_width: 8
//   max_width: 10
//   region: mfpf_region_about_lineseg: { ... }
//   builder: mfpf_norm_corr2d_builder: { ... }
// }
// \endcode
class mfpf_patch_data
{
 protected:
  //: Name of this patch
  std::string name_;

  //: Minimum number of samples along one dimension of patch
  unsigned min_width_;

  //: Maximum number of samples along one dimension of patch
  unsigned max_width_;

  //: Object to define region given a set of points
  mbl_cloneable_ptr<mfpf_region_definer> definer_;

  //: Builder to be used for this patch
  mbl_cloneable_ptr<mfpf_point_finder_builder> builder_;

 public:
  //: Dflt ctor
  mfpf_patch_data();

  //: Destructor
  virtual ~mfpf_patch_data();

  //: Name of this patch
  std::string name() const { return name_; }

  //: Minimum number of samples along one dimension of patch
  unsigned min_width() const { return min_width_; }

  //: Maximum number of samples along one dimension of patch
  unsigned max_width() const { return max_width_; }

  //: Object to define region given a set of points
  mfpf_region_definer& definer() { return definer_; }

  //: Builder to be used for this patch
  mfpf_point_finder_builder& builder() { return builder_; }

  //: Initialise from a text stream
  bool set_from_stream(std::istream &is);

    //: Version number for I/O
  short version_no() const;

    //: Name of the class
  virtual std::string is_a() const;

    //: Print class to os
  virtual void print_summary(std::ostream& os) const;

    //: Save class to binary file stream
  virtual void b_write(vsl_b_ostream& bfs) const;

    //: Load class from binary file stream
  virtual void b_read(vsl_b_istream& bfs);
};

//: Stream output operator for class reference
std::ostream& operator<<(std::ostream& os,const mfpf_patch_data& b);

//: Binary file stream output operator for class reference
void vsl_b_write(vsl_b_ostream& bfs, const mfpf_patch_data& b);

//: Binary file stream input operator for class reference
void vsl_b_read(vsl_b_istream& bfs, mfpf_patch_data& b);

// ======= Functions for sets of mfpf_patch_data =======

//: Reads in a list of mfpf_patch_data objects from a text stream
//  Format: "{ region: { ... } region: { ... } .... }"
//  Throws an mbl_exception_parse_error if it fails.
void mfpf_read_from_stream(std::istream &is,
                           std::vector<mfpf_patch_data>& data);

#endif // mfpf_patch_data_h_
