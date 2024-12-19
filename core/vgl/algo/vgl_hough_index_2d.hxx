// This is core/vgl/algo/vgl_orient_box_3d_operators.hxx
#ifndef vgl_hough_index_2d_hxx_
#define vgl_hough_index_2d_hxx_

#include <cmath>
#include <limits>
#include <iostream>
#include <algorithm>
#include "vgl_hough_index_2d.h"
//:
// \file
//
// Modifications : see vgl_hough_index_2d.h
//
//-----------------------------------------------------------------------------


#include <vcl_compiler.h>
#include <vnl/vnl_math.h>

template <class T>
class nlines
{
public:
  bool
  operator()(const std::vector<vgl_line_segment_2d<T>> & v1, const std::vector<vgl_line_segment_2d<T>> & v2)
  {
    return v1.size() > v2.size();
  }
};

//--------------------------------------------------------------
//
// Constructor and Destructor Functions
//
//--------------------------------------------------------------

//---------------------------------------------------------------
//: Simple Constructor
//
template <class T>
vgl_hough_index_2d<T>::vgl_hough_index_2d(const size_t r_dimension, const size_t theta_dimension)
{
  xo_ = 0;
  yo_ = 0;
  ysize_ = xsize_ = static_cast<T>(std::ceil(r_dimension / vnl_math::sqrt2));
  angle_range_ = static_cast<T>(theta_dimension);
  angle_increment_ = 1.0f;

  this->init(r_dimension, theta_dimension);
}

//--------------------------------------------------------------
//: Constructors from given bounds
//
template <class T>
vgl_hough_index_2d<T>::vgl_hough_index_2d(const T x0, const T y0, const T xsize, const T ysize, const T angle_increment)
  : xo_(x0)
  , yo_(y0)
  , xsize_(xsize)
  , ysize_(ysize)
  , angle_range_(T(180))
  , angle_increment_(angle_increment)
{

  size_t theta_dimension = static_cast<size_t>(std::ceil(angle_range_ / angle_increment_));
  // theta_dimension++; //Include both 0 and angle_range_  (no! near test across cut will fail)
  T diag = std::sqrt(xsize * xsize + ysize * ysize);
  size_t rmax = size_t(diag);
  rmax++; // Round off.
  this->init(rmax, theta_dimension);
}
template <class T>
vgl_hough_index_2d<T>::vgl_hough_index_2d(const vgl_box_2d<T> & box, const T angle_increment)
  : angle_range_(T(180))
  , angle_increment_(angle_increment)
{

  xo_ = box.min_x();
  yo_ = box.min_y();
  xsize_ = box.max_x() - xo_;
  ysize_ = box.max_y() - yo_;

  size_t theta_dimension = static_cast<size_t>(std::ceil(angle_range_ / angle_increment_));
  // theta_dimension++; //Include both 0 and angle_range_ (no! near test across cut will fail)
  T diag = std::sqrt(xsize_ * xsize_ + ysize_ * ysize_);
  size_t rmax = size_t(diag);
  rmax++; // Round off.
  this->init(rmax, theta_dimension);
}
template <class T>
T
vgl_hough_index_2d<T>::tangent_angle(const vgl_line_segment_2d<T> & line)
{
  vgl_vector_2d<T> dir = line.point2() - line.point1();
  T ang = atan2(dir.y(), dir.x()) / vnl_math::pi_over_180;
  if (ang < 0)
    ang += 360.0;
  return ang;
}
//-----------------------------------------------------------------------------
//
//: Compute the vgl_hough_index_2d array locations corresponding to a line
//
template <class T>
void
vgl_hough_index_2d<T>::array_loc(const vgl_line_segment_2d<T> & line, T & r, T & theta) const
{
  // Compute angle index
  T one_eighty = static_cast<T>(180);
  T angle = tangent_angle(line);
  if (angle >= one_eighty)
    angle -= one_eighty;

  if (angle > angle_range_)
  {
    std::cout << "Warning - vgl_hough_index_2d angle outside of range!\n";
    return;
  }

  theta = angle;

  T angrad = vnl_math::pi_over_180 * angle;

  // Compute distance indices
  vgl_point_2d<T> mid = line.mid_point();

  T midx = T(mid.x() - xo_);
  T midy = T(mid.y() - yo_);
  T xs2 = xsize_ / T(2);
  T ys2 = ysize_ / T(2);

  double cx = -(midx - xs2) * std::sin(angrad);
  double cy = (midy - ys2) * std::cos(angrad);

  // We use the middle of the ranges as the origin to insure minimum error
  // Also, the distance index is guaranteed to be positive
  r = static_cast<T>(cx + cy + std::sqrt(xs2 * xs2 + ys2 * ys2));
}

//-----------------------------------------------------------------------------
//
//: Compute the vgl_hough_index_2d array locations corresponding to a line
//
template <class T>
void
vgl_hough_index_2d<T>::array_loc(const vgl_line_segment_2d<T> & line, size_t & r, size_t & theta) const
{
  T angle = 0, radius = 0;
  this->array_loc(line, radius, angle);
  theta = (size_t)std::floor(angle / angle_increment_);
  r = size_t(radius);
}

//-----------------------------------------------------------------------------
//
//: Modify vgl_hough_index_2d array R location under translation
//
template <class T>
size_t
vgl_hough_index_2d<T>::trans_loc(const size_t transx, const size_t transy, const size_t r, const size_t theta)
{
  T angle = angle_increment_ * theta;
  T angrad = vnl_math::pi_over_180 * angle;
  int new_cx = -static_cast<int>(transx * std::sin(angrad));
  int new_cy = static_cast<int>(transy * std::cos(angrad));
  int newr = new_cx + new_cy;
  newr += r;
  if (newr < 0)
    return 0;
  else if (newr > static_cast<int>(r_dim_))
    return r_dim_;
  else
    return static_cast<size_t>(newr);
}

//-----------------------------------------------------------------------------
//
//: Return the count at a given r and theta
//
//-----------------------------------------------------------------------------
template <class T>
size_t
vgl_hough_index_2d<T>::count(const size_t r, const size_t theta)
{
  if (r < 0 || theta < 0 || r >= r_dim_ || theta >= th_dim_)
  {
    std::cout << "Warning - vgl_hough_index_2d index outside of range!\n";
    return 0;
  }

  return index_[r][theta].size();
}

//-----------------------------------------------------------------------------
//
//: Method to index size_to vgl_hough_index_2d index given a vgl_line_segment_2d<T>.
//
template <class T>
bool
vgl_hough_index_2d<T>::insert(const vgl_line_segment_2d<T> & line)
{

  size_t r, theta;
  this->array_loc(line, r, theta);
  if (!(r < r_dim_) || !(theta < th_dim_))
    return false;
  size_t index = lines_.size();
  lines_.push_back(line);
  index_[r][theta].push_back(index);
  return true;
}

//-----------------------------------------------------------------------------
//
//: Method to index size_to vgl_hough_index_2d index given a vgl_line_segment_2d<T>.
//  only a unique new vgl_line_segment_2d<T> is added
template <class T>
bool
vgl_hough_index_2d<T>::insert_new(const vgl_line_segment_2d<T> & line)
{

  size_t r, theta;
  // Check array bounds and uniqueness of line
  this->array_loc(line, r, theta);
  if (!(r < r_dim_) || !(theta < th_dim_))
    return false;

  const std::vector<size_t> & line_ids = index_[r][theta];
  size_t n = line_ids.size();
  if (n == 0)
  {
    size_t index = lines_.size();
    lines_.push_back(line);
    index_[r][theta].push_back(index);
    return true;
  }
  for (size_t i = 0; i < n; ++i)
  {
    if (line == lines_[line_ids[i]])
      return false;
  }
  size_t index = lines_.size();
  lines_.push_back(line);
  index_[r][theta].push_back(index);
  return true;
}

//-----------------------------------------------------------------------------
//
//: find if a line is in the array
template <class T>
bool
vgl_hough_index_2d<T>::find(const vgl_line_segment_2d<T> & line)
{
  size_t r, theta;
  this->array_loc(line, r, theta);
  const std::vector<size_t> & line_ids = index_[r][theta];
  size_t n = line_ids.size();
  for (size_t i = 0; i < n; ++i)
    if (line == lines_[line_ids[i]])
      return true;
  return false;
}
//-----------------------------------------------------------------------------
//
//: find the line defined by a specific r and theta
template <class T>
bool
vgl_hough_index_2d<T>::find(T r, T theta, vgl_line_segment_2d<T> & line)
{
  if (r < 0 || theta < 0)
    return false;
  size_t integer_theta = (size_t)std::floor(theta / angle_increment_);
  size_t integer_r = size_t(r);
  if (integer_r >= r_dim_ || integer_theta >= th_dim_)
    return false;
  const std::vector<size_t> & line_indices = index_[integer_r][integer_theta];
  size_t nl = line_indices.size();
  bool found = false;
  for (size_t i = 0; i < nl && !found; ++i)
  {
    T found_r, found_theta;
    array_loc(lines_[line_indices[i]], found_r, found_theta);
    if (found_r == r && found_theta == theta)
    {
      found = true;
      line = lines_[line_indices[i]];
    }
  }
  return found;
}

//-----------------------------------------------------------------------------
//
//: remove a line from the Hough index
//
template <class T>
bool
vgl_hough_index_2d<T>::remove(const vgl_line_segment_2d<T> & line)
{
  size_t r, theta;
  this->array_loc(line, r, theta);
  if (!(r < r_dim_) || !(theta < th_dim_))
    return false;
  std::vector<size_t> & line_ids = index_[r][theta];
  size_t n = line_ids.size();
  bool found = false;
  size_t match_i = 0;
  for (size_t i = 0; i < n && !found; ++i)
    if (line == lines_[line_ids[i]])
    {
      match_i = i;
      found = true;
    }
  if (!found)
    return false;
  size_t idx = line_ids[match_i];
  line_ids.erase(line_ids.begin() + match_i);
  // lines_.erase(lines_.begin()+ idx); //don't erase since corrupts index space
  return true;
}
//-----------------------------------------------------------------------------
//
//: Fill a vector of line indices at the index location
//
//-----------------------------------------------------------------------------
template <class T>
void
vgl_hough_index_2d<T>::line_indices_at_index(const size_t r, const size_t theta, std::vector<size_t> & line_indices)
{
  line_indices.clear();
  if ((theta < 0) || (theta >= th_dim_) || (r < 0) || (r >= r_dim_))
    return;
  line_indices = index_[r][theta];
}
template <class T>
std::vector<size_t>
vgl_hough_index_2d<T>::line_indices_at_index(const size_t r, const size_t theta)
{
  std::vector<size_t> ret;
  this->line_indices_at_index(r, theta, ret);
  return ret;
}

//-----------------------------------------------------------------------------
//
//: Fill a vector of vgl_line_segment_2d<T>(s) which are at the index location
//
//-----------------------------------------------------------------------------
template <class T>
void
vgl_hough_index_2d<T>::lines_at_index(const size_t r, const size_t theta, std::vector<vgl_line_segment_2d<T>> & lines)
{
  lines.clear();
  if ((theta < 0) || (theta >= th_dim_) || (r < 0) || (r >= r_dim_))
    return;

  const std::vector<size_t> & line_ids = index_[r][theta];
  size_t n = line_ids.size();
  if (n == 0)
    return;
  for (size_t i = 0; i < n; i++)
    lines.push_back(lines_[line_ids[i]]);
}

//-----------------------------------------------------------------------------
//
//: Return a list of vgl_line_segment_2d<T>(s) which are at the index location
//
//-----------------------------------------------------------------------------
template <class T>
std::vector<vgl_line_segment_2d<T>>
vgl_hough_index_2d<T>::lines_at_index(const size_t r, const size_t theta)
{
  std::vector<vgl_line_segment_2d<T>> out;
  this->lines_at_index(r, theta, out);
  return out;
}

//-----------------------------------------------------------------------------
//
//: Fill a list of vgl_line_segment_2d<T>(s) which are within a distance(radius) of the r,theta values of a given line.
//
//-----------------------------------------------------------------------------
template <class T>
bool
vgl_hough_index_2d<T>::lines_near_eq(const vgl_line_segment_2d<T> & la,
                                     const vgl_line_segment_2d<T> & lb,
                                     const T r_tol,
                                     const T theta_tol) const
{
  T a_angle, b_angle;
  T a_ndist, b_ndist;
  this->array_loc(la, a_ndist, a_angle);
  this->array_loc(lb, b_ndist, b_angle);
  // Test error in normal distance
  bool within_r_radius = std::fabs(a_ndist - b_ndist) <= r_tol;
  if (!within_r_radius)
    return false;

  // Tests for angular error taking into account the cut at 180 degrees
  bool a_is_safe = ((180.0 - a_angle) >= theta_tol) && (a_angle >= theta_tol);
  bool b_is_safe = ((180.0 - b_angle) >= theta_tol) && (b_angle >= theta_tol);
  T one_eighty = T(180);
  if (a_is_safe && b_is_safe && std::fabs(a_angle - b_angle) <= theta_tol)
    return true;
  bool a_near_180 = ((one_eighty - a_angle) < theta_tol), b_near_180 = ((one_eighty - b_angle) < theta_tol);
  bool a_near_0 = (a_angle < theta_tol), b_near_0 = (b_angle < theta_tol);

  if (a_near_180 && b_near_180 && fabs(a_angle - b_angle) <= theta_tol)
    return true;

  if (a_near_0 && b_near_0 && fabs(a_angle - b_angle) <= theta_tol)
    return true;

  if (a_near_0 && b_near_180)
  {
    T temp = b_angle - one_eighty;
    return fabs(a_angle - temp) <= theta_tol;
  }
  if (a_near_180 && b_near_0)
  {
    T temp = a_angle - one_eighty;
    return fabs(b_angle - temp) <= theta_tol;
  }
  return false;
}
template <class T>
void
vgl_hough_index_2d<T>::lines_in_interval(const vgl_line_segment_2d<T> & l,
                                         const T r_dist,
                                         const T theta_dist,
                                         std::vector<vgl_line_segment_2d<T>> & lines)
{
  lines.clear();
  size_t r, theta;
  this->array_loc(l, r, theta);
  if ((theta < 0) || (theta >= th_dim_) || (r < 0) || (r >= r_dim_))
    return;
  int angle_radius = static_cast<int>(std::ceil(theta_dist / angle_increment_));
  int r_radius = static_cast<int>(std::ceil(r_dist));
  size_t th_dim_m1 = th_dim_ - 1;

  for (int i = -angle_radius; i <= angle_radius; i++)
  {
    // The angle space is circular
    int t_indx = (theta + i) % (th_dim_);
    if (t_indx < 0)
      t_indx += th_dim_m1;

    for (int j = -r_radius; j <= r_radius; j++)
    {
      int r_indx = r + j;
      if ((r_indx < 0) || (r_indx >= static_cast<int>(r_dim_)))
        continue;
      std::vector<vgl_line_segment_2d<T>> temp;
      this->lines_at_index(r_indx, t_indx, temp);
      for (typename std::vector<vgl_line_segment_2d<T>>::iterator lit = temp.begin(); lit != temp.end(); lit++)
      {
        // Note, these tests should eventually be more
        // sophisticated - JLM
        const vgl_line_segment_2d<T> & line = *lit;
#if 0

        T l_angle, line_angle;
        T l_ndist, line_ndist;
        this->array_loc(l, l_ndist, l_angle);
        this->array_loc(line, line_ndist, line_angle);

        //Test error in normal distance
        bool within_r_radius = std::fabs(l_ndist - line_ndist) < r_dist;
        if (!within_r_radius)
          continue;

        //Test angular error
        bool within_angle_radius = std::fabs(l_angle - line_angle) < theta_dist;
        if (!within_angle_radius)
          continue;
#endif
        if (lines_near_eq(l, line, r_dist, theta_dist))
          lines.push_back(line); // line, passed both tests
      }
    }
  }
}

//-----------------------------------------------------------------------------
//
//: Return a list of vgl_line_segment_2d<T>(s) which are within a distance(radius) of the r,theta values of a given
//: line.
//
//-----------------------------------------------------------------------------
template <class T>
std::vector<vgl_line_segment_2d<T>>
vgl_hough_index_2d<T>::lines_in_interval(const vgl_line_segment_2d<T> & l, const T r_dist, const T theta_dist)
{
  std::vector<vgl_line_segment_2d<T>> out;
  this->lines_in_interval(l, r_dist, theta_dist, out);
  return out;
}

template <class T>
void
vgl_hough_index_2d<T>::lines_in_interval(const size_t r_index,
                                         const size_t theta_index,
                                         const T r_dist,
                                         const T theta_dist,
                                         std::vector<vgl_line_segment_2d<T>> & lines)
{
  lines.clear();
  std::vector<size_t> line_indices;
  this->line_indices_in_interval(r_index, theta_index, r_dist, theta_dist, line_indices);
  for (std::vector<size_t>::iterator lit = line_indices.begin(); lit != line_indices.end(); ++lit)
    lines.push_back(lines_[*lit]);
}
template <class T>
std::vector<vgl_line_segment_2d<T>>
vgl_hough_index_2d<T>::lines_in_interval(const size_t r_index,
                                         const size_t theta_index,
                                         const T r_dist,
                                         const T theta_dist)
{
  std::vector<vgl_line_segment_2d<T>> ret;
  this->lines_in_interval(r_index, theta_index, r_dist, theta_dist, ret);
  return ret;
}

template <class T>
void
vgl_hough_index_2d<T>::line_indices_in_interval(const size_t r_index,
                                                const size_t theta_index,
                                                const T r_dist,
                                                const T theta_dist,
                                                std::vector<size_t> & line_indices)
{
  if ((theta_index >= th_dim_) || (r_index >= r_dim_))
    return;
  int angle_radius = static_cast<int>(std::ceil(theta_dist / angle_increment_));
  int r_radius = static_cast<int>(std::ceil(r_dist));
  size_t th_dim_m1 = th_dim_ - 1;

  std::vector<size_t> line_indices_at_center;
  line_indices_at_index(r_index, theta_index, line_indices_at_center);

  std::vector<vgl_line_segment_2d<T>> lines_at_center;
  lines_at_index(r_index, theta_index, lines_at_center);

  line_indices = line_indices_at_center;

  for (int i = -angle_radius; i <= angle_radius; i++)
  {

    // The angle space is circular
    int t_indx = (theta_index + i) % (th_dim_);
    if (t_indx < 0)
      t_indx += th_dim_m1;

    for (int j = -r_radius; j <= r_radius; j++)
    {
      if (i == 0 && j == 0)
        continue; // skip center query bin

      int r_indx = r_index + j;

      if ((r_indx < 0) || (r_indx >= static_cast<int>(r_dim_)))
        continue;
      std::vector<vgl_line_segment_2d<T>> temp;
      this->lines_at_index(r_indx, t_indx, temp);
      std::vector<size_t> temp_i;
      this->line_indices_at_index(r_indx, t_indx, temp_i);
      size_t idx_tmp = 0;
      for (typename std::vector<vgl_line_segment_2d<T>>::iterator lit = temp.begin(); lit != temp.end();
           lit++, ++idx_tmp)
      {
        // Note, these tests should eventually be more
        // sophisticated - JLM
        const vgl_line_segment_2d<T> & line = *lit;
        size_t idx_cent = 0;
        for (typename std::vector<vgl_line_segment_2d<T>>::iterator cit = lines_at_center.begin();
             cit != lines_at_center.end();
             ++cit, ++idx_cent)
        {
          if (lines_near_eq(*cit, line, r_dist, theta_dist))
            line_indices.push_back(temp_i[idx_tmp]); // line, passed both tests
        }
      }
    }
  }
}

template <class T>
std::vector<size_t>
vgl_hough_index_2d<T>::line_indices_in_interval(const size_t r_index,
                                                const size_t theta_index,
                                                const T r_dist,
                                                const T theta_dist)
{
  std::vector<size_t> ret;
  this->line_indices_in_interval(r_index, theta_index, r_dist, theta_dist, ret);
  return ret;
}

//-----------------------------------------------------------------------------
//
//: Fill a list of vgl_line_segment_2d<T>(s) which are within angle_dist of a given angle
//
//-----------------------------------------------------------------------------
template <class T>
void
vgl_hough_index_2d<T>::parallel_lines(const T angle, const T angle_dist, std::vector<vgl_line_segment_2d<T>> & lines)
{
  lines.clear();
  T one_eighty = static_cast<T>(180);
  // Compute angle index and tolerance
  T ang = std::fmod(angle, one_eighty);
  if (ang < 0)
    ang = one_eighty - ang;

  if (ang > angle_range_)
  {
    std::cout << "Warning - vgl_hough_index_2d angle outside of range!\n";
    return;
  }
  int theta = static_cast<int>(std::floor(ang / angle_increment_));
  int angle_radius = static_cast<int>(std::ceil(angle_dist / angle_increment_));
  size_t th_dim_m1 = th_dim_ - 1;

  for (int i = -angle_radius; i <= angle_radius; i++)
  {
    // The angle space is circular
    size_t t_indx = (theta + i) % (th_dim_m1);
    if (t_indx < 0)
      t_indx += th_dim_m1;
    for (size_t j = 0; j < r_dim_; j++)
    {
      if (!(this->count(j, t_indx) > 0))
        continue;
      std::vector<vgl_line_segment_2d<T>> temp;
      this->lines_at_index(j, t_indx, temp);

      for (typename std::vector<vgl_line_segment_2d<T>>::iterator lit = temp.begin(); lit != temp.end(); lit++)
      {
        const vgl_line_segment_2d<T> & line = *lit;
        // Test angular error
        T line_angle = tangent_angle(line);
        if (line_angle >= one_eighty)
          line_angle -= one_eighty;
        T ang_error = std::fabs(ang - line_angle);
        if (ang_error < angle_dist)
          lines.push_back(line);
        ang_error -= one_eighty; // anti-parallel lines are included
        if (std::fabs(ang_error) < angle_dist)
          lines.push_back(line);
      }
    }
  }
}

//-----------------------------------------------------------------------------
//
//: Return a list of vgl_line_segment_2d<T>(s) which are within angle_dist of a given angle
//
//-----------------------------------------------------------------------------
template <class T>
std::vector<vgl_line_segment_2d<T>>
vgl_hough_index_2d<T>::parallel_lines(const T angle, const T angle_dist)
{
  std::vector<vgl_line_segment_2d<T>> out;
  this->parallel_lines(angle, angle_dist, out);
  return out;
}

//-----------------------------------------------------------------------------
//
//: Fill a list of vgl_line_segment_2d<T>(s) which are oriented at an angle with respect to a given line.
//  A specified angular tolerance can be given.
//
//-----------------------------------------------------------------------------
template <class T>
void
vgl_hough_index_2d<T>::lines_at_angle(const vgl_line_segment_2d<T> & l,
                                      const T angle,
                                      const T angle_dist,
                                      std::vector<vgl_line_segment_2d<T>> & lines)
{
  lines.clear();
  T ang = tangent_angle(l) + angle;
  this->parallel_lines(ang, angle_dist, lines);
}

//-----------------------------------------------------------------------------
//
//: Return a new list of vgl_line_segment_2d<T>(s) which are oriented at an angle with respect to a given line.
//  A specified angular tolerance can be given.
//-----------------------------------------------------------------------------
template <class T>
std::vector<vgl_line_segment_2d<T>>
vgl_hough_index_2d<T>::lines_at_angle(const vgl_line_segment_2d<T> & l, const T angle, const T angle_dist)
{
  std::vector<vgl_line_segment_2d<T>> out;
  this->lines_at_angle(l, angle, angle_dist, out);
  return out;
}

//-----------------------------------------------------------------------------
//
//: Fill a list of vgl_line_segment_2d<T>(s) which are within angle_dist of the orientation of a given line.
//
//-----------------------------------------------------------------------------
template <class T>
void
vgl_hough_index_2d<T>::parallel_lines(const vgl_line_segment_2d<T> & l,
                                      const T angle_dist,
                                      std::vector<vgl_line_segment_2d<T>> & lines)
{
  lines.clear();

  T angle = tangent_angle(l);
  this->parallel_lines(angle, angle_dist, lines);
}

//-----------------------------------------------------------------------------
//
//: Return a list of vgl_line_segment_2d<T>(s) which are within angle_dist of the orientation of a given line.
//
//-----------------------------------------------------------------------------
template <class T>
std::vector<vgl_line_segment_2d<T>>
vgl_hough_index_2d<T>::parallel_lines(const vgl_line_segment_2d<T> & l, const T angle_dist)
{
  std::vector<vgl_line_segment_2d<T>> out;

  this->parallel_lines(l, angle_dist, out);
  return out;
}

//-----------------------------------------------------------------------------
//
//: Clear the vgl_hough_index_2d<T> index space
//
//-----------------------------------------------------------------------------
template <class T>
void
vgl_hough_index_2d<T>::clear_index()
{
  for (size_t r = 0; r < r_dim_; r++)
    for (size_t th = 0; th < th_dim_; th++)
      index_[r][th].clear();
  lines_.clear();
}

//-----------------------------------------------------------------
//: Constructor Utility
//
template <class T>
void
vgl_hough_index_2d<T>::init(const size_t r_dimension, const size_t theta_dimension)
{
  r_dim_ = r_dimension;
  th_dim_ = theta_dimension;
  index_.resize(r_dim_, std::vector<std::vector<size_t>>(th_dim_));
}

//-----------------------------------------------------------------
//: Fill the angle histogram
//
template <class T>
std::vector<size_t>
vgl_hough_index_2d<T>::angle_histogram()
{
  std::vector<size_t> angle_hist(th_dim_);
  for (size_t x = 0; x < th_dim_; x++)
  {
    angle_hist[x] = 0;
    for (size_t y = 0; y < r_dim_; y++)
    {
      size_t n = index_[y][x].size();
      angle_hist[x] += n;
    }
  }
  return angle_hist;
}

//-------------------------------------------------------------------
//:
// Provides the correct values for angle histogram counts when the bin index
// extends outside the valid range of the counts array.  This function
// permits easy access logic for the non_maximum_suppression algorithm.
static size_t
get_extended_count(int bin, const std::vector<size_t> & ang_hist)
{
  size_t n_bins = ang_hist.size();
  size_t nbm = n_bins - 1;
  // Abnormal Cases
  if (bin < 0)
    return ang_hist[nbm + bin];

  if (bin >= static_cast<int>(n_bins))
    return ang_hist[bin - n_bins];
  // Normal Case
  return ang_hist[bin];
}

//---------------------------------------------------------------------
//: Prune any sequences of more than one maximum value.
// That is, it is possible to have a "flat" top peak with an arbitrarily
// long sequence of equal, but maximum values.
static void
remove_flat_peaks(std::vector<size_t> & angle_hist)
{
  size_t nbins = angle_hist.size();
  size_t nbm = nbins - 1;

  // Here we define a small state machine - parsing for runs of peaks
  // init is the state corresponding to an initial run (starting at i ==0)
  bool init = get_extended_count(0, angle_hist) > 0;
  size_t init_end = 0;

  // start is the state corresponding to any other run of peaks
  bool start = false;
  size_t start_index = 0;

  // The scan of the state machine
  for (size_t i = 0; i < nbins; i++)
  {
    size_t v = get_extended_count(i, angle_hist);

    // State init: a string of non-zeroes at the beginning.
    if (init && v != 0)
      continue;

    if (init && v == 0)
    {
      init_end = i;
      init = false;
      continue;
    }

    // State !init&&!start: a string of "0s"
    if (!start && v == 0)
      continue;

    // State !init&&start: the first non-zero value
    if (!start && v != 0)
    {
      start_index = i;
      start = true;
      continue;
    }
    // State ending flat peak: encountered a subsequent zero after starting
    if (start && v == 0)
    {
      size_t peak_location = (start_index + i - 1) / 2; // The middle of the run
      for (size_t k = start_index; k <= (i - 1); k++)
        if (k != peak_location)
          angle_hist[k] = 0;
      start = false;
    }
  }
  // Now handle the boundary conditions
  if (init_end != 0)
  { // Is there a run which crosses the cyclic cut?
    if (start)
    { // Yes, so define the peak location accordingly
      size_t peak_location = (start_index + init_end - nbm - 1) / 2;
      size_t k;
      if (peak_location < 0) // Is the peak to the left of the cut?
      {                      // Yes, to the left
        peak_location += nbm;
        for (k = 0; k < init_end; k++)
          angle_hist[k] = 0;
        for (k = start_index; k < nbins; k++)
          if (k != peak_location)
            angle_hist[k] = 0;
      }
      else
      { // No, on the right.
        for (k = start_index; k < nbins; k++)
          angle_hist[k] = 0;
        for (k = 0; k < init_end; k++)
          if (k != peak_location)
            angle_hist[k] = 0;
      }
    }
    else
    { // There wasn't a final run so just clean up the initial run
      size_t init_location = (init_end - 1) / 2;
      for (size_t k = 0; k <= init_end; k++)
        if (k != init_location)
          angle_hist[k] = 0;
    }
  }
}

//----------------------------------------------------------
//: Suppress values in the angle histogram which are not locally a maximum.
//  The neighborhood for computing the local maximum
//  is [radius X radius], e.g. for radius =1 the neighborhood
//  is [-X-], for radius = 2, the neighborhood is [--X--], etc.
//
template <class T>
std::vector<size_t>
vgl_hough_index_2d<T>::non_maximum_suppress(const size_t radius, const std::vector<size_t> & bins)
{
  size_t num = bins.size();
  int iradius = static_cast<int>(radius);
  std::vector<size_t> out(num);
  if (4 * radius + 2 > num)
  {
    std::cout << "vgl_hough_index_2d::non_maximum_suppress(..) - radius is too large\n";
    return out;
  }

  // Clear the output
  for (size_t indx = 0; indx < num; indx++)
    out[indx] = 0;

  // Find local maxima
  for (int indx = 0; indx < static_cast<int>(num); indx++)
  {
    // find the maximum value in the current kernel
    size_t max_count = bins[indx];
    for (int k = -iradius; k <= iradius; k++)
    {
      int index = indx + k;
      size_t c = get_extended_count(index, bins);
      if (c > max_count)
        max_count = c;
    }
    // Is position th a local maximum?
    if (max_count == bins[indx])
      out[indx] = max_count; // Yes. So set the counts to the max value
  }
  remove_flat_peaks(out);
  return out;
}

//-----------------------------------------------------------------
//: Find the dominant peaks in the direction histogram.
//  The output vector contains the theta indices of dominant direction peaks.
//  \param thresh is the minimum number of lines in a valid peak.
//  \param angle tol is the width of the peak in degrees.
template <class T>
size_t
vgl_hough_index_2d<T>::dominant_directions(const size_t thresh, const T angle_tol, std::vector<size_t> & dirs)
{
  size_t radius = static_cast<size_t>(0.5f + angle_tol / angle_increment_); // round to nearest size_t
  std::vector<size_t> angle_hist = this->angle_histogram();
  std::vector<size_t> suppressed_hist = this->non_maximum_suppress(radius, angle_hist);
  for (size_t i = 0; i < th_dim_; i++)
    if (suppressed_hist[i] >= thresh)
      dirs.push_back(i);
  return dirs.size();
}

//------------------------------------------------------------------
//: Get the dominant line groups in the hough index.
//  Sets of lines belonging to distinct orientations are returned.
//
template <class T>
size_t
vgl_hough_index_2d<T>::dominant_line_groups(const size_t thresh,
                                            const T angle_tol,
                                            std::vector<std::vector<vgl_line_segment_2d<T>>> & groups)
{
  groups.clear();
  std::vector<size_t> dirs;
  size_t n_groups = this->dominant_directions(thresh, angle_tol, dirs);
  if (!n_groups)
    return 0;
  for (size_t gi = 0; gi < n_groups; gi++)
  {
    std::vector<vgl_line_segment_2d<T>> lines;
    T angle = dirs[gi] * angle_increment_;
    this->parallel_lines(angle, angle_tol, lines);
    groups.push_back(lines);
  }
  // group with largest population first
  std::sort(groups.begin(), groups.end(), nlines<T>());
  return n_groups;
}
template <class T>
size_t
vgl_hough_index_2d<T>::dominant_group_hough_coords(const size_t thresh,
                                                   const T angle_tol,
                                                   std::vector<std::vector<std::pair<T, T>>> & group_h_coords)
{
  group_h_coords.clear();
  std::vector<std::vector<vgl_line_segment_2d<T>>> groups;
  size_t ng = this->dominant_line_groups(thresh, angle_tol, groups);
  for (size_t i = 0; i < ng; ++i)
  {
    std::vector<std::pair<T, T>> groupi;
    const std::vector<vgl_line_segment_2d<T>> & lines = groups[i];
    /// debug
    std::cout << "duplicate lines in hough coords" << std::endl;
    size_t nl = lines.size();
    for (size_t k = 0; k < nl; ++k)
    {
      const vgl_line_segment_2d<T> & lk = lines[k];
      for (size_t m = k + 1; m < nl; ++m)
      {
        const vgl_line_segment_2d<T> & lm = lines[m];
        if (lm == lk)
        {
          std::cout << "equal line " << lm << std::endl;
        }
      }
    }
    size_t nll = lines.size();
    for (size_t j = 0; j < nll; ++j)
    {
      T r, theta;
      this->array_loc(lines[j], r, theta);
      std::pair<T, T> pr(r, theta);
      groupi.push_back(pr);
    }
    group_h_coords.push_back(groupi);
  }
  return ng;
}
template <class T>
size_t
vgl_hough_index_2d<T>::max_count() const
{
  size_t max_cnt = 0;
  for (size_t r = 0; r < r_dim_; ++r)
    for (size_t theta = 0; theta < th_dim_; ++theta)
    {
      size_t cnt = index_[r][theta].size();
      if (cnt > max_cnt)
        max_cnt = cnt;
    }
  return max_cnt;
}

template <class T>
size_t
vgl_hough_index_2d<T>::min_count() const
{
  size_t min_cnt = std::numeric_limits<size_t>::max();
  for (size_t r = 0; r < r_dim_; ++r)
    for (size_t theta = 0; theta < th_dim_; ++theta)
    {
      size_t cnt = index_[r][theta].size();
      if (cnt < min_cnt)
        min_cnt = cnt;
    }
  return min_cnt;
}

template <class T>
T
vgl_hough_index_2d<T>::average_count() const
{
  T avg_cnt = T(0);
  T ncells = T(0);
  for (size_t r = 0; r < r_dim_; ++r)
    for (size_t theta = 0; theta < th_dim_; ++theta)
    {
      ncells += T(1);
      T cnt = static_cast<T>(index_[r][theta].size());
      avg_cnt += cnt;
    }
  if (ncells == T(0))
    return T(0);
  return avg_cnt / ncells;
}
template <class T>
void
vgl_hough_index_2d<T>::cells_ge_count(const size_t min_count, std::vector<std::pair<size_t, size_t>> & cells) const
{
  cells.clear();
  for (size_t r = 0; r < r_dim_; ++r)
    for (size_t theta = 0; theta < th_dim_; ++theta)
    {
      if (index_[r][theta].size() < min_count)
        continue;
      std::pair<size_t, size_t> pr(r, theta);
      cells.push_back(pr);
    }
}

template <class T>
void
vgl_hough_index_2d<T>::lines_with_cells_ge_count(const size_t min_count,
                                                 std::vector<vgl_line_segment_2d<T>> & lines) const
{
  lines.clear();
  for (size_t r = 0; r < r_dim_; ++r)
    for (size_t theta = 0; theta < th_dim_; ++theta)
    {
      if (index_[r][theta].size() < min_count)
        continue;
      const std::vector<size_t> & line_indices = index_[r][theta];
      size_t n = line_indices.size();
      for (size_t i = 0; i < n; ++i)
        lines.push_back(lines_[line_indices[i]]);
    }
}
#undef VGL_HOUGH_INDEX_2D_INSTANTIATE
#define VGL_HOUGH_INDEX_2D_INSTANTIATE(Type) template class vgl_hough_index_2d<Type>
#endif
