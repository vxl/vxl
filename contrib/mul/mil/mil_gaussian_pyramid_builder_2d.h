// This is mul/mil/mil_gaussian_pyramid_builder_2d.h
#ifndef mil_gaussian_pyramid_builder_2d_h_
#define mil_gaussian_pyramid_builder_2d_h_
//:
// \file
// \brief Build Gaussian pyramids of mil_image_2d_of<T>
// \author Tim Cootes

#include <vsl/vsl_binary_io.h>
#include <vcl_string.h>
#include <mil/mil_image_2d_of.h>
#include <mil/mil_image_pyramid_builder.h>

//: Build Gaussian pyramids of mil_image_2d_of<T>
//  Smooth with a Gaussian filter (1-5-8-5-1 by default)
//  and subsample so that image at level i-1 is half the
//  size of that at level i
template <class T>
class mil_gaussian_pyramid_builder_2d : public mil_image_pyramid_builder
{
  int max_levels_;

  mutable mil_image_2d_of<T> work_im_;

  //: Filter width (usually 5 for a 15851 filter, or 3 for a 121 filter)
  unsigned filter_width_;

  //:Minimum size in X direction of top layer of pyramid.
  unsigned minXSize_;

  //:Minimum size in Y direction of top layer of pyramid.
  unsigned minYSize_;

 protected:
  //: Checks pyramid has at least n levels of correct type
  void checkPyr(mil_image_pyramid& im_pyr,  int n_levels) const;

  //: Deletes all data in im_pyr
  void emptyPyr(mil_image_pyramid& im_pyr) const;

  //: Smooth and subsample src_im to produce dest_im.
  //  Applies 1-5-8-5-1 filter in x and y, then samples
  //  every other pixel.
  void gauss_reduce_15851(mil_image_2d_of<T>& dest_im,
                          mil_image_2d_of<T>const& src_im) const;

  //: Smooth and subsample src_im to produce dest_im.
  //  Applies 1-2-1 filter in x and y, then samples
  //  every other pixel.
  void gauss_reduce_121(mil_image_2d_of<T>& dest_im,
                        mil_image_2d_of<T>const& src_im) const;

 public:
  //: Dflt ctor
  mil_gaussian_pyramid_builder_2d();

  //: Destructor
  virtual ~mil_gaussian_pyramid_builder_2d();

  //: Current filter width
  unsigned filter_width() const { return filter_width_; }

  //: Set current filter width (must be 3 or 5 at present)
  void set_filter_width(unsigned);

  //: Create new (empty) pyramid on heap.
  //  Caller responsible for its deletion
  virtual mil_image_pyramid* newImagePyramid() const;

  //: Define maximum number of levels to build.
  //  Limits levels built in subsequent calls to build()
  //  Useful efficiency measure.  As build() only takes
  //  a shallow copy of the original image, using
  //  max_l=1 avoids any copying or smoothing.
  virtual void setMaxLevels(int max_l);

  //: Get the current maximum number levels allowed
  virtual int maxLevels() const;

  //: Build pyramid
  virtual void build(mil_image_pyramid&, const mil_image&) const;

  //: Extend pyramid.
  // The first layer of the pyramid must already be set.
  // Scale steps must be equal.
  virtual void extend(mil_image_pyramid&) const;

  //: Smooth and subsample src_im to produce dest_im.
  //  Applies filter in x and y, then samples every other pixel.
  //  Filter width defined by set_filter_width()
  void gauss_reduce(mil_image_2d_of<T>& dest_im,
                    const mil_image_2d_of<T>& src_im) const;

  //: Scale step between levels
  virtual double scale_step() const;

  //: Get the minimum Y size of the top layer of the pyramid.
  // Defaults to 5.
  unsigned min_y_size() const { return minYSize_;}

  //: Get the minimum Y size of the top layer of the pyramid.
  // Defaults to 5.
  unsigned min_x_size() const { return minXSize_;}

  //: Set the minimum size of the top layer of the pyramid
  virtual void set_min_size(unsigned X, unsigned Y) { minYSize_ = Y; minXSize_ = X;}

  //: Version number for I/O
  short version_no() const;

  //: Name of the class
  virtual vcl_string is_a() const;

  //: Does the name of the class match the argument?
  virtual bool is_class(vcl_string const& s) const;

  //: Create a copy on the heap and return base class pointer
  virtual mil_image_pyramid_builder* clone() const;

  //: Print class to os
  virtual void print_summary(vcl_ostream& os) const;

  //: Save class to binary file stream
  virtual void b_write(vsl_b_ostream& bfs) const;

  //: Load class from binary file stream
  virtual void b_read(vsl_b_istream& bfs);
};

#define MIL_GAUSSIAN_PYRAMID_BUILDER_2D_INSTANTIATE(T) \
extern "please #include mil/mil_gaussian_pyramid_builder_2d.txx instead"

#endif // mil_gaussian_pyramid_builder_2d_h_
