#ifndef vsl_harris_h
#define vsl_harris_h

//: The harris corner detector.
// Parameters :
//   corner_count_max - maximum no of corners required.
//   gauss_sigma      - sigma for the Gaussian smoothing.
//   relative minimum - a guidance term: first attempt will be to collect all
//                      corners whose corner strength is greater than (max 
//                      strength * relative  minimum).
//   scale factor     - the 'k' from the harris auto-correlation expression.
//
// Author: Paul Beardsley, Robotics Research Group, Oxford University 
// Created: 16.10.95
//
// History:
//       First version generated from C code with minimal modification for C++/
//	 Target data structures.
//   P.Vanroose  Mar97  corrected memory management (_point_list & *_free_*())
//   P.Vanroose  Aug97  _point_list now safer (added AddPoint(); SetPointList() out of use now)
//   J.Mundy     Jan98  slight modified the interface to correspond to new
//               style  using parameter blocks and image dispatch
//   F.Schaffalitzky
//               Feb 1998 rewrite.
//               Feb 2000 yet another rewrite, this time for vxl.

#include <vsl/vsl_harris_params.h>
#include <vsl/vsl_roi_window.h>
#include <vcl/vcl_vector.h>
#include <vil/vil_fwd.h>

//: A vsl_harris object stores the internal buffers used by the harris corner detector.
class vsl_harris : public vsl_harris_params {
public:
  vsl_harris(vsl_harris_params const & params);
  ~vsl_harris();
  
  void compute(vil_image image);
  void get_corners(vcl_vector<float> &, vcl_vector<float> &) const;
  void save_corners(char const *file) const;

  //------------------------------ computed things ------------------------------

  // the input image, as a monochrome byte bitmap.
  vil_byte_buffer *image_ptr;

  // gradient bitmaps.
  vil_int_buffer *image_gradx_ptr;
  vil_int_buffer *image_grady_ptr;

  // second moment matrix of the gradient vector.
  vil_float_buffer *image_fxx_ptr;
  vil_float_buffer *image_fxy_ptr;
  vil_float_buffer *image_fyy_ptr;

  // the cornerness response map and its maximum value.
  vil_float_buffer *image_cornerness_ptr;
  float corner_max;

  // local maximum map.
  vil_bool_buffer *image_cornermax_ptr;

  // region of interest ?
  vsl_roi_window window_str;

private:
  //------------------------------ private ------------------------------

  vsl_harris_params &_params; // FIXME
  unsigned image_w, image_h;  // size of input image. ***

  //
  void init_module (vil_image image);
  void uninit_module ();

  // these routines driven by compute() :
  void compute_response();

  void do_non_adaptive(double* corner_min);
  void do_adaptive();

  int  dr_store_corners (float corner_min);
  vcl_vector<float> cx, cy; // 
};

#endif
