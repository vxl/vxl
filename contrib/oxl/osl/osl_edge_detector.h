//-*- c++ -*-------------------------------------------------------------------
#ifndef osl_edge_detector_h
#define osl_edge_detector_h
//:
// \file
// \brief Charlie's topological edge detector
//
// A class for performing topologically-accurate edgel detection.  NB: this
// implementation is (unintentionally) stochastic, so you don't necessarily
// get the same results from the same image.
//
// The basic implementation is based on that described in Canny's thesis
// in that we compute the norm of the gradient, but then we use
// non-maximal suppression dynamically to set the edge strength
// thresholds. Non-maximal suppression is actually done using a
// variant of Tsai-Fu thinning, and not as described in Canny's
// thesis.
//
// Full documentation is given in INRIA technical report 2444, 1994.
//
// \author Charlie Rothwell - 5/10/94
//         INRIA, Sophia Antipolis
//
// \verbatim
//  Modifications:
//          CAR March 1995: improved memory management so that computation
//             time is reduced. Re-use of certain large arrays. Tried to
//             re-write Set_thresholds() to use Delauney triangulation rather
//             than Chamfer filtering. This didn't work as computation time
//             became far too high.
//
//          JLM May 1995: Added a mask in Compute_gradient to support
//             edge detection within a polygonal region. Should add a similar
//             test in smoothing and derivative iterations for maximum
//             efficiency, but it isn't clear that there would be a big overall
//             gain, given the computation of IsMasked(x,y).
//
//          JLM May 1995: Added a histogram of edgel strengths as an
//             additional output to support edgel change detection.  The
//             histogram is constructed in Set_thresholds
//
//          JLM May 1995 Added a new Do_edge_detector signature
//             to output an edgel group which bundles edgel chains and
//             gradient statistics together.
//
//          JLM May 1997 Modified the formation of the gradient histogram
//                       computation so that values are accumulated only
//                       along edgel chains.  New accumulation is in
//                       Follow_curves(edges). The old code is still in place
//                       but commented out. (Look for "May")  The old code
//                       just histogrammed the gradient magnitude.
//          JLM June 1997 Found a bug in the above change which failed if
//                       ghist_ is NULL.
//          JLM Dec 1997 Moved sigma_, low_, gauss_tail_ and verbose_
//                       up to osl_edge_detector_params and added new syle
//                       constructors and execution
// \endverbatim
//-----------------------------------------------------------------------------

#include <osl/osl_canny_port.h>
#include <osl/osl_edge_detector_params.h>
#include <vil1/vil1_image.h>

struct osl_edge_detector : public osl_edge_detector_params
{
  osl_edge_detector(osl_edge_detector_params const &);

  ~osl_edge_detector();

  void detect_edges(vil1_image const &image,
                    std::list<osl_edge*> *edges,
                    bool maintain_topology = true);

 private:
  void Sub_pixel_interpolation();
  void Thicken_threshold(int,int);
  void Set_thresholds();
  void Fill_holes();
  void Thin_edges();
  void Follow_curves(std::list<osl_edge*>*);
  void Follow(int,int,std::list<int>*,std::list<int>*,std::list<float>*,int);

  void Find_junctions();
  void Find_junction_clusters();
  void Follow_junctions(int,int,std::list<int>*,std::list<int>*);
  void Cluster_centre(std::list<int>&,std::list<int>&,int&,int&);

 private:
  unsigned int width_;  // The smoothing kernel width
  unsigned int k_size_; // The kernel is 2*width_+1s
  float *kernel_;       // 1-Dimensional convolution kernel of size k_size

  unsigned int xstart_,ystart_; // The origin of the buffer in the image
  unsigned int xsize_,ysize_;   // The width of the image buffer

  float **dx_;         // Derivatives in x, and sub-pixel x coordinates
  float **dy_;         // Derivatives in y, and sub-pixel y coordinates
  float **grad_;       // Gradient image, and various other storage images
  float **smooth_;     // Smoothed image.

  // Quite a few of the following could be done using hash tables
  float **thin_;       // Gradient image after thinning
  float **theta_;      // Orientation image
  float **thresh_;     // Image of the different thresholds used (replacing low)

  int **dist_;         // Distance transform image
  int **junction_;     // Image true only at junctions ends, and relevant lists
  int **jx_,**jy_;     // Images of (x,y) coordinates of nearest cluster centre
  std::list<int> *xjunc_,*yjunc_;
  std::list<osl_Vertex*> *vlist_;   // The junction cluster centres

  float jval_;        // A dummy junction intensity step value
  int chain_no_;      // A dummy variable used in following

  int vertidcount_;   // A counter used for setting vertex identifiers

// Added a histogram of edge gradient magnitudes - JLM May 1995
  bool gradient_histogram_; // Do we need to compute one?
  float max_gradient_; // Added May 1997 - JLM
  int histogram_resolution_; // The number of buckets in the histogram
  //Histogram* ghist_;
};

#endif // osl_edge_detector_h
