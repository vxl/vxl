//-*- c++ -*-------------------------------------------------------------------
#ifndef osl_edge_detector_h
#define osl_edge_detector_h
//
// .NAME osl_edge_detector - Charlie's topological edge detector:
// .INCLUDE osl/osl_edge_detector.h
// .FILE osl_edge_detector.cxx
//
// .SECTION Description
// A class for performing topologically-accurate edgel detection.  NB: this
// implementation is (unintentionally) stochastic, so you don't necessarily
// get the same results from the same image.
//
// The basic implementatation is based on that described in Canny's thesis
// in that we compute the norm of the gradient, but then we use
// non-maximal supression dynamically to set the edge strength
// thresholds. Non-maximal supression is actually done using a
// varient of Tsai-Fu thinning, and not as described in Canny's
// thesis.
//
// Full documentation is given in INRIA technical report 2444, 1994.
//
// .SECTION Author:
//             Charlie Rothwell - 5/10/94
//             INRIA, Sophia Antipolis
//
// .SECTION Modifications:
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
//                       just histogrammed the gradient magintude.
//          JLM June 1997 Found a bug in the above change which failed if
//                       _ghist is NULL.
//          JLM Dec 1997 Moved _sigma, _low, _gauss_tail and _verbose
//                       up to osl_edge_detector_params and added new syle
//                       constructors and execution
//-----------------------------------------------------------------------------

#include <osl/osl_canny_port.h>
#include <osl/osl_edge_detector_params.h>

struct osl_edge_detector : public osl_edge_detector_params
{
  osl_edge_detector(osl_edge_detector_params const &);

  ~osl_edge_detector();

  void detect_edges(vil_image const &image,
                    vcl_list<osl_edge*> *edges,
                    bool maintain_topology = true);

private:
  void Sub_pixel_interpolation();
  void Thicken_threshold(int,int);
  void Set_thresholds();
  void Fill_holes();
  void Thin_edges();
  void Follow_curves(vcl_list<osl_edge*>*);
  void Follow(int,int,vcl_list<int>*,vcl_list<int>*,vcl_list<float>*,int);

  void Find_junctions();
  void Find_junction_clusters();
  void Follow_junctions(int,int,vcl_list<int>*,vcl_list<int>*);
  void Cluster_centre(vcl_list<int>&,vcl_list<int>&,int&,int&);

private:
  int _width;          // The smoothing kernel width
  int _k_size;         // The kernel is 2*_width+1s
  float *_kernel;      // 1-Dimensional convolution kernel of size k_size

  int _xstart,_ystart; // The origin of the buffer in the image
  int _xsize,_ysize;   // The width of the image buffer

  float **_dx;         // Derivatives in x, and sub-pixel x coordinates
  float **_dy;         // Derivatives in y, and sub-pixel y coordinates
  float **_grad;       // Gradient image, and various other storage images
  float **_smooth;     // Smoothed image.

  // Quite a few of the following could be done using hash tables
  float **_thin;       // Gradient image after thinning
  float **_theta;      // Orientation image
  float **_thresh;     // Image of the different thresholds used (replacing low)

  int **_dist;         // Distance transform image
  int **_junction;     // Image true only at junctions ends, and relevant lists
  int **_jx,**_jy;     // Images of (x,y) coordinates of nearest cluster centre
  vcl_list<int> *_xjunc,*_yjunc;
  vcl_list<osl_Vertex*> *_vlist;   // The junction cluster centres

  float _jval;        // A dummy junction intensity step value
  int _chain_no;      // A dummy variable used in following

  int _vertidcount;   // A counter used for setting vertex identifiers

// Added a histogram of edge gradient magnitudes - JLM May 1995
  bool _gradient_histogram; // Do we need to compute one?
  float _max_gradient; // Added May 1997 - JLM
  int _histogram_resolution; // The number of buckets in the histogram
  //Histogram* _ghist;
};

#endif // osl_edge_detector_h
