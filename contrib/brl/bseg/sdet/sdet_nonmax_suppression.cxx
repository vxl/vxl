// This is brl/bseg/sdet/sdet_nonmax_suppression.cxx
#include "sdet_nonmax_suppression.h"
//:
// \file
#include <vcl_cstdlib.h>   // for vcl_abs(int) and vcl_sqrt()
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_line_2d.h>
#include <vnl/vnl_matrix.h>
#include <vnl/algo/vnl_matrix_inverse.h>
#include <vnl/algo/vnl_svd.h>
#include <vil/algo/vil_convolve_2d.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vgl/algo/vgl_homg_operators_2d.h>
#include <vcl_cstdio.h>
#include <vcl_cassert.h>

//---------------------------------------------------------------
// Constructors
//
//----------------------------------------------------------------

//: Constructor from a parameter block, and gradients along x and y directions given as arrays
//
sdet_nonmax_suppression::sdet_nonmax_suppression(sdet_nonmax_suppression_params& nsp,
                                                 vbl_array_2d<double> &grad_x,
                                                 vbl_array_2d<double> &grad_y)
  : sdet_nonmax_suppression_params(nsp)
{
  width_ = grad_x.rows();
  height_ = grad_x.cols();
  grad_x_.resize(width_, height_);
  grad_y_.resize(width_, height_);
  grad_mag_.resize(width_, height_);
  grad_x_ = grad_x;
  grad_y_ = grad_y;

  for (int j = 0; j < height_; j++)
  {
    for (int i = 0; i < width_; i++)
    {
      double val = vcl_sqrt(vcl_pow(grad_x_(i,j),2.0) + vcl_pow(grad_y_(i,j),2.0));
      grad_mag_(i,j) = val;
      if (val > max_grad_mag_)
        max_grad_mag_ = val;
    }
  }
  points_valid_ = false;
  parabola_fit_type_ = nsp.pfit_type_;
}

//: Constructor from a parameter block, gradient magnitudes given as an array and directions given as component arrays
//
sdet_nonmax_suppression::sdet_nonmax_suppression(sdet_nonmax_suppression_params& nsp, 
                                                 vbl_array_2d<double> &dir_x, 
                                                 vbl_array_2d<double> &dir_y,
                                                 vbl_array_2d<double> &grad_mag)
  : sdet_nonmax_suppression_params(nsp)
{
  width_ = grad_mag.rows();
  height_ = grad_mag.cols();
  grad_x_.resize(width_, height_);
  grad_y_.resize(width_, height_);
  grad_mag_.resize(width_, height_);
  grad_x_ = dir_x;
  grad_y_ = dir_y;
  grad_mag_ = grad_mag;

  for (int j = 0; j < height_; j++)
  {
    for (int i = 0; i < width_; i++)
    {
      double val = grad_mag_(i,j);
      if (val > max_grad_mag_)
        max_grad_mag_ = val;
    }
  }
  points_valid_ = false;
  parabola_fit_type_ = nsp.pfit_type_;
}

//: Constructor from a parameter block, gradient magnitudes given as an array and the search directions
//
sdet_nonmax_suppression::sdet_nonmax_suppression(sdet_nonmax_suppression_params& nsp,
                                                 vbl_array_2d<double> &grad_mag,
                                                 vbl_array_2d<vgl_vector_2d <double> > &directions)
  : sdet_nonmax_suppression_params(nsp)
{
  width_ = grad_mag.rows();
  height_ = grad_mag.cols();
  grad_x_.resize(width_, height_);
  grad_y_.resize(width_, height_);
  grad_mag_.resize(width_, height_);
  grad_mag_ = grad_mag;
  for (int j = 0; j < height_; j++)
  {
    for (int i = 0; i < width_; i++)
    {
      vgl_vector_2d<double> direction = directions(i,j);
      normalize(direction);
      grad_x_(i,j) = grad_mag_(i,j) * direction.x();
      grad_y_(i,j) = grad_mag_(i,j) * direction.y();
      double val = grad_mag_(i,j);
      if (val > max_grad_mag_)
        max_grad_mag_ = val;
    }
  }
  points_valid_ = false;
  parabola_fit_type_ = nsp.pfit_type_;
}

//: Constructor from a parameter block, and gradients along x and y directions given as images
//
sdet_nonmax_suppression::sdet_nonmax_suppression(sdet_nonmax_suppression_params& nsp,
                                                 vil_image_view<double> &grad_x,
                                                 vil_image_view<double> &grad_y)
  : sdet_nonmax_suppression_params(nsp)
{
  width_ = grad_x.ni();
  height_ = grad_x.nj();
  grad_x_.resize(width_, height_);
  grad_y_.resize(width_, height_);
  grad_mag_.resize(width_, height_);

  for (int j = 0; j < height_; j++)
  {
    for (int i = 0; i < width_; i++)
    {
      double val_x = grad_x(i,j);
      double val_y = grad_y(i,j);
      grad_x_(i,j) = val_x;
      grad_y_(i,j) = val_y;
      double val = vcl_sqrt(vcl_pow(val_x,2.0) + vcl_pow(val_y,2.0));
      grad_mag_(i,j) = val;
      if (val > max_grad_mag_)
        max_grad_mag_ = val;
    }
  }
  points_valid_ = false;
  parabola_fit_type_ = nsp.pfit_type_;
}

//: Constructor from a parameter block, gradient magnitudes given as an image and directions given as component image
//
sdet_nonmax_suppression::sdet_nonmax_suppression(sdet_nonmax_suppression_params& nsp, 
                                                 vil_image_view<double> &dir_x, 
                                                 vil_image_view<double> &dir_y,
                                                 vil_image_view<double> &grad_mag)
  : sdet_nonmax_suppression_params(nsp)
{
  width_ = grad_mag.ni();
  height_ = grad_mag.nj();
  grad_x_.resize(width_, height_);
  grad_y_.resize(width_, height_);
  grad_mag_.resize(width_, height_);

  for (int j = 0; j < height_; j++)
  {
    for (int i = 0; i < width_; i++)
    {
      grad_x_(i,j) = dir_x(i,j);
      grad_y_(i,j) = dir_y(i,j);
      double val = grad_mag(i,j);
      grad_mag_(i,j) = val;
      if (val > max_grad_mag_)
        max_grad_mag_ = val;
    }
  }
  points_valid_ = false;
  parabola_fit_type_ = nsp.pfit_type_;
}

//: Constructor from a parameter block, gradient magnitudes given as an image and the search directions
//
sdet_nonmax_suppression::sdet_nonmax_suppression(sdet_nonmax_suppression_params& nsp,
                                                 vil_image_view<double> &grad_mag,
                                                 vbl_array_2d<vgl_vector_2d <double> > &directions)
  : sdet_nonmax_suppression_params(nsp)
{
  width_ = grad_mag.ni();
  height_ = grad_mag.nj();
  grad_x_.resize(width_, height_);
  grad_y_.resize(width_, height_);
  grad_mag_.resize(width_, height_);
  for (int j = 0; j < height_; j++)
  {
    for (int i = 0; i < width_; i++)
    {
      double val = grad_mag(i,j);
      grad_mag_(i,j) = val;
      vgl_vector_2d<double> direction = directions(i,j);
      normalize(direction);
      grad_x_(i,j) = val * direction.x();
      grad_y_(i,j) = val * direction.y();
      if (val > max_grad_mag_)
        max_grad_mag_ = val;
    }
  }
  points_valid_ = false;
  parabola_fit_type_ = nsp.pfit_type_;
}

//:Default Destructor
sdet_nonmax_suppression::~sdet_nonmax_suppression()
{
}

//-------------------------------------------------------------------------
//: Apply the algorithm
//
void sdet_nonmax_suppression::apply()
{
  if (points_valid_)
    return;
  points_.clear();

  // run non-maximum suppression at every point

  for (int y = 1; y < height_-1; y++)
  {
    for (int x = 1; x < width_-1; x++)
    {
      //if (grad_mag_(x,y) > max_grad_mag_ * thresh_ / 100.0)
      if (grad_mag_(x,y) > thresh_)
      {
        double gx = grad_x_(x,y);
        double gy = grad_y_(x,y);
        vgl_vector_2d<double> direction(gx,gy);
        normalize(direction);
        if (vcl_abs(gx) > 10e-6 && vcl_abs(gy) > 10e-6)
        {
          int face_num = intersected_face_number(gx, gy);
          assert(face_num != -1);
          double s = intersection_parameter(gx, gy, face_num);
          assert(s != -1000);
          double f[3];
          f_values(x, y, gx, gy, s, face_num, f);
          double s_list[3];
          s_list[0] = -s;
          s_list[1] = 0.0;
          s_list[2] = s;
          if (f[1] > f[0] && f[1] > f[2])
          {
            double s_star = (parabola_fit_type_ == PFIT_3_POINTS)
                          ? subpixel_s(s_list, f)
                          : subpixel_s(x, y, direction);
            if (-1.5 < s_star && s_star < 1.5)
            {
              vgl_point_2d<double> subpix(x + s_star * direction.x(), y + s_star * direction.y());
              vsol_point_2d_sptr p = new vsol_point_2d(subpix.x(), subpix.y());
              vsol_point_2d_sptr line_start = new vsol_point_2d(subpix.x()-direction.y()*0.5, subpix.y()+direction.x()*0.5);
              vsol_point_2d_sptr line_end = new vsol_point_2d(subpix.x()+direction.y()*0.5, subpix.y()-direction.x()*0.5);
              vsol_line_2d_sptr l = new vsol_line_2d(line_start, line_end);
              points_.push_back(p);
              lines_.push_back(l);
              directions_.push_back(direction);
            }
          }
        }
      }
    }
  }
  points_valid_ = true;
}

int sdet_nonmax_suppression::intersected_face_number(double gx, double gy)
{
  if (gx >= 0 && gy >= 0)
  {
    if (gx >= gy)
      return 1;
    else
      return 2;
  }
  else if (gx < 0 && gy >= 0)
  {
    if (vcl_abs(gx) < gy)
      return 3;
    else
      return 4;
  }
  else if (gx < 0 && gy < 0)
  {
    if (vcl_abs(gx) >= vcl_abs(gy))
      return 5;
    else
      return 6;
  }
  else if (gx >= 0 && gy < 0)
  {
    if (gx < vcl_abs(gy))
      return 7;
    else
      return 8;
  }
  return -1;
}

double sdet_nonmax_suppression::intersection_parameter(double gx, double gy, int face_num)
{
  vgl_vector_2d<double> direction(gx,gy);
  normalize(direction);
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

void sdet_nonmax_suppression::f_values(int x, int y, double gx, double gy, double s, int face_num, double *f)
{
  vgl_vector_2d<double> direction(gx,gy);
  normalize(direction);

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

void sdet_nonmax_suppression::get_relative_corner_coordinates(int face_num, int *corners)
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

double sdet_nonmax_suppression::subpixel_s(double *s, double *f)
{
  double A = f[2] / ((s[2]-s[0])*(s[2]-s[1]));
  double B = f[1] / ((s[1]-s[0])*(s[1]-s[2]));
  double C = f[0] / ((s[0]-s[1])*(s[0]-s[2]));
  double s_star = ((A+B)*s[0] + (A+C)*s[1] + (B+C)*s[2]) / (2*(A+B+C));
  return s_star;
}

double sdet_nonmax_suppression::subpixel_s(int x, int y, vgl_vector_2d<double> direction)
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
      A(index, 0) = vcl_pow(s,2.0);
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
  return s_star;
}

void sdet_nonmax_suppression::find_distance_s_and_f_for_point(int x, int y, vgl_homg_line_2d<double> line,
                                                              double &d, double &s, vgl_vector_2d<double> direction)
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
void sdet_nonmax_suppression::clear()
{
  points_.clear();
  points_valid_ = false;
}
