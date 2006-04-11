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
#include <vcl_vector.h>
#include <vil/vil_image_resource.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <vsol/vsol_line_2d_sptr.h>
#include <sdet/sdet_nonmax_suppression_params.h>
#include <vgl/vgl_vector_2d.h>
#include <vil/vil_image_view.h>
#include <vbl/vbl_array_2d.h>

class sdet_nonmax_suppression : public sdet_nonmax_suppression_params
{
public:
  //Constructor from a parameter block, and gradients along x and y directions
  sdet_nonmax_suppression(sdet_nonmax_suppression_params& nsp, 
                          vbl_array_2d<double> &grad_x, 
                          vbl_array_2d<double> &grad_y);
  //Constructor from a parameter block, gradient magnitudes and the search directions
  sdet_nonmax_suppression(sdet_nonmax_suppression_params& nsp, 
                          vbl_array_2d<double> &grad_mag, 
                          vbl_array_2d<vgl_vector_2d <double> > &directions);
  //Destructor
  ~sdet_nonmax_suppression();
  //Accessors
  vcl_vector<vsol_point_2d_sptr>& get_points(){return points_;}
  vcl_vector<vsol_line_2d_sptr>& get_lines(){return lines_;}
  vcl_vector<vgl_vector_2d<double> >& get_directions() {return directions_;}
  //Utility Methods
  void apply();
  void clear();

protected:
  //members
  bool points_valid_;      //process state flag
  vcl_vector<vsol_point_2d_sptr> points_; //output, resulting edge points
  vcl_vector<vsol_line_2d_sptr> lines_; //output, lines along the edges
  vcl_vector<vgl_vector_2d<double> > directions_; //output, direction along which non-maximal suppression was done
  vbl_array_2d<double> grad_x_; //Gradient in x-direction
  vbl_array_2d<double> grad_y_; //Gradient in y-direction
  vbl_array_2d<double> grad_mag_;   //Gradient magnitude
  int width_, height_; // Width and height of the vbl_array_2d
  double max_grad_mag_; //maximum gradient magnitude value
  //functions
  int intersected_face_number(double gx, double gy);
  double intersection_parameter(double gx, double gy, int face_num);
  vcl_vector<double> f_values(int x, int y, double gx, double gy, double s, int face_num);
  vcl_vector< vgl_vector_2d<int> > get_relative_corner_coordinates(int face_num);
  double subpixel_s(vcl_vector<double> s, vcl_vector<double> f);
};

#endif // sdet_nonmax_suppression_h_
