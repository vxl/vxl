#ifndef osl_harris_h
#define osl_harris_h
//:
//  \file
// \brief The harris corner detector
//
// \par Parameters :
// - corner_count_max - maximum no of corners required.
// - gauss_sigma      - sigma for the Gaussian smoothing.
// - relative minimum - a guidance term: first attempt will be to collect all
//                      corners whose corner strength is greater than (max
//                      strength * relative  minimum).
// - scale factor     - the 'k' from the harris auto-correlation expression.
//
// \author
//    Paul Beardsley, Robotics Research Group, Oxford University
// \date   16.10.95
//
// \verbatim
// Modifications:
//       First version generated from C code with minimal modification for C++/
//       Target data structures.
//   P.Vanroose  Mar97  corrected memory management (point_list_ & *free_*())
//   P.Vanroose  Aug97  point_list_ now safer (added AddPoint(); SetPointList() out of use now)
//   J.Mundy     Jan98  slight modified the interface to correspond to new style using parameter blocks and image dispatch
// \endverbatim

#include <vcl_iosfwd.h>
#include <vcl_vector.h>
#include <vcl_utility.h>
#include <vxl_config.h>
#include <vil1/vil1_memory_image_of.h>
#include <osl/osl_harris_params.h>
#include <osl/osl_roi_window.h>

//: A osl_harris object stores the internal buffers used by the harris corner detector.
class osl_harris
{
 public:
  osl_harris(osl_harris_params const & params) : image_w(0), image_h(0), params_(params) { }

  void compute(vil1_image const &image) {
    prepare_buffers(image.width(), image.height());
    compute_gradients(image);
    compute_2nd_moments();
    compute_cornerness();
    compute_corners();
  }

  void get_corners(vcl_vector<vcl_pair<float, float> > &) const;
  void get_corners(vcl_vector<float> &, vcl_vector<float> &) const;
  void save_corners(vcl_ostream &stream) const;
  void save_corners(char const *file) const;

  // these buffers persist between invocations so that
  // unnecessary allocation is not performed (a.stoddart).
  int image_w, image_h;

  // the input image, as a monochrome byte bitmap.
  vil1_memory_image_of<vxl_byte> image_buf;

  // gradient bitmaps.
  vil1_memory_image_of<int>      image_gradx_buf;
  vil1_memory_image_of<int>      image_grady_buf;

  // second moment matrix of the gradient vector.
  vil1_memory_image_of<float>    image_fxx_buf;
  vil1_memory_image_of<float>    image_fxy_buf;
  vil1_memory_image_of<float>    image_fyy_buf;

  // the cornerness response map and its maximum value.
  vil1_memory_image_of<float>    image_cornerness_buf;
  float corner_max;

  // local maximum map.
  vil1_memory_image_of<bool>     image_cornermax_buf;

  // region of interest ?
  osl_roi_window window_str;


  // These are the stages of algorithm. Clients can call a subset of
  // these manually in order to insert algorithms of their own choice.
  void prepare_buffers(int w, int h);
  void compute_gradients(vil1_image const &);
  void compute_2nd_moments();
  void compute_cornerness();
  void compute_corners();

 protected:
  osl_harris_params params_;
  vcl_vector<vcl_pair<float, float> > cc; // corners
 private:
  // these routines called by compute() :
  void do_non_adaptive(double *corner_min);
  void do_adaptive();
};

#endif // osl_harris_h
