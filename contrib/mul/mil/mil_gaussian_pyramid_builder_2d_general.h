// This is mul/mil/mil_gaussian_pyramid_builder_2d_general.h
#ifndef mil_gaussian_pyramid_builder_2d_general_h_
#define mil_gaussian_pyramid_builder_2d_general_h_
//:
// \file
// \brief Build Gaussian image pyramids at any scale separation
// \author Ian Scott
//=======================================================================

#include <mil/mil_gaussian_pyramid_builder_2d.h>
#include <mil/mil_image_2d_of.h>
#include <vcl_iosfwd.h>

//: Build pyramids of mil_image_2d_of<vxl_byte> at any scale step

template <class T>
class mil_gaussian_pyramid_builder_2d_general : public mil_gaussian_pyramid_builder_2d<T>
{
  //: the current scale step between pyramid levels
  double scale_step_;
  //: image workspace
  mutable mil_image_2d_of<T> worka_;
  //: image workspace
  mutable mil_image_2d_of<T> workb_;

  //: Filter tap value
  // The value of the two outside elements of the 5-tap 1D FIR filter
  double filt2_;
  //: Filter tap value
  // The value of elements 2 and 4 of the 5-tap 1D FIR filter
  double filt1_;
  //: Filter tap value
  // The value of the central element of the 5-tap 1D FIR filter
  double filt0_;

  //: Filter tap value
  // The value of the first element of the 3 tap 1D FIR filter for use at the edge of the window
  // Corresponds to the filt2_ elements in a symmetrical filter
  double filt_edge2_;
  //: Filter tap value
  // The value of the second element of the 3 tap 1D FIR filter for use at the edge of the window
  // Corresponds to the filt1_ elements in a symmetrical filter
  double filt_edge1_;
  //: Filter tap value
  // The value of the third element of the 3 tap 1D FIR filter for use at the edge of the window
  // Corresponds to the filt0_ element in a symmetrical filter
  double filt_edge0_;

  //: Filter tap value
  // The value of the first element of the 4 tap 1D FIR filter for use 1 pixel away the edge of the window
  // Corresponds to the filt2_ elements in a symmetrical filter
  double filt_pen_edge2_;
  //: Filter tap value
  // The value of the second element of the 4 tap 1D FIR filter for use 1 pixel away the edge of the window
  // Corresponds to the filt1_ elements in a symmetrical filter
  double filt_pen_edge1_;
  //: Filter tap value
  // The value of the third element of the 4 tap 1D FIR filter for use 1 pixel away the edge of the window
  // Corresponds to the filt0_ elements in a symmetrical filter
  double filt_pen_edge0_;
  //: Filter tap value
  // The value of the fourth element of the 4 tap 1D FIR filter for use 1 pixel away the edge of the window
  // Corresponds to the filt1_ elements in a symmetrical filter
  double filt_pen_edge_n1_;

 public:
  //: Build pyramid
  virtual void build(mil_image_pyramid& im_pyr, const mil_image& im) const;

  //: Dflt ctor
  mil_gaussian_pyramid_builder_2d_general();

  //: Construct with given scale_step
  mil_gaussian_pyramid_builder_2d_general(double scale_step);

  //: Destructor
  ~mil_gaussian_pyramid_builder_2d_general();

  //: get the current scale step;
  double scale_step() const { return scale_step_; };

  //: Set the Scale step
  void set_scale_step(double scale_step);

  //: Smooth and subsample src_im to produce dest_im
  //  Applies an appropriate filter in x and y, then sub-samples
  //  at the chosen scaleStep;
  void gauss_reduce(mil_image_2d_of<T>& dest_im,
                    const mil_image_2d_of<T>& src_im) const;

  //: Extend pyramid
  // The first layer of the pyramid must already be set.
  void extend(mil_image_pyramid& image_pyr) const;

  //: Smooth and subsample src_im to produce dest_im
  //  Applies 5 element FIR filter in x and y, then samples
  //  Assumes dest_im has sufficient data allocated
  static void gauss_reduce(T* dest_im, int dest_ystep,
                           const T* src_im,
                           int src_nx, int src_ny,
                           int dest_nx, int dest_ny,
                           int src_ystep,
                           T* worka_im, T* workb_im, int work_ystep,
                           const mil_gaussian_pyramid_builder_2d_general<T>*);

  mil_image_pyramid_builder* clone() const;
  //: Version number for I/O
  short version_no() const ;
  //: Name of the class
  virtual vcl_string is_a() const ;
  //: Does the name of the class match the argument?
  virtual bool is_class(vcl_string const& s) const;
  //: Print class to os
  void print_summary(vcl_ostream&) const;
  //: Save class to binary file stream
  void b_write(vsl_b_ostream& bfs) const;
  //: Load class from binary file stream
  void b_read(vsl_b_istream& bfs);
};

#define MIL_GAUSSIAN_PYRAMID_BUILDER_2D_GENERAL_INSTANTIATE(T) \
extern "please #include mil/mil_gaussian_pyramid_builder_2d_general.txx instead"

#endif // mil_gaussian_pyramid_builder_2d_general_h_
