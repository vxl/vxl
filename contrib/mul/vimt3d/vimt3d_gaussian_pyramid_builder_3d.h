// This is mul/vimt3d/vimt3d_gaussian_pyramid_builder_3d.h
#ifndef vimt3d_gaussian_pyramid_builder_3d_h_
#define vimt3d_gaussian_pyramid_builder_3d_h_
//:
// \file
// \brief Build Gaussian pyramids of vimt3d_image_3d_of<T>
// \author Tim Cootes

#include <vsl/vsl_binary_io.h>
#include <vcl_string.h>
#include <vimt3d/vimt3d_image_3d_of.h>
#include <vimt/vimt_image_pyramid_builder.h>

//: Build Gaussian pyramids of vimt3d_image_3d_of<T>
//  Smooth with a Gaussian filter (1-5-8-5-1 by default)
//  and subsample so that image at level i-1 is half the
//  size of that at level i
//
//  Note that if set_uniform_reduction(false) and width of z pixels much more
//  than that in x and y,  then only smooth and sub-sample in x and y.
//  This is useful for images with non-isotropic sampling (eg MR images)
//  Similarly, if either x or y has significantly larger sample spacing,
//  the others will be smoothed first.  Note, currently only works for one
//  dimension being significantly larger than the other two.
template <class T>
class vimt3d_gaussian_pyramid_builder_3d : public vimt_image_pyramid_builder
{
  int max_levels_;

  //: When true, subsample in x,y,z every time.
  //  When not true and width of z pixels much more than that in x and y,
  //  then only smooth and sub-sample in x and y
  bool uniform_reduction_;

  mutable vil3d_image_view<T> work_im1_,work_im2_;

  //: Filter width (usually 5 for a 15851 filter, or 3 for a 121 filter)
  unsigned filter_width_;

  //:Minimum size in X direction of top layer of pyramid.
  unsigned min_x_size_;

  //:Minimum size in Y direction of top layer of pyramid.
  unsigned min_y_size_;

  //:Minimum size in Z direction of top layer of pyramid.
  unsigned min_z_size_;

 protected:
  //: Checks pyramid has at least n levels of correct type
  void checkPyr(vimt_image_pyramid& im_pyr,  int n_levels) const;

  //: Deletes all data in im_pyr
  void emptyPyr(vimt_image_pyramid& im_pyr) const;

  //: Select number of levels to use
  int n_levels(const vimt3d_image_3d_of<T>& base_image) const;

  //: Compute real world size of pixel in image
  void get_pixel_size(double &dx, double& dy, double& dz,
                      const vimt3d_image_3d_of<T>& image) const;

 public:
  //: Dflt ctor
  vimt3d_gaussian_pyramid_builder_3d();

  //: Destructor
  virtual ~vimt3d_gaussian_pyramid_builder_3d();

  //: Current filter width
  unsigned filter_width() const { return filter_width_; }

  //: Set current filter width (must be 5 at present)
  void set_filter_width(unsigned);

  //: When true, subsample in x,y,z every time.
  //  When not true and width of z pixels much more than that in x and y,
  //  then only smooth and sub-sample in x and y
  bool uniform_reduction() const { return uniform_reduction_; }

  //: When true, subsample in x,y,z every time.
  //  When not true and width of z pixels much more than that in x and y,
  //  then only smooth and sub-sample in x and y
  void set_uniform_reduction(bool b) { uniform_reduction_ = b; }

  //: Create new (empty) pyramid on heap.
  //  Caller responsible for its deletion
  virtual vimt_image_pyramid* new_image_pyramid() const;

  //: Define maximum number of levels to build.
  //  Limits levels built in subsequent calls to build()
  //  Useful efficiency measure.  As build() only takes
  //  a shallow copy of the original image, using
  //  max_l=1 avoids any copying or smoothing.
  virtual void set_max_levels(int max_l);

  //: Get the current maximum number levels allowed
  virtual int max_levels() const;

  //: Build pyramid
  virtual void build(vimt_image_pyramid&, const vimt_image&) const;

  //: Extend pyramid
  // The first layer of the pyramid must already be set.
  virtual void extend(vimt_image_pyramid&) const;

  //: Smooth and subsample src_im to produce dest_im.
  //  Applies filter in x,y and z, then samples every other pixel.
  //  Filter width defined by set_filter_width()
  void gauss_reduce(vimt3d_image_3d_of<T>& dest_im,
                    const vimt3d_image_3d_of<T>& src_im) const;

  //: Scale step between levels
  virtual double scale_step() const;

  //: Get the minimum X size of the top layer of the pyramid.
  // Defaults to 5.
  unsigned min_x_size() const { return min_x_size_;}

  //: Get the minimum Y size of the top layer of the pyramid.
  // Defaults to 5.
  unsigned min_y_size() const { return min_y_size_;}

  //: Get the minimum Z size of the top layer of the pyramid.
  // Defaults to 5.
  unsigned min_z_size() const { return min_z_size_;}

  //: Set the minimum size of the top layer of the pyramid
  virtual void set_min_size(unsigned X, unsigned Y, unsigned Z);

  //: Version number for I/O
  short version_no() const;

  //: Name of the class
  virtual vcl_string is_a() const;

  //: Does the name of the class match the argument?
  virtual bool is_class(vcl_string const& s) const;

  //: Create a copy on the heap and return base class pointer
  virtual vimt_image_pyramid_builder* clone() const;

  //: Print class to os
  virtual void print_summary(vcl_ostream& os) const;

  //: Save class to binary file stream
  virtual void b_write(vsl_b_ostream& bfs) const;

  //: Load class from binary file stream
  virtual void b_read(vsl_b_istream& bfs);
};

#define VIMT3D_GAUSSIAN_PYRAMID_BUILDER_3D_INSTANTIATE(T) \
extern "please #include vimt3d/vimt3d_gaussian_pyramid_builder_3d.txx instead"

#endif // vimt3d_gaussian_pyramid_builder_3d_h_
