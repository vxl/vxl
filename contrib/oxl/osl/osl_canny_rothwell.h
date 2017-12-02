#ifndef osl_canny_rothwell_h
#define osl_canny_rothwell_h
//:
// \file
//
// A class for performing traditional Canny edge detection. The basic
// implementation is as close to that described in Canny's thesis
// as possible, though sub-pixel interpolation of the final edge
// output has been included, as has edge thinning. Note that there are
// probably differences between this implementation and the documentation
// (that is his Master's thesis).
//
// The improved implementation includes the patching of single pixel
// gaps in the Edgel chains, as well as improved search for edgels near
// to edgelchain terminations (dangling ends). At these places the scale
// of the smoothing convolution kernel is reduced dynamically to remove
// interference from corners (if they exist) near to the ends. Overall,
// this yields a much better understanding of the image topology.
//
// Note that nothing special has been done around the border of the image;
// we have simply ignored a border of size width_ all the way round. Perhaps
// this should be changed to provide consistency with the rest of TargetJr.
//
// \author
//       Charlie Rothwell - 25/1/92
//       GE Corporate Research and Development
//
// \verbatim
//  Modifications
//   Samer Abdallah - 5/10/95
//   Robotics Research Group, Oxford University
//    Most members and methods placed in new base class osl_canny_rothwellBase.
//    Fixed bugs in Final_hysteresis() and Jump_single_pixels().
// \endverbatim

#include <osl/osl_canny_base.h>
#include <vil1/vil1_image.h>

class osl_canny_rothwell_params;

class osl_canny_rothwell : public osl_canny_base
{
 public:
  osl_canny_rothwell(osl_canny_rothwell_params const &);
  ~osl_canny_rothwell();

  void detect_edges(vil1_image const &image, std::list<osl_edge*>*, bool adaptive = false);

 protected:
  void Non_maximal_suppression();
  void Initial_hysteresis();
  void Final_hysteresis(std::list<osl_edge*>*);
  void Jump_gap(int,int,int,int,int*,int*);
  void Thin_edges();
  void Jump_single_breaks();
  void Adaptive_Canny(vil1_image const &);
  void Compute_adaptive_images(int,int,int,float**,float**,float**);
  void Subtract_thick(int,int,int,float**);
  void Best_eight_way(int,int,float**,int*,int*);
  void Find_dangling_ends();
  int Dangling_end(int,int);
  void Find_junctions();
  void Find_junction_clusters();

  int **dangling_;     // Binary image true only at dangling ends, and relevant lists
  std::list<int> *xdang_,*ydang_;
  float range_;       // The maximal region of effect of the smallest smoothing kernel

  // Parameters for the adaptive smoothing
  float old_sigma_;   // Smoothing sigma
  int old_width_;     // The smoothing kernel width
  int old_k_size_;    // The kernel is 2*width_+1s

  float dummy_;       // A dummy intensity step value
};

#endif // osl_canny_rothwell_h
