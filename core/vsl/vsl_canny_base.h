//-*- c++ -*-------------------------------------------------------------------
#ifndef _CannyBase_h
#define _CannyBase_h
//
// .NAME CannyBase - a base class for edge detectors
// .LIBRARY Detection
// .HEADER Segmentation package
// .INCLUDE Detection/CannyBase.h
// .FILE CannyBase.h
// .FILE CannyBase.C
//
// .SECTION Description:
//
// Definition of a Base class for doing Canny edge detection. 
// Note that nothing special has been done around the border of the image;
// we have simply ignored a border of size _width all the way round. Perhaps
// this should be changed to provide consistency with the rest of TargetJr.
//
// .SECTION Author:
//       Samer Abdallah - 5/10/95
//       Robotics Research Group, Oxford University
//         CannyBase is built from Canny  which was originally developed by
//         Charlie Rothwell - 25/1/92
//         GE Corporate Research and Development
// .SECTION Modifications :
//       Samer Abdallah (samer@robots.ox.ac.uk) - 18/05/96
//        Cleared some compiler warnings
//
//-----------------------------------------------------------------------------

#include <vsl/vsl_canny_port.h>

template <class T> 
T     **vsl_canny_base_make_raw_image(int, int, T *dummy);
template <class T> 
void    vsl_canny_base_fill_raw_image(T **, int, int, T value);
template <class T> 
void    vsl_canny_base_free_raw_image(T **);
template <class S, class T> 
void    vsl_canny_base_copy_raw_image(S const * const *src, T * const *dst, int, int);
  
class vsl_canny_base {
public:
  vsl_canny_base(float sigma, float low, float high, bool verbose = true);
  ~vsl_canny_base();

protected:

  static void Initial_follow(float * const *thin, int xsize, int ysize, float low,
			     int x,int y, 
			     vcl_list<int> *xc,vcl_list<int> *yc,
			     vcl_list<float> *grad);
  void Final_follow(int,int,vcl_list<int>*,vcl_list<int>*,vcl_list<float>*,int);
  static void Follow_junctions(int * const *junction, int x, int y, vcl_list<int> *xc, vcl_list<int> *yc);
  static void Cluster_centre_of_gravity(int * const *jx, int * const *jy, 
					vcl_list<int> &xc, vcl_list<int> &yc,
					int &x0,int &y0);
  static int Junction_neighbour(int const * const *junction, int x, int y);


  int _xstart,_ystart; // The origin of the buffer in the image
  int _xsize,_ysize;   // The width of the image buffer

  float **_smooth;     // Smoothed intensity image
  float **_dx;         // Derivatives in x, and sub-pixel x coordinates
  float **_dy;         // Derivatives in y, and sub-pixel y coordinates
  float **_grad;       // Gradient image

  float **_thick;      // Gradient image after NMS
  float **_thin;       // Gradient image after NMS and thinning
  float **_theta;      // Orientation image


  int **_junction;     // Binary image true only at junctions ends, and relevant lists
  int **_jx,**_jy;     // Images of (x,y) coordinates of nearest cluster centre
  vcl_list<int> *_xjunc;
  vcl_list<int> *_yjunc;
  vcl_list<vsl_Vertex*> *_vlist;   // The junction cluster centres

  float _gauss_tail;  // The value of the kernel at its tail
  float _sigma;       // Smoothing sigma
  int _width;         // The smoothing kernel width - can change
  int _w0;            // Same as above, but does not change
  int _k_size;        // The kernel is 2*_width+1s
  float *_kernel;     // 1-Dimensional convolution kernel of size k_size
  float _low;         // Low threshold for hysteresis
  float _high;        // High threshold for hysteresis

  float _jval;        // A dummy junction intensity step value
  int _chain_no;      // A dummy variable used in following
  bool verbose;
};

#endif
