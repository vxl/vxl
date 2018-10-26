// This is brl/bseg/brip/brip_gain_offset_solver.h
#ifndef brip_gain_offset_solver_h_
#define brip_gain_offset_solver_h_
//-----------------------------------------------------------------------------
//:
// \file
// \author J.L. Mundy
// \date February 26, 2012
// \brief Finds a gain and offset intensity map that minimizes squared difference between images.
//
// In order to accurately compare images that have corresponding pixel
// intensities it is necessary to adjust the gain and offset of the test
// image to match the model image as closely as possible. It is assumed
// that both the model and test images are the same size and corresponding
// pixels represent the same scene surface element, i.e. should have the
// same intensity
//
// \verbatim
//  Modifications
//   none yet
// \endverbatim
//
//-----------------------------------------------------------------------------
#include <vil/vil_image_view.h>

class brip_gain_offset_solver
{
 public:

  brip_gain_offset_solver() : ni_(0), nj_(0), gain_(1.0f), offset_(0.0f),
    t_mask_(false), m_mask_(false), n_valid_pix_(0) {}

  //: constructor with images, which are used in entirety
  brip_gain_offset_solver(vil_image_view<float> const& model_image,
                          vil_image_view<float> const& test_image);

  //: constructor with images and masks. pixels with mask(i,j)=true are valid
  brip_gain_offset_solver(vil_image_view<float> const& model_image,
                          vil_image_view<float> const& test_image,
                          vil_image_view<unsigned char> const& model_mask,
                          vil_image_view<unsigned char> const& test_mask);

  ~brip_gain_offset_solver() = default;

  //: set images after default construction
  void set_model_image(vil_image_view<float> const& image) {model_image_ = image;}
  void set_test_image (vil_image_view<float> const& image) {test_image_ = image;}
  //: set masks to define valid pixels in each image. Otherwise entire image is used
  void set_model_mask(vil_image_view<unsigned char> const& mask) {model_mask_ = mask;}
  void set_test_mask (vil_image_view<unsigned char> const& mask) {test_mask_ = mask;}

  //: accessors
  unsigned ni() const {return ni_;}
  unsigned nj() const {return nj_;}
  float gain()  const {return gain_;}
  float offset()const {return offset_;}
  //: operations
  bool solve();
  bool solve_with_constraints(double lambda);
  //:transform the test image by the solved gain and offset
  vil_image_view<float> mapped_test_image();
 protected:
  //: internal methods
  void compute_valid_pix();
  //: members
  unsigned ni_, nj_;
  vil_image_view<float> model_image_;
  vil_image_view<float> test_image_;
  float gain_;
  float offset_;
  vil_image_view<unsigned char> model_mask_;
  vil_image_view<unsigned char> test_mask_;
  bool t_mask_;
  bool m_mask_;
  unsigned n_valid_pix_;
};

#endif // brip_gain_offset_solver_h_
