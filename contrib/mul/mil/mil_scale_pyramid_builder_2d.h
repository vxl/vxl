// This is mul/mil/mil_scale_pyramid_builder_2d.h
#ifndef mil_scale_pyramid_builder_2d_h_
#define mil_scale_pyramid_builder_2d_h_
//:
// \file
// \brief Build scale image pyramids at any scale separation
// \author Ian Scott
//=======================================================================

#include <mil/mil_image_pyramid_builder.h>
#include <mil/mil_image_2d_of.h>
#include <vcl_iosfwd.h>

//: Build pyramids of mil_image_2d_of<vxl_byte> at any scale step

template <class T>
class mil_scale_pyramid_builder_2d : public mil_image_pyramid_builder
{
  //: Minimum size in X direction of top layer of pyramid.
  unsigned min_x_size_;

  //: Minimum size in Y direction of top layer of pyramid.
  unsigned min_y_size_;

  //: Maximum levels
  unsigned max_levels_;

  //: Checks pyramid has at least n levels of correct type
  void checkPyr(mil_image_pyramid& im_pyr,  int n_levels) const;

  //: Deletes all data in im_pyr
  void emptyPyr(mil_image_pyramid& im_pyr) const;

  //: The current scale step between pyramid levels
  double scale_step_;

 public:

  //: Set the minimum size of the top layer of the pyramid
  void set_min_size(unsigned X, unsigned Y) { min_y_size_ = Y; min_x_size_ = X;}

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
  virtual void build(mil_image_pyramid& im_pyr, const mil_image& im) const;

  //: Dflt ctor
  mil_scale_pyramid_builder_2d();

  //: Construct with given scale_step
  mil_scale_pyramid_builder_2d(double scale_step);

  //: Destructor
  ~mil_scale_pyramid_builder_2d();

  //: get the current scale step;
  virtual double scale_step() const { return scale_step_; };

  //: Set the Scale step
  void set_scale_step(double scale_step);

  //: Smooth and subsample src_im to produce dest_im
  //  Applies an appropriate filter in x and y, then sub-samples
  //  at the chosen scaleStep;
  void scale_reduce(mil_image_2d_of<T>& dest_im,
                    mil_image_2d_of<T>const& src_im) const;

  //: Extend pyramid
  // The first layer of the pyramid must already be set.
  virtual void extend(mil_image_pyramid& image_pyr) const;

  //: Smooth and subsample src_im to produce dest_im
  //  Applies 5 element FIR filter in x and y, then samples
  //  Assumes dest_im has sufficient data allocated
  void scale_reduce(T* dest_im, int dest_ystep,
                    const T* src_im,
                    int src_nx, int src_ny,
                    int dest_nx, int dest_ny,
                    int src_ystep) const;

  mil_image_pyramid_builder* clone() const;

  //: Version number for I/O
  short version_no() const ;

  //: Name of the class
  virtual vcl_string is_a() const ;

  //: Does the name of the class match the argument?
  virtual bool is_class(vcl_string const& s) const;

  //: Print class to os
  virtual void print_summary(vcl_ostream&) const;

  //: Save class to binary file stream
  virtual void b_write(vsl_b_ostream& bfs) const;

  //: Load class from binary file stream
  virtual void b_read(vsl_b_istream& bfs);
};

#define MIL_SCALE_PYRAMID_BUILDER_2D_INSTANTIATE(T) \
extern "please #include mil/mil_scale_pyramid_builder_2d.txx instead"

#endif // mil_scale_pyramid_builder_2d_h_
