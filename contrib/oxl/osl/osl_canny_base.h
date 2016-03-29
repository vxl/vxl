//-*- c++ -*-------------------------------------------------------------------
#ifndef osl_canny_base_h_
#define osl_canny_base_h_
//
// .NAME osl_canny_base - a base class for edge detectors
// .LIBRARY osl
// .HEADER vxl package
// .INCLUDE osl/osl_canny_base.h
// .FILE osl_canny_base.cxx
//
// .SECTION Description
//
// Definition of a base class for doing Canny edge detection.
// Note that nothing special has been done around the border of the image;
// we have simply ignored a border of size width_ all the way round. Perhaps
// this should be changed to provide consistency with the rest of TargetJr.
//
// .SECTION Author
//       Samer Abdallah - 5/10/95
//       Robotics Research Group, Oxford University
//         osl_canny_base is built from Canny  which was originally developed by
//         Charlie Rothwell - 25/1/92
//         GE Corporate Research and Development
// .SECTION Modifications
//       Samer Abdallah (samer@robots.ox.ac.uk) - 18/05/96
//        Cleared some compiler warnings
//
//-----------------------------------------------------------------------------

#include <osl/osl_canny_port.h>

template <class T>
T     **osl_canny_base_make_raw_image(int, int, T *dummy);
template <class T>
void    osl_canny_base_fill_raw_image(T **, int, int, T value);
template <class T>
void    osl_canny_base_free_raw_image(T **);
template <class S, class T>
void    osl_canny_base_copy_raw_image(S const * const *src, T * const *dst, int, int);

class osl_canny_base {
public:
  osl_canny_base(float sigma, float low, float high, bool verbose = true);
  ~osl_canny_base();

protected:

  static void Initial_follow(float * const *thin, int xsize, int ysize, float low,
                             int x,int y,
                             std::list<int> *xc,std::list<int> *yc,
                             std::list<float> *grad);
  void Final_follow(int,int,std::list<int>*,std::list<int>*,std::list<float>*,int);
  static void Follow_junctions(int * const *junction, int x, int y, std::list<int> *xc, std::list<int> *yc);
  static void Cluster_centre_of_gravity(int * const *jx, int * const *jy,
                                        std::list<int> &xc, std::list<int> &yc,
                                        int &x0,int &y0);
  static int Junction_neighbour(int const * const *junction, int x, int y);


  unsigned int xstart_,ystart_; // The origin of the buffer in the image
  unsigned int xsize_,ysize_;   // The width of the image buffer

  float **smooth_;     // Smoothed intensity image
  float **dx_;         // Derivatives in x, and sub-pixel x coordinates
  float **dy_;         // Derivatives in y, and sub-pixel y coordinates
  float **grad_;       // Gradient image

  float **thick_;      // Gradient image after NMS
  float **thin_;       // Gradient image after NMS and thinning
  float **theta_;      // Orientation image


  int **junction_;     // Binary image true only at junctions ends, and relevant lists
  int **jx_,**jy_;     // Images of (x,y) coordinates of nearest cluster centre
  std::list<int> *xjunc_;
  std::list<int> *yjunc_;
  std::list<osl_Vertex*> *vlist_;   // The junction cluster centres

  float gauss_tail_;  // The value of the kernel at its tail
  float sigma_;       // Smoothing sigma
  int width_;         // The smoothing kernel width - can change
  int w0_;            // Same as above, but does not change
  int k_size_;        // The kernel is 2*width_+1s
  float *kernel_;     // 1-Dimensional convolution kernel of size k_size
  float low_;         // Low threshold for hysteresis
  float high_;        // High threshold for hysteresis

  float jval_;        // A dummy junction intensity step value
  int chain_no_;      // A dummy variable used in following
  bool verbose;
};

#endif // osl_canny_base_h_
