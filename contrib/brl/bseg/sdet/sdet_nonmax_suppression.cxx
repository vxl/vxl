// This is brl/bseg/sdet/sdet_nonmax_suppression.cxx
#include "sdet_nonmax_suppression.h"
//:
// \file
#include <vcl_cstdlib.h>   // for vcl_abs(int) and vcl_sqrt()
#include <vil/vil_convert.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_line_2d.h>
#include <vnl/vnl_math.h>
#include <vil/algo/vil_convolve_2d.h>
#include <vcl_cstdio.h>
#include <vcl_cassert.h>

//---------------------------------------------------------------
// Constructors
//
//----------------------------------------------------------------

//: Constructor from a parameter block, and gradients along x and y directions
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
}

//: Constructor from a parameter block, gradient magnitudes and the search directions
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
  for(int j = 0; j < height_; j++)
  {
    for(int i = 0; i < width_; i++)
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

  int num_entry = 0;
  for (int y = 1; y < height_-1; y++)
  {
    for (int x = 1; x < width_-1; x++)
    {
      if (grad_mag_(x,y) > max_grad_mag_ * thresh_ / 100.0)
      {
        num_entry++;
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
          vcl_vector<double> f(f_values(x, y, gx, gy, s, face_num));
          vcl_vector<double> ss;
          ss.push_back(-s); ss.push_back(0); ss.push_back(s);
          if (f[1] > f[0] && f[1] > f[2])
          {
            double s_star = subpixel_s(ss, f);
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

vcl_vector<double> sdet_nonmax_suppression::f_values(int x, int y, double gx, double gy, double s, int face_num)
{
  vgl_vector_2d<double> direction(gx,gy);
  normalize(direction);

  vcl_vector<double> f;
  vcl_vector< vgl_vector_2d <int> > corners = get_relative_corner_coordinates(face_num);

  vgl_vector_2d<double> intersection_point = s * direction;
  vgl_vector_2d<double> corner1(corners[0].x(), corners[0].y()); //have to convert to double for subtraction
  vgl_vector_2d<double> corner2(corners[1].x(), corners[1].y()); //have to convert to double for subtraction
  double distance1 = length(intersection_point - corner1);
  double distance2 = length(intersection_point - corner2);
  double value1 = grad_mag_(x+corners[0].x(), y+corners[0].y());
  double value2 = grad_mag_(x+corners[1].x(), y+corners[1].y());
  double f_plus = value1 * distance2 + value2 * distance1;

  intersection_point = -s * direction;
  corner1.set(-corners[0].x(), -corners[0].y()); //have to convert to double for subtraction
  corner2.set(-corners[1].x(), -corners[1].y()); //have to convert to double for subtraction
  distance1 = length(intersection_point - corner1);
  distance2 = length(intersection_point - corner2);
  value1 = grad_mag_(x-corners[0].x(), y-corners[0].y());
  value2 = grad_mag_(x-corners[1].x(), y-corners[1].y());
  double f_minus = value1 * distance2 + value2 * distance1;

  f.push_back(f_minus);
  f.push_back(grad_mag_(x,y));
  f.push_back(f_plus);

  return f;
}

vcl_vector< vgl_vector_2d <int> > sdet_nonmax_suppression::get_relative_corner_coordinates(int face_num)
{
  vgl_vector_2d<int> corner1;
  vgl_vector_2d<int> corner2;
  switch (face_num)
  {
   case 1:
    corner1.set(1,0);
    corner2.set(1,1);
    break;
   case 2:
    corner1.set(1,1);
    corner2.set(0,1);
    break;
   case 3:
    corner1.set(0,1);
    corner2.set(-1,1);
    break;
   case 4:
    corner1.set(-1,1);
    corner2.set(-1,0);
    break;
   case 5:
    corner1.set(-1,0);
    corner2.set(-1,-1);
    break;
   case 6:
    corner1.set(-1,-1);
    corner2.set(0,-1);
    break;
   case 7:
    corner1.set(0,-1);
    corner2.set(1,-1);
    break;
   case 8:
    corner1.set(1,-1);
    corner2.set(1,0);
    break;
   default:
    corner1.set(0,0);
    corner2.set(0,0);
  }
  vcl_vector< vgl_vector_2d<int> > corners;
  corners.push_back(corner1);
  corners.push_back(corner2);
  return corners;
}

double sdet_nonmax_suppression::subpixel_s(vcl_vector<double> s, vcl_vector<double> f)
{
  double A = f[2] / ((s[2]-s[0])*(s[2]-s[1]));
  double B = f[1] / ((s[1]-s[0])*(s[1]-s[2]));
  double C = f[0] / ((s[0]-s[1])*(s[0]-s[2]));
  double s_star = ((A+B)*s[0] + (A+C)*s[1] + (B+C)*s[2]) / (2*(A+B+C));
  return s_star;
}

//----------------------------------------------------------
//: Clear internal storage
//
void sdet_nonmax_suppression::clear()
{
  points_.clear();
  points_valid_ = false;
}
