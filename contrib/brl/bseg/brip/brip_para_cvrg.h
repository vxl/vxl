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
//   the approriate gradient image.
//    Parallel support is defined as the existence of two or more peaks in
//   the projection array. The peaks are found by carrying out non-maximal
//   suppression on the projection array. This parallel coverage is quantified
//   by computing the average peak height.  If there are not at least two
//   peaks, the "coverage" is defined as zero.
//    The direction with maximum coverage value is determined and that
//   value is inserted an output image called the cover_image.
//
//   The current algorithm has a performance of about 1.8*10^4 pixels/sec
//   The bulk of the time is taken in doing the projections.
//
//
// \verbatim
// Modifications:
//   Ported to vxl July 01, 2004
// \endverbatim
//-----------------------------------------------------------------------------

#include <brip/brip_para_cvrg_params.h>
#include <vil1/vil1_rgb.h>
#include <vil1/vil1_image.h>
#include <vil1/vil1_memory_image_of.h>
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
  ~brip_para_cvrg();
  void do_coverage(vil1_image const& image);

  // Data Access---------------------------------------------------------------
  vil1_memory_image_of<float> get_float_detection_image(const float max = 255);
  vil1_memory_image_of<unsigned char> get_detection_image();
  vil1_memory_image_of<unsigned char> get_dir_image();
  vil1_memory_image_of<vil1_rgb<unsigned char> > get_combined_image();
  // Utility Methods
 private:
  void init(vil1_image const& image);
  void init_variables();
  float **make_float_image(int m , int n); //A m x n array m = xsize;
  void free_float_image(float** image);
  float *make_float_vector(int size);
  void set_float_vector(float* vector, int size, float val);
  void set_float_image(float**,float);
  void copy_image(float**,float**);
  void set_kernel();
  void smooth_image();
  void avg(int x, int y, vil1_memory_image_of<float> const& smooth, float** avg);
  void grad0(int x, int y, vil1_memory_image_of<float> const& smooth, float** grad0);
  void grad45(int x, int y, vil1_memory_image_of<float> const& smooth, float** grad45);
  void grad90(int x, int y, vil1_memory_image_of<float> const& smooth, float** grad90);
  void grad135(int x, int y, vil1_memory_image_of<float> const& smooth, float** grad135);
  void compute_gradients();
  float project(int x, int y, int dir, float* projection);
  void remove_flat_peaks(int n, float* array);
  void non_maximum_supress(float* array, float* sup_array);
  float parallel_coverage(float* sup_array);
  void compute_parallel_coverage();
  void compute_image(float** data, vil1_memory_image_of<unsigned char>& image);
  //
  // Members
  int proj_n_;         // Number of pixels in the projection array
  int width_;          // The smoothing kernel width
  int k_size_;         // The kernel is 2*_width+1
  float *kernel_;      // 1-Dimensional convolution kernel of size k_size

  int xstart_, ystart_; // The origin of the buffer in the image
  int xsize_, ysize_;   // The width of the image buffer
  vil1_memory_image_of<float> smooth_;       // The smoothed image
  float **avg_;       // average intensity
  float **grad0_;       // Derivative images in 45 degree increments
  float **grad45_;
  float **grad90_;
  float **grad135_;
  float **det_;        //The resulting detector value
  float **dir_;        //The direction of maximum detection amplitude
  //A memory image of the detected pattern
  vil1_memory_image_of<unsigned char> det_image_;
  //A memory image of the max pattern orientation
  vil1_memory_image_of<unsigned char> dir_image_;
  vil1_memory_image_of<float> image_;
  float* sup_proj_;     //A 1-d array for maximal suppression
  float* proj_0_;       //1d arrays for projecting the gradient magnitude
  float* proj_45_;
  float* proj_90_;
  float* proj_135_;
};

#endif
