#ifndef brip_para_cvrg_h
#define brip_para_cvrg_h
//-----------------------------------------------------------------------------
//:
// \file
// \author  Joe Mundy July 31, 1999
// \brief brip_para_cvrg - Detection of parallel image features
//
//    A detector for parallel lines. The algorithm uses a 4 direction set
//   of gradient filters (0, 45, 90, 135 degrees) to detect linear steps.
//   The steps are then projected onto an accumulation array oriented
//   perpendicular to the direction of the linear feature. The projection
//   is carried out in each of the four orientations at each pixel, using
//   the appropriate gradient image.
//
//    Parallel support is defined as the existence of two or more peaks in
//   the projection array. The peaks are found by carrying out non-maximal
//   suppression on the projection array. This parallel coverage is quantified
//   by computing the average peak height.  If there are not at least two
//   peaks, the "coverage" is defined as zero.
//
//    The direction with maximum coverage value is determined and that
//   value is inserted an output image called the cover_image.
//
//   The current algorithm has a performance of about 1.8*10^4 pixels/sec
//   The bulk of the time is taken in doing the projections.
//
// \verbatim
//  Modifications:
//   Ported to vxl July 01, 2004
//   Converted to vil October 3, 2009
// \endverbatim
//-----------------------------------------------------------------------------

#include <iostream>
#include <vector>
#include <brip/brip_para_cvrg_params.h>
#include <vil/vil_image_view.h>
#include <vil/vil_image_resource_sptr.h>
#include <vil/vil_rgb.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
class brip_para_cvrg : public brip_para_cvrg_params
{
  // PUBLIC INTERFACE----------------------------------------------------------

 public:

  // Constructors/Initializers/Destructors-------------------------------------
  brip_para_cvrg(float sigma = 1.0, float low = 6,
                 float gauss_tail = .05,
                 int proj_width = 7, int proj_height=1,
                 int sup_radius = 1,
                 bool verbose = true);

  brip_para_cvrg(brip_para_cvrg_params& pdp);
  ~brip_para_cvrg() override;
  void do_coverage(vil_image_resource_sptr const& image);

  // Data Access---------------------------------------------------------------
  vil_image_view<float> get_float_detection_image(const float max = 255);
  vil_image_view<unsigned char> get_detection_image();
  vil_image_view<unsigned char> get_dir_image();
  vil_image_view<vil_rgb<unsigned char> > get_combined_image();
  // Utility Methods
 private:
  void init(vil_image_resource_sptr const & image);
  void init_variables();
  void set_kernel();
  void smooth_image();
  void avg(int x, int y, vil_image_view<float> const& smooth,
           vil_image_view<float>& avg);
  void grad0(int x, int y, vil_image_view<float> const& smooth,
             vil_image_view<float>& grad0);
  void grad45(int x, int y, vil_image_view<float> const& smooth,
              vil_image_view<float>& grad45);
  void grad90(int x, int y, vil_image_view<float> const& smooth,
              vil_image_view<float>& grad90);
  void grad135(int x, int y, vil_image_view<float> const& smooth,
               vil_image_view<float>& grad135);
  void compute_gradients();
  float project(int x, int y, int dir, std::vector<float>& projection);
  void remove_flat_peaks(int n, std::vector<float>& array);
  void non_maximum_supress(std::vector<float> const& array, std::vector<float>& sup_array);
  float parallel_coverage(std::vector<float> const& sup_array);
  void compute_parallel_coverage();
  void compute_image(vil_image_view<float> const& data,
                     vil_image_view<unsigned char>& image);
  //
  // Members
  int proj_n_;         // Number of pixels in the projection array
  int width_;          // The smoothing kernel width
  int k_size_;         // The kernel is 2*_width+1
  std::vector<float> kernel_;      // 1-Dimensional convolution kernel of size k_size

  int xstart_, ystart_; // The origin of the buffer in the image
  int xsize_, ysize_;   // The width of the image buffer
  vil_image_view<float> smooth_;       // The smoothed image
  vil_image_view<float> avg_;       // average intensity
  vil_image_view<float> grad0_;       // Derivative images in 45 degree increments
  vil_image_view<float> grad45_;
  vil_image_view<float> grad90_;
  vil_image_view<float> grad135_;
  vil_image_view<float> det_;        //The resulting detector value
  vil_image_view<float> dir_;        //The direction of maximum detection amplitude
  //A memory image of the detected pattern
  vil_image_view<unsigned char> det_image_;
  //A memory image of the max pattern orientation
  vil_image_view<unsigned char> dir_image_;
  vil_image_view<float> image_; //original image converted to float
  std::vector<float> sup_proj_;     //A 1-d array for maximal suppression
  std::vector<float> proj_0_;       //1d arrays for projecting the gradient magnitude
  std::vector<float> proj_45_;
  std::vector<float> proj_90_;
  std::vector<float> proj_135_;
};

#endif
