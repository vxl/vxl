// This is mul/vimt/vimt_scale_pyramid_builder_2d.h
#ifndef vimt_scale_pyramid_builder_2d_h_
#define vimt_scale_pyramid_builder_2d_h_
//:
// \file
// \brief Build scale image pyramids at any scale separation
// \author Ian Scott
//=======================================================================

#include <iostream>
#include <iosfwd>
#include <vimt/vimt_image_pyramid_builder.h>
#include <vimt/vimt_image_2d_of.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Build pyramids of vimt_image_2d_of<vxl_byte> at any scale step
template <class T>
class vimt_scale_pyramid_builder_2d : public vimt_image_pyramid_builder
{
  //: Minimum size in X direction of top layer of pyramid.
  unsigned min_x_size_;

  //: Minimum size in Y direction of top layer of pyramid.
  unsigned min_y_size_;

  //: Maximum levels
  unsigned max_levels_;

  //: Checks pyramid has at least n levels of correct type
  void checkPyr(vimt_image_pyramid& im_pyr, int n_levels) const;

  //: Deletes all data in im_pyr
  void emptyPyr(vimt_image_pyramid& im_pyr) const;

  //: The current scale step between pyramid levels
  double scale_step_;

 public:

  //: Set the minimum size of the top layer of the pyramid
  void set_min_size(unsigned X, unsigned Y) { min_y_size_=Y; min_x_size_=X; }

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
  void build(vimt_image_pyramid& im_pyr, const vimt_image& im) const override;

  //: Dflt ctor
  vimt_scale_pyramid_builder_2d();

  //: Construct with given scale_step
  vimt_scale_pyramid_builder_2d(double scale_step);

  //: Destructor
  ~vimt_scale_pyramid_builder_2d() override;

  //: get the current scale step;
  double scale_step() const override { return scale_step_; }

  //: Set the Scale step
  void set_scale_step(double scale_step);

  //: Smooth and subsample src_im to produce dest_im
  //  Applies an appropriate filter in x and y, then sub-samples
  //  at the chosen scaleStep;
  void scale_reduce(vimt_image_2d_of<T>& dest_im,
                    vimt_image_2d_of<T>const& src_im) const;

  //: Extend pyramid
  // The first layer of the pyramid must already be set.
  void extend(vimt_image_pyramid& image_pyr) const override;

  //: Smooth and subsample src_im to produce dest_im
  //  Applies 5 element FIR filter in x and y, then samples
  //  Assumes dest_im has sufficient data allocated
  void scale_reduce(T* dest_im, std::ptrdiff_t dest_ystep,
                    const T* src_im,
                    int src_nx, int src_ny,
                    int dest_nx, int dest_ny,
                    std::ptrdiff_t src_istep, std::ptrdiff_t src_jstep) const;

  vimt_image_pyramid_builder* clone() const override;

  //: Version number for I/O
  short version_no() const;

  //: Name of the class
  std::string is_a() const override;

  //: Does the name of the class match the argument?
  bool is_class(std::string const& s) const override;

  //: Print class to os
  void print_summary(std::ostream&) const override;

  //: Save class to binary file stream
  void b_write(vsl_b_ostream& bfs) const override;

  //: Load class from binary file stream
  void b_read(vsl_b_istream& bfs) override;
};

#endif // vimt_scale_pyramid_builder_2d_h_
