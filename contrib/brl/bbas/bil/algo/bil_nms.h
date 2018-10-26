// This is brl/bbas/bil/algo/bil_nms.h
#ifndef bil_nms_h
#define bil_nms_h
//:
//\file
//\brief A NMS class that can work with the dbdet_edge_map class.
//
// For every pixel in an image, a parabola fit is applied along the
// gradient direction, and the maximum point on this parabola is used
// to get the sub-pixel location of the edge. The pixel point must qualify
// for being a maximum, i.e. it should have higher values than the
// interpolated sub-pixels along the positive and negative gradient
// direction.
//
// Below drawing shows the face numbers at a pixel and
// is for author's own reference.
// \verbatim
//       .---->x
//       |
//       |
//       v y
//
//          6    7
//        -----------
//      5 |    |    | 8
//        |    |    |
//        -----------
//      4 |    |    | 1
//        |    |    |
//        -----------
//          3     2
// \endverbatim
//
// \author Amir Tamrakar
// \date November 7, 2009
//
// \verbatim
//  Modifications
//   Vishal Jain moved the code from lemsvxl to vxl
// \endverbatim
//
//-------------------------------------------------------------------------
#include <vbl/vbl_array_2d.h>
#include <vil/vil_image_view.h>
#include <vgl/vgl_vector_2d.h>
#include <vgl/vgl_homg_line_2d.h>

//: parameters for NMS
class bil_nms_params
{
 public:
  // enumeration for the parabola fit type
  enum PFIT_TYPE {PFIT_3_POINTS=0, PFIT_9_POINTS=1};

  double thresh_;
  PFIT_TYPE pfit_type_;
  unsigned margin_;
  double rel_thresh_;                       // = 1.3*noise_sigma./sigma^3;
  bool use_adaptive_thresh_;

  bil_nms_params(double thresh=1.0, PFIT_TYPE pfit_type=PFIT_3_POINTS, unsigned margin=1, double rel_thresh=2.5, bool adaptive_thresh=false):
    thresh_(thresh), pfit_type_(pfit_type), margin_(margin), rel_thresh_(rel_thresh), use_adaptive_thresh_(adaptive_thresh){}
  ~bil_nms_params()= default;
};

class bil_nms
{
 protected:
  double thresh_;                                  ///< threshold
  bil_nms_params::PFIT_TYPE parabola_fit_type_;  ///< flag for parabola fit method
  unsigned margin_;                                ///< margin size
  double rel_thresh_;                              ///< reliable threshold (depends on sensor noise and sigma)
  bool use_adaptive_thresh_;                       ///< use reliable threshold or not

  //references to the data passed to this algo
  const vil_image_view<double>& dir_x_;
  const vil_image_view<double>& dir_y_;
  const vil_image_view<float>& grad_mag_;

  // various 2d arrays holding the NMS pixel information to allow for post processing before
  // edgel tokens are returned

  vbl_array_2d<double> x_;   ///< to store the x coordinate of the subpixel token
  vbl_array_2d<double> y_;   ///< to store the y coordinate of the subpixel token
  vbl_array_2d<double> dir_; ///< to store the orientation of the subpixel token (this might be redundant)
  vil_image_view<float> mag_; ///< to store the magnitude of the maxima points (also doubles as a marker of edge pixels)
  vbl_array_2d<double> deriv_; ///< to store the second derivative of the maxima points

 protected:
  //: default constructor is not to be used
  bil_nms();

 public:
  //: Constructor from a parameter block, gradient magnitudes given as an image and gradients given as component images
  bil_nms(const bil_nms_params& nsp,
          const vil_image_view<double>& dir_x,
          const vil_image_view<double>& dir_y,
          const vil_image_view<float>& grad_mag);

  // Destructor
  ~bil_nms() = default;

  //Accessors
  unsigned width() const { return mag_.ni(); }
  unsigned height() const { return mag_.nj(); }

  //: return the array containing the suppressed non maxima (i.e., only maxima remain)
  vil_image_view<float> mag() { return mag_; }

  //: return the array containing the second deriv map
  vbl_array_2d<double>& deriv() { return deriv_; }

  void apply();

  void clear();

  //intermediate functions
  int intersected_face_number(const vgl_vector_2d<double>& direction);
  double intersection_parameter(const vgl_vector_2d<double>& direction, int face_num);
  void f_values(int x, int y, const vgl_vector_2d<double>& direction, double s, int face_num, double *f);
  // get the corners related to the given face
  void get_relative_corner_coordinates(int face_num, int *corners);
  // used for 3 points parabola fit
  double subpixel_s(const double *s, const double *f, double & max_f, double &max_d);
  // used for 9 points parabola fit
  double subpixel_s(int x, int y, const vgl_vector_2d<double>& direction, double &max_f);
  void find_distance_s_and_f_for_point(int x, int y, vgl_homg_line_2d<double> line,
                                       double &d, double &s, const vgl_vector_2d<double>& direction);
};


#endif // bil_nms_h
