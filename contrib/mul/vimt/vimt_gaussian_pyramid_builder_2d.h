// This is mul/vimt/vimt_gaussian_pyramid_builder_2d.h
#ifndef vimt_gaussian_pyramid_builder_2d_h_
#define vimt_gaussian_pyramid_builder_2d_h_
//:
// \file
// \brief Build gaussian pyramids of vimt_image_2d_of<T>
// \author Tim Cootes

#include <string>
#include <iostream>
#include <iosfwd>
#include <vimt/vimt_image_2d_of.h>
#include <vimt/vimt_image_pyramid_builder.h>
#include <vsl/vsl_binary_io.h>
#include <vcl_compiler.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Build gaussian pyramids of vimt_image_2d_of<T>
//  Smooth with a gaussian filter (1-5-8-5-1 by default)
//  and subsample so that image at level i-1 is half the
//  size of that at level i
template <class T>
class vimt_gaussian_pyramid_builder_2d : public vimt_image_pyramid_builder
{
  int max_levels_;

  mutable vimt_image_2d_of<T> work_im_;

  //: Filter width (usually 5 for a 15851 filter, or 3 for a 121 filter)
  unsigned filter_width_;

  //:Minimum size in X direction of top layer of pyramid.
  unsigned minXSize_;

  //:Minimum size in Y direction of top layer of pyramid.
  unsigned minYSize_;

 protected:
  //: Checks pyramid has at least n levels of correct type
  void check_pyr(vimt_image_pyramid& im_pyr,  int n_levels) const;

  //: Deletes all data in im_pyr
  void empty_pyr(vimt_image_pyramid& im_pyr) const;

 public:
  //: Dflt ctor
  vimt_gaussian_pyramid_builder_2d();

  //: Destructor
  ~vimt_gaussian_pyramid_builder_2d() override;

  //: Current filter width
  unsigned filter_width() const { return filter_width_; }

  //: Set current filter width (must be 3 or 5 at present)
  void set_filter_width(unsigned);

  //: Create new (empty) pyramid on heap.
  //  Caller responsible for its deletion
  vimt_image_pyramid* new_image_pyramid() const override;

  //: Define maximum number of levels to build.
  //  Limits levels built in subsequent calls to build()
  //  Useful efficiency measure.  As build() only takes
  //  a shallow copy of the original image, using
  //  max_l=1 avoids any copying or smoothing.
  void set_max_levels(int max_l) override;

  //: Get the current maximum number levels allowed
  int max_levels() const override;

  //: Build pyramid
  void build(vimt_image_pyramid&, const vimt_image&) const override;

  //: Extend pyramid.
  // The first layer of the pyramid must already be set.
  // Scale steps must be equal.
  void extend(vimt_image_pyramid&) const override;

  //: Smooth and subsample src_im to produce dest_im.
  //  Applies filter in x and y, then samples every other pixel.
  //  Filter width defined by set_filter_width()
  void gauss_reduce(const vimt_image_2d_of<T>& src_im,
                    vimt_image_2d_of<T>& dest_im) const;

  //: Scale step between levels
  double scale_step() const override;

  //: Get the minimum Y size of the top layer of the pyramid.
  // Defaults to 5.
  unsigned min_y_size() const { return minYSize_;}

  //: Get the minimum X size of the top layer of the pyramid.
  // Defaults to 5.
  unsigned min_x_size() const { return minXSize_;}

  //: Set the minimum size of the top layer of the pyramid
  virtual void set_min_size(unsigned X, unsigned Y) { minYSize_ = Y; minXSize_ = X;}

  //: Version number for I/O
  short version_no() const;

  //: Name of the class
  std::string is_a() const override;

  //: Does the name of the class match the argument?
  bool is_class(std::string const& s) const override;

  //: Create a copy on the heap and return base class pointer
  vimt_image_pyramid_builder* clone() const override;

  //: Print class to os
  void print_summary(std::ostream& os) const override;

  //: Save class to binary file stream
  void b_write(vsl_b_ostream& bfs) const override;

  //: Load class from binary file stream
  void b_read(vsl_b_istream& bfs) override;
};

#endif // vimt_gaussian_pyramid_builder_2d_h_
