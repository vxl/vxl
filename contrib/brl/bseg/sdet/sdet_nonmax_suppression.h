// This is brl/bseg/sdet/sdet_nonmax_suppression.h
#ifndef sdet_nonmax_suppression_h_
#define sdet_nonmax_suppression_h_
//---------------------------------------------------------------------
//:
// \file
// \brief a processor for non-maximal suppression
// For every pixel in an image, a parabola fit is applied along the
// gradient direction, and the maximum point on this parabola is used
// to get the sub-pixel location of the edge. The pixel point must qualify
// for being a maximum, i.e. it should have higher values than the
// interpolated sub-pixels along the positive and negative gradient
// direction.
//
// Below drawing shows the face numbers at a pixel and
// is for author's own reference.
//
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
//
// \author
//  H. Can Aras - February 22, 2006
//
// \verbatim
//  Modifications
//  2006-04-08 H. Can Aras
//             changed input style through the constructors, added another output variable
// \endverbatim
//
//-------------------------------------------------------------------------
#include <iostream>
#include <vector>
#include <vcl_compiler.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <vsol/vsol_line_2d_sptr.h>
#include <sdet/sdet_nonmax_suppression_params.h>
#include <vgl/vgl_vector_2d.h>
#include <vil/vil_image_view.h>
#include <vbl/vbl_array_2d.h>
#include <vgl/vgl_homg_line_2d.h>

class sdet_nonmax_suppression : public sdet_nonmax_suppression_params
{
 public:
  //: Constructor from a parameter block, and gradients along x and y directions given as arrays
  sdet_nonmax_suppression(sdet_nonmax_suppression_params& nsp,
                          vbl_array_2d<double> &grad_x,
                          vbl_array_2d<double> &grad_y);
  //: Constructor from a parameter block, gradient magnitudes given as an array and directions given as component arrays
  sdet_nonmax_suppression(sdet_nonmax_suppression_params& nsp,
                          vbl_array_2d<double> &dir_x,
                          vbl_array_2d<double> &dir_y,
                          vbl_array_2d<double> &grad_mag);
  //: Constructor from a parameter block, gradient magnitudes given as an array and the search directions
  sdet_nonmax_suppression(sdet_nonmax_suppression_params& nsp,
                          vbl_array_2d<double> &grad_mag,
                          vbl_array_2d<vgl_vector_2d <double> > &directions);
  //: Constructor from a parameter block, and gradients along x and y directions given as images
  sdet_nonmax_suppression(sdet_nonmax_suppression_params& nsp,
                          vil_image_view<double> &grad_x,
                          vil_image_view<double> &grad_y);
  //: Constructor from a parameter block, gradient magnitudes given as an image and directions given as component image
  sdet_nonmax_suppression(sdet_nonmax_suppression_params& nsp,
                          vil_image_view<double> &dir_x,
                          vil_image_view<double> &dir_y,
                          vil_image_view<double> &grad_mag);
  //: Constructor from a parameter block, gradient magnitudes given as an image and the search directions
  sdet_nonmax_suppression(sdet_nonmax_suppression_params& nsp,
                          vil_image_view<double> &grad_mag,
                          vbl_array_2d<vgl_vector_2d <double> > &directions);
  //: Destructor
  ~sdet_nonmax_suppression();
  //Accessors
  std::vector<vsol_point_2d_sptr>& get_points(){return points_;}
  std::vector<vsol_line_2d_sptr>& get_lines(){return lines_;}
  std::vector<vgl_vector_2d<double> >& get_directions() {return directions_;}
  //Utility Methods
  void apply();
  void clear();

 protected:
  //members
  bool points_valid_;      //process state flag
  std::vector<vsol_point_2d_sptr> points_; //output, resulting edge points
  std::vector<vsol_line_2d_sptr> lines_; //output, lines along the edges
  std::vector<vgl_vector_2d<double> > directions_; //output, direction along which non-maximal suppression was done
  vbl_array_2d<double> grad_x_; //Gradient in x-direction
  vbl_array_2d<double> grad_y_; //Gradient in y-direction
  vbl_array_2d<double> grad_mag_;   //Gradient magnitude
  int width_, height_; // Width and height of the vbl_array_2d
  double max_grad_mag_; //maximum gradient magnitude value
  int parabola_fit_type_; //flag for parabola fit method
  //functions
  int intersected_face_number(double gx, double gy);
  double intersection_parameter(double gx, double gy, int face_num);
  void f_values(int x, int y, double gx, double gy, double s, int face_num, double *f);
  // get the corners related to the given face
  void get_relative_corner_coordinates(int face_num, int *corners);
  // used for 3 points parabola fit
  double subpixel_s(double *s, double *f);
  // used for 9 points parabola fit
  double subpixel_s(int x, int y, vgl_vector_2d<double> direction);
  void find_distance_s_and_f_for_point(int x, int y, vgl_homg_line_2d<double> line,
                                       double &d, double &s, vgl_vector_2d<double> direction);
};

#endif // sdet_nonmax_suppression_h_
