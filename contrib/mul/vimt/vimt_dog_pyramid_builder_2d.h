// This is mul/vimt/vimt_dog_pyramid_builder_2d.h
#ifndef vimt_dog_pyramid_builder_2d_h_
#define vimt_dog_pyramid_builder_2d_h_
//:
//  \file
//  \brief Build difference of gaussian pyramids of vimt_image_2d_of<T>
//  \author Tim Cootes

#include <vsl/vsl_binary_io.h>
#include <vcl_string.h>
#include <vimt/vimt_image_2d_of.h>
#include <vimt/vimt_image_pyramid_builder.h>

//: Build difference of gaussian pyramids of vimt_image_2d_of<T>
//  Computes each layer of a pyramid by smoothing
//  then computing the difference from the original image.
//  The smoothed is then subsampled using a reduction factor of 1.5
//  (ie each level is 2/3 the size of the level below) and
//  used to produced the next level.
//
//  This is useful for finding locally interesting points and their
//  associated scales - see "Object Recognition from Scale Invariant Features"
//  D.Lowe, ICCV1999, pp.1150-1157.
template <class T>
class vimt_dog_pyramid_builder_2d : public vimt_image_pyramid_builder
{
  int max_levels_;

  mutable vimt_image_2d_of<T> work_im_;

  //:Minimum size in X direction of top layer of pyramid.
  unsigned min_x_size_;

  //:Minimum size in Y direction of top layer of pyramid.
  unsigned min_y_size_;

 protected:
  //: Checks pyramid has at least n levels of correct type
  void check_pyr(vimt_image_pyramid& im_pyr,  int n_levels) const;

  //: Deletes all data in im_pyr
  void empty_pyr(vimt_image_pyramid& im_pyr) const;

 public:
  //: Dflt ctor
  vimt_dog_pyramid_builder_2d();

  //: Destructor
  virtual ~vimt_dog_pyramid_builder_2d();

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

  //: Build difference of gaussian pyramid and a gaussian pyramid
  //  If abs_diff, then use absolute difference of gaussians
  void build_dog(vimt_image_pyramid& dog_pyr,
                 vimt_image_pyramid& smooth_pyr,
                 const vimt_image& im, bool abs_diff=true) const;

  //: Build pyramid
  virtual void build(vimt_image_pyramid& dog_pyr, const vimt_image&) const;

  //: Extend pyramid (not implemented)
  virtual void extend(vimt_image_pyramid&) const;

  //: Smooth and subsample src_im to produce dest_im.
  //  Applies filter in x and y, then samples every other pixel.
  //  Filter width defined by set_filter_width()
  void gauss_reduce(const vimt_image_2d_of<T>& src_im,
                    vimt_image_2d_of<T>& dest_im) const;

  //: Scale step between levels
  virtual double scale_step() const;

  //: Get the minimum Y size of the top layer of the pyramid.
  // Defaults to 5.
  unsigned min_y_size() const { return min_y_size_;}

  //: Get the minimum Y size of the top layer of the pyramid.
  // Defaults to 5.
  unsigned min_x_size() const { return min_x_size_;}

  //: Set the minimum size of the top layer of the pyramid
  virtual void set_min_size(unsigned X, unsigned Y) { min_y_size_ = Y; min_x_size_ = X;}

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

#endif // vimt_dog_pyramid_builder_2d_h_
