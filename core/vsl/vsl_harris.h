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

#include "harris_params.h"
#include "harris_internals.h"
#include <vcl/vcl_vector.h>

class vil_image_ref;
template <class T> class vil_memory_image_of;

//: A vsl_harris object stores the internal buffers used by the harris corner detector.
class vsl_harris : public harris_params {
public:
  vsl_harris(harris_params const & params);
  ~vsl_harris();
  
  void compute(vil_image_ref image);
  void get_corners(vcl_vector<float> &, vcl_vector<float> &) const;
  void save_corners(char const *file) const;

  // typedefs
  typedef unsigned char byte;
  typedef vil_memory_image_of<bool>  bool_map;
  typedef vil_memory_image_of<byte>  byte_map;
  typedef vil_memory_image_of<int>   int_map;
  typedef vil_memory_image_of<float> float_map;

  //------------------------------ computed things ------------------------------

  // the input image, as a monochrome byte bitmap.
  byte_map *image_ptr;

  // gradient bitmaps.
  int_map *image_gradx_ptr;
  int_map *image_grady_ptr;

  // second moment matrix of the gradient vector.
  float_map *image_fxx_ptr;
  float_map *image_fxy_ptr;
  float_map *image_fyy_ptr;

  // the cornerness response map and its maximum value.
  float_map *pixel_cornerness;
  float corner_max;

  // local maximum map.
  bool_map *image_corner_max_ptr;

  // region of interest ?
  GL_WINDOW_STR window_str;
  
private:
  harris_params &_params; // FIXME
  unsigned image_w, image_h; // size of input image. ***

  //
  void init_module (vil_image_ref image);
  void uninit_module ();

  // these routines driven by compute() :
  void compute_response();

  void do_non_adaptive(double corner_min);
  void do_adaptive();

  int  dr_store_corners (float corner_min);
  vcl_vector<float> cx, cy; // 
};

#endif
