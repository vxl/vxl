#ifndef _vsl_canny_rothwell_h
#define _vsl_canny_rothwell_h

// .SECTION Description
//
// A class for performing traditional Canny edge detection. The basic
// implementatation is as close to that described in Canny's thesis
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
// this yeilds a much better understanding of the image topology.
//
// Note that nothing special has been done around the border of the image;
// we have simply ignored a border of size _width all the way round. Perhaps
// this should be changed to provide consistency with the rest of TargetJr.
//
// .SECTION Author
//       Charlie Rothwell - 25/1/92
//       GE Corporate Research and Development
//
// .SECTION Modifications
//       Samer Abdallah - 5/10/95
//       Robotics Research Group, Oxford University
//         Most members and methods placed in new base class vsl_canny_rothwellBase.
//         Fixed bugs in Final_hysteresis() and Jump_single_pixels().

#include <vsl/vsl_canny_base.h>
class vsl_canny_rothwell_params;

class vsl_canny_rothwell : public vsl_canny_base {
public:
  vsl_canny_rothwell(vsl_canny_rothwell_params const &);
  ~vsl_canny_rothwell();

  void detect_edges(vil_image const &image, vcl_list<vsl_Edge*>*, bool adaptive = false);

protected:
  void Non_maximal_supression();
  void Initial_hysteresis();
  void Final_hysteresis(vcl_list<vsl_Edge*>*);
  void Jump_gap(int,int,int,int,int*,int*);
  void Thin_edges();
  void Jump_single_breaks();
  void Adaptive_Canny(vil_image const &);
  void Compute_adaptive_images(int,int,int,float**,float**,float**);
  void Subtract_thick(int,int,int,float**);
  void Best_eight_way(int,int,float**,int*,int*);
  void Find_dangling_ends();
  int Dangling_end(int,int);
  void Find_junctions();
  void Find_junction_clusters();

  int **_dangling;     // Binary image true only at dangling ends, and relevant lists
  vcl_list<int> *_xdang,*_ydang;
  float _range;       // The maximal region of effect of the smallest smoothing kernel

  // Parameters for the adaptive smoothing
  float _old_sigma;   // Smoothing sigma
  int _old_width;     // The smoothing kernel width
  int _old_k_size;    // The kernel is 2*_width+1s

  float _dummy;       // A dummy intensity step value
};

#endif
