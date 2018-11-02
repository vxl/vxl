// This is brl/bseg/sdet/sdet_nms.cxx
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include "sdet_nms.h"
//:
// \file

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_matrix.h>
#include <vnl/algo/vnl_svd.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vgl/vgl_homg_line_2d.h>
#include <vgl/algo/vgl_homg_operators_2d.h>

//---------------------------------------------------------------
// Constructors
//----------------------------------------------------------------

//: Constructor from a parameter block, gradient magnitudes given as an image and directions given as component image
sdet_nms::sdet_nms(const sdet_nms_params& nsp, const vil_image_view<double>& dir_x,
                   const vil_image_view<double>& dir_y, const vil_image_view<double>& grad_mag) :
  thresh_(nsp.thresh_),
  parabola_fit_type_(nsp.pfit_type_),
  margin_(nsp.margin_),
  rel_thresh_(nsp.rel_thresh_),
  use_adaptive_thresh_(nsp.use_adaptive_thresh_),
  dir_x_(dir_x),
  dir_y_(dir_y),
  grad_mag_(grad_mag),
  x_(grad_mag.nj(), grad_mag.ni(), 0.0),
  y_(grad_mag.nj(), grad_mag.ni(), 0.0),
  dir_(grad_mag.nj(), grad_mag.ni(), 0.0),
  mag_(grad_mag.nj(), grad_mag.ni(), 0.0),
  deriv_(grad_mag.nj(), grad_mag.ni(), 0.0)
{
}

//-------------------------------------------------------------------------

// backward compatible method
void sdet_nms::apply(bool collect_tokens,
                     std::vector<vgl_point_2d<double> >& loc,
                     std::vector<double>& orientation,
                     std::vector<double>& mag)
{
  std::vector<double> d2f;
  std::vector<vgl_point_2d<int> > pix_loc;

  //call the main method
  apply(collect_tokens, loc, orientation, mag, d2f, pix_loc);
}

void sdet_nms::apply(bool collect_tokens,
                     std::vector<vgl_point_2d<double> >& loc,
                     std::vector<double>& orientation,
                     std::vector<double>& mag,
                     std::vector<double>& d2f)
{
  std::vector<vgl_point_2d<int> > pix_loc;

  //call the main method
  apply(collect_tokens, loc, orientation, mag, d2f, pix_loc);
}

//: Apply the algorithm
void sdet_nms::apply(bool collect_tokens,
                     std::vector<vgl_point_2d<double> >& loc,
                     std::vector<double>& orientation,
                     std::vector<double>& mag,
                     std::vector<double>& d2f,
                     std::vector<vgl_point_2d<int> >& pix_loc)
{
  double f[3], s_list[3];

  // run non-maximum suppression at every point inside the margins
  for (unsigned x = margin_; x < grad_mag_.ni()-margin_; x++) {
    for (unsigned y = margin_; y < grad_mag_.nj()-margin_; y++)
    {
      if (grad_mag_(x,y) < thresh_) //threshold by gradient magnitude
        continue;

      double gx = dir_x_(x,y);
      double gy = dir_y_(x,y);
      vgl_vector_2d<double> direction(gx,gy);
      normalize(direction);

      //The gradient has to be non-degenerate
      if (std::abs(direction.x()) < 10e-6 && std::abs(direction.y()) < 10e-6)
        continue;

      //now compute the values orthogonal to the edge and fit a parabola
      int face_num = intersected_face_number(direction); assert(face_num != -1);
      double s = intersection_parameter(direction, face_num); assert(s != -1000);
      f_values(x, y, direction, s, face_num, f);
      s_list[0] = -s; s_list[1] = 0.0; s_list[2] = s;

      //Amir: removed this maximum check because, there can be a maxima between f- and f+ even when f<f- or f<f+

      ////Maximum check
      //if ( (f[1]-f[0])>1e-2 && (f[1]-f[2])>1e-2) //epsilon checks instead of absolute
      //{
        //fit a parabola to the data
        double max_val = f[1]; //default (should be updated by parabola fit)
        double grad_val = 0.0; //should be updated by parabola fit

        //compute location of extrema
        double s_star = (parabola_fit_type_ == sdet_nms_params::PFIT_3_POINTS) ?
                            subpixel_s(s_list, f, max_val, grad_val) : subpixel_s(x, y, direction, max_val);

        if (std::fabs(s_star)< 0.7)
        {
          //record this edgel
          x_(y,x) = x + s_star * direction.x();
          y_(y,x) = y + s_star * direction.y();
          dir_(y,x) = std::atan2(direction.x(), -direction.y());
          mag_(y,x) = max_val; //the mag at the max of the parabola
          deriv_(y,x) = grad_val;
        }
      //}
    }
  }

  //post-process the NMS edgel map to reduce the occurrence of duplicate edgels
  for (unsigned x = margin_; x < grad_mag_.ni()-margin_; x++) {
    for (unsigned y = margin_; y < grad_mag_.nj()-margin_; y++)
    {
      if (mag_(y,x)==0.0)
        continue;

      //use the orientation of the edgel to determine the closest neighbors that could produce a duplicate edgel

      //Hack: for now just look over all the 8-neighbors
      for (int ii=-1; ii<2; ii++) {
        for (int jj=-1; jj<2; jj++) {

          if (ii==0 && jj==0) continue;

          //if there is an edgel at this location, compute the distance to the current edgel
          if (mag_(y+jj,x+ii)>0.0) {
            double dx = x_(y+jj,x+ii) - x_(y,x);
            double dy = y_(y+jj,x+ii) - y_(y,x);

            if ((dx*dx+dy*dy)<0.1) { //closeness threshold, may be made into a parameter if anyone cares
              mag_(y,x)=0.0; //kill the current edgel
              continue;
            }
          }
        }
      }
    }
  }

  //if seeking tokens, form tokens from the remaining edgels
  if (collect_tokens) {
    for (unsigned x = margin_; x < grad_mag_.ni()-margin_; x++) {
      for (unsigned y = margin_; y < grad_mag_.nj()-margin_; y++)
      {
        if (mag_(y,x)==0.0)
          continue;

        loc.emplace_back(x_(y,x), y_(y,x));
        orientation.push_back(dir_(y,x));
        mag.push_back(mag_(y,x));
        d2f.push_back(deriv_(y,x));

        //also return the pixel location so that they can be used for tracing algorithms
        pix_loc.emplace_back(x, y);
      }
    }
  }
}

int sdet_nms::intersected_face_number(const vgl_vector_2d<double>& direction)
{
  if (direction.x() >= 0 && direction.y() >= 0)
  {
    if (direction.x() >= direction.y())
      return 1;
    else
      return 2;
  }
  else if (direction.x() < 0 && direction.y() >= 0)
  {
    if (std::abs(direction.x()) < direction.y())
      return 3;
    else
      return 4;
  }
  else if (direction.x() < 0 && direction.y() < 0)
  {
    if (std::abs(direction.x()) >= std::abs(direction.y()))
      return 5;
    else
      return 6;
  }
  else if (direction.x() >= 0 && direction.y() < 0)
  {
    if (direction.x() < std::abs(direction.y()))
      return 7;
    else
      return 8;
  }
  return -1;
}

double sdet_nms::intersection_parameter(const vgl_vector_2d<double>& direction, int face_num)
{
  if (face_num == 1 || face_num == 8)
    return 1.0/direction.x();
  else if (face_num == 2 || face_num == 3)
    return 1.0/direction.y();
  else if (face_num == 4 || face_num == 5)
    return -1.0/direction.x();
  else if (face_num == 6 || face_num == 7)
    return -1.0/direction.y();
  return -1000.0;
}

void sdet_nms::f_values(int x, int y, const vgl_vector_2d<double>& direction, double s, int face_num, double *f)
{
  int corners[4];
  get_relative_corner_coordinates(face_num, corners);

  vgl_vector_2d<double> intersection_point = s * direction;
  vgl_vector_2d<double> corner1(corners[0], corners[1]); //have to convert to double for subtraction
  vgl_vector_2d<double> corner2(corners[2], corners[3]); //have to convert to double for subtraction
  double distance1 = length(intersection_point - corner1);
  double distance2 = length(intersection_point - corner2);
  double value1 = grad_mag_(x+corners[0], y+corners[1]);
  double value2 = grad_mag_(x+corners[2], y+corners[3]);
  double f_plus = value1 * distance2 + value2 * distance1;

  intersection_point = -s * direction;
  corner1.set(-corners[0], -corners[1]); //have to convert to double for subtraction
  corner2.set(-corners[2], -corners[3]); //have to convert to double for subtraction
  distance1 = length(intersection_point - corner1);
  distance2 = length(intersection_point - corner2);
  value1 = grad_mag_(x-corners[0], y-corners[1]);
  value2 = grad_mag_(x-corners[2], y-corners[3]);
  double f_minus = value1 * distance2 + value2 * distance1;

  f[0] = f_minus;
  f[1] = grad_mag_(x,y);
  f[2] = f_plus;
}

void sdet_nms::get_relative_corner_coordinates(int face_num, int *corners)
{
  switch (face_num)
  {
   case 1:
     corners[0] = 1;
     corners[1] = 0;
     corners[2] = 1;
     corners[3] = 1;
     break;
   case 2:
     corners[0] = 1;
     corners[1] = 1;
     corners[2] = 0;
     corners[3] = 1;
     break;
   case 3:
     corners[0] = 0;
     corners[1] = 1;
     corners[2] = -1;
     corners[3] = 1;
    break;
   case 4:
     corners[0] = -1;
     corners[1] = 1;
     corners[2] = -1;
     corners[3] = 0;
    break;
   case 5:
     corners[0] = -1;
     corners[1] = 0;
     corners[2] = -1;
     corners[3] = -1;
    break;
   case 6:
     corners[0] = -1;
     corners[1] = -1;
     corners[2] = 0;
     corners[3] = -1;
    break;
   case 7:
     corners[0] = 0;
     corners[1] = -1;
     corners[2] = 1;
     corners[3] = -1;
    break;
   case 8:
     corners[0] = 1;
     corners[1] = -1;
     corners[2] = 1;
     corners[3] = 0;
    break;
   default:
     corners[0] = 0;
     corners[1] = 0;
     corners[2] = 0;
     corners[3] = 0;
  }
}

double sdet_nms::subpixel_s(const double *s, const double *f, double &max_f, double &max_d)
{
  //double A = f[2] / ((s[2]-s[0])*(s[2]-s[1]));
  //double B = f[1] / ((s[1]-s[0])*(s[1]-s[2]));
  //double C = f[0] / ((s[0]-s[1])*(s[0]-s[2]));
  //double s_star = ((A+B)*s[0] + (A+C)*s[1] + (B+C)*s[2]) / (2*(A+B+C));


  //new version: assumes s[1]=0 and s[2]=-s[0]
  double A = -(f[1] - (f[0]+f[2])/2.0)/(s[2]*s[2]);
  double B = -(f[0]-f[2])/(2*s[2]);
  double C = f[1];

  double s_star = -B/(2*A);

  max_f = A*s_star*s_star + B*s_star + C;

  //second derivative
  double d2f = 2*A;
  max_d = d2f;

  if (A<0) { //make sure this is a maximum
    if (use_adaptive_thresh_) {
#if 0 // no longer used...
      //derivatives at f+ and f-
      double d2fp = 2*A*s[2] + B;
      double d2fm = 2*A*s[0] + B;
      if (std::fabs(d2fp)>rel_thresh_ || std::fabs(d2fm)>rel_thresh_)
#endif // 0
      if (d2f<-rel_thresh_)//d2f is always negative at a maxima
        return s_star;
      else
        return 5.0; //not reliable
    }
    else
      return s_star;
  }
  else
    return 5.0; //not a maximum

//From gevd NMS
#if 0
  // Fit a parabola through 3 points with strict local max/min.
  // Return the offset location, and value of the maximum/minimum.
  float
  gevd_float_operators::InterpolateParabola(float y_1, float y_0, float y_2,
                                            float&y)
  {
    float diff1 = y_2 - y_1;      // first derivative
    float diff2 = 2 * y_0 - y_1 - y_2; // second derivative
    y = y_0 + diff1 * diff1 / (8 * diff2);        // interpolate y as max/min
    return diff1 / (2 * diff2);   // interpolate x offset
}
#endif // 0
}

double sdet_nms::subpixel_s(int x, int y, const vgl_vector_2d<double>& direction, double &max_f)
{
  double d;
  double s;
  double f;
  vgl_homg_point_2d<double> p1(0.0, 0.0);
  vgl_homg_point_2d<double> p2(direction.x(), direction.y());
  vgl_homg_line_2d<double> line1(p1,p2);
  //construct the matrices
  vnl_matrix<double> A(9, 3);
  vnl_matrix<double> B(9, 1);
  vnl_matrix<double> P(3, 1);
  int index = 0;
  for (int j = -1; j <= 1; j++)
  {
    for (int i = -1; i <= 1; i++)
    {
      find_distance_s_and_f_for_point(i, j, line1, d, s, direction);
      f = grad_mag_(x+i,y+j);
      A(index, 0) = std::pow(s,2.0);
      A(index, 1) = s;
      A(index, 2) = 1.0;
      B(index, 0) = f;
      index++;
    }
  }
//  vnl_matrix<double> A_trans = A.transpose();
//  vnl_matrix<double> temp = vnl_matrix_inverse<double> (A_trans*A);
//  vnl_matrix<double> temp2 = temp * A_trans;
//  P = temp2 * B;
  vnl_svd<double> svd(A);
  P = svd.solve(B);
  double s_star = -P(1,0)/(2*P(0,0));


  max_f = P(0,0)*s_star*s_star + P(1,0)*s_star + P(2,0);

  if (P(0,0)<0)
    return s_star;
  else
    return 5.0; //not a maxima
}

void sdet_nms::find_distance_s_and_f_for_point(int x, int y, vgl_homg_line_2d<double> line,
                                               double &d, double &s, const vgl_vector_2d<double>& direction)
{
  vgl_homg_point_2d<double> point(x,y);
  vgl_homg_line_2d<double> perp_line = vgl_homg_operators_2d<double>::perp_line_through_point(line, point);
  vgl_homg_point_2d<double> intersection_point_homg = vgl_homg_operators_2d<double>::intersection(line, perp_line);
  vgl_point_2d<double> intersection_point(intersection_point_homg);
  vgl_vector_2d<double> d_helper(x-intersection_point.x(), y-intersection_point.y());
  d = length(d_helper);
  s = intersection_point.x() / direction.x();
}

//----------------------------------------------------------
//: Clear internal storage
//
void sdet_nms::clear()
{
  mag_.clear();
}
