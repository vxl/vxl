#ifndef boxm_raytrace_operations_h_
#define boxm_raytrace_operations_h_
//:
// \file

#include <iostream>
#include "boxm_triangle_interpolation_iterator.h"
#include "boxm_triangle_scan_iterator.h"
#include <boxm/boxm_apm_traits.h>

#include <vnl/algo/vnl_determinant.h>
#include <vgl/vgl_triangle_scan_iterator.h>
#include <boct/boct_tree.h>
#include <vil/vil_image_view.h>

#if 0
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#endif

bool cube_fill_value(double* xverts_2d, double* yverts_2d, boct_face_idx visible_faces, vil_image_view<float> &img, float const& val);

bool boxm_alpha_seg_len(double *xverts_2d, double* yverts_2d, float* vert_distances, boct_face_idx visible_faces, float alpha, vil_image_view<float> &alpha_distance);
bool boxm_depth_fill(double *xverts_2d, double* yverts_2d,
                     float* vert_distances, boct_face_idx visible_faces,
                     vil_image_view<float> &depth_image);
template<class T, class tri_int_it_t>
bool tri_interpolated_weighted_sum(tri_int_it_t &tri_it, vil_image_view<T> const& values, T &val_sum, float &weight_sum, bool subtract)
{
  tri_it.reset();
  while (tri_it.next()) {
    int y = tri_it.scany();
    if (y < 0) {
      // not inside of image bounds yet. go to next scanline.
      continue;
    }
    unsigned int yu = (unsigned int)y;
    if (yu >= values.nj() ) {
      // we have left the image bounds. no need to continue.
      break;
    }
    if ( (tri_it.startx() >= (int)values.ni()) || (tri_it.endx() <= 0) ) {
      // no part of this scanline is within the image bounds. go to next scanline.
      continue;
    }
    unsigned int startx = (unsigned int)std::max((int)0,tri_it.startx());
    unsigned int endx = (unsigned int)std::min((int)values.ni(),tri_it.endx());

    if (subtract) {
      for (unsigned int x = startx; x < endx; ++x) {
        float point_dist = tri_it.value_at(x);
        val_sum -= values(x,yu) * point_dist;
        weight_sum -= point_dist;
      }
    }
    else {
      for (unsigned int x = startx; x < endx; ++x) {
        float point_dist = tri_it.value_at(x);
        val_sum += values(x,yu) * point_dist;
        weight_sum += point_dist;
      }
    }
  }
  return true;
}

//: returns the sum of the weights
template<class T1, class T2, class tri_it_t>
bool tri_interpolated_weighted_sum(tri_it_t &tri_it, vil_image_view<T1> const& weights, vil_image_view<T2> const& values, T2 &val_sum, T1 &weight_sum, bool subtract=false)
{
  tri_it.reset();
  while (tri_it.next()) {
    int y = tri_it.scany();
    if (y < 0) {
      // not inside of image bounds yet. go to next scanline.
      continue;
    }
    unsigned int yu = (unsigned int)y;
    if (yu >= values.nj() ) {
      // we have left the image bounds. no need to continue.
      break;
    }
    if ( (tri_it.startx() >= (int)values.ni()) || (tri_it.endx() <= 0) ) {
      // no part of this scanline is within the image bounds. go to next scanline.
      continue;
    }
    unsigned int startx = (unsigned int)std::max((int)0,tri_it.startx());
    unsigned int endx = (unsigned int)std::min((int)values.ni(),tri_it.endx());

    if (subtract) {
      for (unsigned int x = startx; x < endx; ++x) {
        T1 weight = tri_it.value_at(x)*weights(x,yu);
        val_sum -= weight * values(x,yu);
        weight_sum -= weight;
      }
    }
    else {
      for (unsigned int x = startx; x < endx; ++x) {
        T1 weight = tri_it.value_at(x)*weights(x,yu);
        val_sum += weight * values(x,yu);
        weight_sum += weight;
      }
    }
  }
  return true;
}


template<class T, class tri_it_t>
void tri_interpolate_values(tri_it_t &tri_it, vil_image_view<T> &img, bool subtract = false)
{
  tri_it.reset();
  while (tri_it.next()) {
    int y = tri_it.scany();
    if (y < 0) {
      // not inside of image bounds yet. go to next scanline.
      continue;
    }
    unsigned int yu = (unsigned int)y;
    if (yu >= img.nj() ) {
      // we have left the image bounds. no need to continue.
      break;
    }
    if ( (tri_it.startx() >= (int)img.ni()) || (tri_it.endx() <= 0) ) {
      // no part of this scanline is within the image bounds. go to next scanline.
      continue;
    }
    unsigned int startx = (unsigned int)std::max((int)0,tri_it.startx());
    unsigned int endx = (unsigned int)std::min((int)img.ni(),tri_it.endx());

    if (subtract) {
      for (unsigned int x = startx; x < endx; ++x) {
        img(x,yu) -= tri_it.value_at(x);
      }
    }
    else {
      for (unsigned int x = startx; x < endx; ++x) {
        img(x,yu) += tri_it.value_at(x);
      }
    }
  }
  return;
}


#if 0
template<class T, class tri_it_T>
void tri_interpolate_values(tri_it_T &tri_it, double* xvals, double* yvals, T* vals, vil_image_view<T> &img, vil_image_view<float> &pix_coverage, unsigned int v0=0, unsigned int v1=1, unsigned int v2=2)
{
  // first compute s0, s1, s2 such that  val = s0*x + s1*y + s2 for any point within the triangle
  // (no need to compute barycentric coordinates at each step)
  // subtract 0.5 from xvals and yvals, so that interpolated value at pixel x,y evaluates to coordinates x+0.5, y+0.5 (center of pixel)
  double Acol0[] = {xvals[v0]-0.5, xvals[v1]-0.5, xvals[v2]-0.5};
  double Acol1[] = {yvals[v0]-0.5, yvals[v1]-0.5, yvals[v2]-0.5};
  double Acol2[] = {1.0, 1.0, 1.0};

  double Z[] = {vals[v0], vals[v1], vals[v2]};

  double detA = vnl_determinant(Acol0, Acol1, Acol2);
  double s0 = vnl_determinant(Z, Acol1, Acol2) / detA;
  double s1 = vnl_determinant(Acol0, Z, Acol2) / detA;
  double s2 = vnl_determinant(Acol0, Acol1, Z) / detA;

  tri_it.reset();
  while (tri_it.next()) {
    int y = tri_it.scany();
    if (y < 0) {
      // not inside of image bounds yet. go to next scanline.
      continue;
    }
    unsigned int yu = (unsigned int)y;
    if (yu > img.nj() ) {
      // we have left the image bounds. no need to continue.
      break;
    }
    if ( (tri_it.startx() >= (int)img.ni()) || (tri_it.endx() <= 0) ) {
      // no part of this scanline is within the image bounds. go to next scanline.
      continue;
    }
    unsigned int startx = (unsigned int)std::max((int)0,tri_it.startx());
    unsigned int endx = (unsigned int)std::min((int)img.ni(),tri_it.endx());

    for (unsigned int x = startx; x < endx; ++x) {
      T interp_val = (T)(s0*x + s1*y + s2);
      if ( (min_img(x,yu) == 0) || (min_img(x,yu) > interp_val) ) {
        min_img(x,yu) = interp_val;
      }
      if (max_img(x,yu) < interp_val) {
        max_img(x,yu) = interp_val;
      }
    }
  }
  return;
}
#endif

template<class T, class tri_it_T>
void tri_interpolate_min_max(tri_it_T &tri_it, double* xvals, double* yvals, T* vals, vil_image_view<T> &min_img, vil_image_view<T> &max_img, unsigned int v0=0, unsigned int v1=1, unsigned int v2=2)
{
  // first compute s0, s1, s2 such that  val = s0*x + s1*y + s2 for any point within the triangle
  // (no need to compute barycentric coordinates at each step)
  // subtract 0.5 from xvals and yvals, so that interpolated value at pixel x,y evaluates to coordinates x+0.5, y+0.5 (center of pixel)
  double Acol0[] = {xvals[v0]-0.5, xvals[v1]-0.5, xvals[v2]-0.5};
  double Acol1[] = {yvals[v0]-0.5, yvals[v1]-0.5, yvals[v2]-0.5};
  double Acol2[] = {1.0, 1.0, 1.0};

  double Z[] = {vals[v0], vals[v1], vals[v2]};

  double detA = vnl_determinant(Acol0, Acol1, Acol2);
  double s0 = vnl_determinant(Z, Acol1, Acol2) / detA;
  double s1 = vnl_determinant(Acol0, Z, Acol2) / detA;
  double s2 = vnl_determinant(Acol0, Acol1, Z) / detA;

  tri_it.reset();
  while (tri_it.next()) {
    int y = tri_it.scany();
    if (y < 0) {
      // not inside of image bounds yet. go to next scanline.
      continue;
    }
    unsigned int yu = (unsigned int)y;
    if (yu >= max_img.nj() ) {
      // we have left the image bounds. no need to continue.
      break;
    }
    if ( (tri_it.startx() >= (int)max_img.ni()) || (tri_it.endx() <= 0) ) {
      // no part of this scanline is within the image bounds. go to next scanline.
      continue;
    }
    unsigned int startx = (unsigned int)std::max((int)0,tri_it.startx());
    unsigned int endx = (unsigned int)std::min((int)max_img.ni(),tri_it.endx());

    for (unsigned int x = startx; x < endx; ++x) {
      T interp_val = (T)(s0*x + s1*y + s2);
      if ( (min_img(x,yu) == 0) || (min_img(x,yu) > interp_val) ) {
        min_img(x,yu) = interp_val;
      }
      if (max_img(x,yu) < interp_val) {
        max_img(x,yu) = interp_val;
      }
    }
  }
  return;
}

template<class T1, class T2>
void tri_weighted_sum(vgl_triangle_scan_iterator<double> &tri_it, vil_image_view<T1> &img, vil_image_view<T2> &weights, T1 &val_sum, T2 &weight_sum)
{
  tri_it.reset();
  while (tri_it.next()) {
    int y = tri_it.scany();
    if (y < 0) {
      // not inside of image bounds yet. go to next scanline.
      continue;
    }
    unsigned int yu = (unsigned int)y;
    if (yu >= img.nj() ) {
      // we have left the image bounds. no need to continue.
      break;
    }
    if ( (tri_it.startx() >= (int)img.ni()) || (tri_it.endx() <= 0) ) {
      // no part of this scanline is within the image bounds. go to next scanline.
      continue;
    }
    unsigned int startx = (unsigned int)std::max((int)0,tri_it.startx());
    unsigned int endx = (unsigned int)std::min((int)img.ni(),tri_it.endx());

    for (unsigned int x = startx; x < endx; ++x) {
      val_sum += img(x,yu)*weights(x,yu);
      weight_sum += weights(x,yu);
    }
  }
  return;
}

template<class T, class tri_it_T>
void tri_fill_value(tri_it_T &tri_it, vil_image_view<T> &img, T val)
{
  tri_it.reset();
  while (tri_it.next()) {
    int y = tri_it.scany();
    if (y < 0) {
      // not inside of image bounds yet. go to next scanline.
      continue;
    }
    unsigned int yu = (unsigned int)y;
    if (yu >= img.nj() ) {
      // we have left the image bounds. no need to continue.
      break;
    }
    if ( (tri_it.startx() >= (int)img.ni()) || (tri_it.endx() <= 0) ) {
      // no part of this scanline is within the image bounds. go to next scanline.
      continue;
    }
    unsigned int startx = (unsigned int)std::max((int)0,tri_it.startx());
    unsigned int endx = (unsigned int)std::min((int)img.ni(),tri_it.endx());

    for (unsigned int x = startx; x < endx; ++x) {
      img(x,yu) = val;
    }
  }
  return;
}

#if 0 // this function commented out
template <class T>
void tri_fill_value_aa(boxm_triangle_scan_iterator_aa &tri_it, vil_image_view<T> &img, vil_image_view<float> &aa_weights, T val)
{
  tri_it.reset();
  while (tri_it.next())
  {
    int y = tri_it.scany();
    if (y < 0) {
      // not inside of image bounds yet. go to next scanline.
      continue;
    }
    unsigned int yu = (unsigned int)y;
    if (yu >= img.nj() ) {
      // we have left the image bounds. no need to continue.
      break;
    }
    if ( (tri_it.startx() >= (int)img.ni()) || (tri_it.endx() <= 0) ) {
      // no part of this scanline is within the image bounds. go to next scanline.
      continue;
    }
    unsigned int startx = (unsigned int)std::max((int)0,tri_it.startx());
    unsigned int endx = (unsigned int)std::min((int)img.ni(),tri_it.endx());

    for (unsigned int x = startx; x < endx; ++x) {
      float pix_cov = tri_it.pix_coverage(x);
      if ((pix_cov < 0.0f) || (pix_cov > 1.0f)) {
        std::cerr << " error: pix_cov = " << pix_cov << std::endl;
      }
      aa_weights(x,yu) += tri_it.pix_coverage(x);
      img(x,yu) += val * pix_cov;
    }
  }
  return;
}
#endif // 0

template<class T>
bool cube_mean(double* xverts_2d, double* yverts_2d, float* vert_dists, boct_face_idx visible_faces, vil_image_view<T> const& values, T &mean)
{
  // for each face, create two triangle iterators to interpolate distances
  T value_sum(0);
  float weight_sum(0);
  // X_LOW
  // tri 0
  boxm_triangle_interpolation_iterator<float> tri_it(xverts_2d, yverts_2d, vert_dists, 0, 3, 4);
  if (visible_faces & X_LOW)
    tri_interpolated_weighted_sum(tri_it, values, value_sum, weight_sum, true);
  else
    tri_interpolated_weighted_sum(tri_it, values, value_sum, weight_sum, false);

  // tri 1
  tri_it = boxm_triangle_interpolation_iterator<float>(xverts_2d, yverts_2d, vert_dists, 3, 4, 7);
  if (visible_faces & X_LOW)
    tri_interpolated_weighted_sum(tri_it, values, value_sum, weight_sum, true);
  else
    tri_interpolated_weighted_sum(tri_it, values, value_sum, weight_sum, false);

  // X_HIGH
  // tri 0
  tri_it = boxm_triangle_interpolation_iterator<float>(xverts_2d, yverts_2d, vert_dists, 1, 2, 5);
  if (visible_faces & X_HIGH)
    tri_interpolated_weighted_sum(tri_it, values, value_sum, weight_sum, true);
  else
    tri_interpolated_weighted_sum(tri_it, values, value_sum, weight_sum, false);

  // tri 1
  tri_it = boxm_triangle_interpolation_iterator<float>(xverts_2d, yverts_2d, vert_dists, 2, 5, 6);
  if (visible_faces & X_HIGH)
    tri_interpolated_weighted_sum(tri_it, values, value_sum, weight_sum, true);
  else
    tri_interpolated_weighted_sum(tri_it, values, value_sum, weight_sum, false);

  // Y_LOW
  // tri 0
  tri_it = boxm_triangle_interpolation_iterator<float>(xverts_2d, yverts_2d, vert_dists, 0, 1, 5);
  if (visible_faces & Y_LOW)
    tri_interpolated_weighted_sum(tri_it, values, value_sum, weight_sum, true);
  else
    tri_interpolated_weighted_sum(tri_it, values, value_sum, weight_sum, false);
  // tri 1
  tri_it = boxm_triangle_interpolation_iterator<float>(xverts_2d, yverts_2d, vert_dists, 4, 0, 5);
  if (visible_faces & Y_LOW)
    tri_interpolated_weighted_sum(tri_it, values, value_sum, weight_sum, true);
  else
    tri_interpolated_weighted_sum(tri_it, values, value_sum, weight_sum, false);

  // Y_HIGH
  // tri 0
  tri_it = boxm_triangle_interpolation_iterator<float>(xverts_2d, yverts_2d, vert_dists, 7, 3, 2);
  if (visible_faces & Y_HIGH)
    tri_interpolated_weighted_sum(tri_it, values, value_sum, weight_sum, true);
  else
    tri_interpolated_weighted_sum(tri_it, values, value_sum, weight_sum, false);
  // tri 1
  tri_it = boxm_triangle_interpolation_iterator<float>(xverts_2d, yverts_2d, vert_dists, 2, 6, 7);
  if (visible_faces & Y_HIGH)
    tri_interpolated_weighted_sum(tri_it, values, value_sum, weight_sum, true);
  else
    tri_interpolated_weighted_sum(tri_it, values, value_sum, weight_sum, false);

  // Z_LOW
  // tri 0
  tri_it = boxm_triangle_interpolation_iterator<float>(xverts_2d, yverts_2d, vert_dists, 0, 2, 1);
  if (visible_faces & Z_LOW)
    tri_interpolated_weighted_sum(tri_it, values, value_sum, weight_sum, true);
  else
    tri_interpolated_weighted_sum(tri_it, values, value_sum, weight_sum, false);
  // tri 1
  tri_it = boxm_triangle_interpolation_iterator<float>(xverts_2d, yverts_2d, vert_dists, 0, 2, 3);
  if (visible_faces & Z_LOW)
    tri_interpolated_weighted_sum(tri_it, values, value_sum, weight_sum, true);
  else
    tri_interpolated_weighted_sum(tri_it, values, value_sum, weight_sum, false);

  // Z_HIGH
  // tri 0
  tri_it = boxm_triangle_interpolation_iterator<float>(xverts_2d, yverts_2d, vert_dists, 4, 5, 6);
  if (visible_faces & Z_HIGH)
    tri_interpolated_weighted_sum(tri_it, values, value_sum, weight_sum, true);
  else
    tri_interpolated_weighted_sum(tri_it, values, value_sum, weight_sum, false);
  // tri 1
  tri_it = boxm_triangle_interpolation_iterator<float>(xverts_2d, yverts_2d, vert_dists, 4, 7, 6);
  if (visible_faces & Z_HIGH)
    tri_interpolated_weighted_sum(tri_it, values, value_sum, weight_sum, true);
  else
    tri_interpolated_weighted_sum(tri_it, values, value_sum, weight_sum, false);

  if (std::fabs(weight_sum) > 1e-6)  {
    mean = value_sum / weight_sum;
  }
  else {
    mean = T(0);
    return false;
  }
  return true;
}

inline void init_triangle_scan_iterator(vgl_triangle_scan_iterator<double> &tri_it, const double* xverts, const double* yverts, unsigned int v0=0, unsigned int v1=1, unsigned int v2=2)
{
  // subtract 0.5 to follow convention that the center of pixel i,j is located at coordinates i+0.5, j+0.5
  tri_it.a.x = xverts[v0] - 0.5; tri_it.a.y = yverts[v0] - 0.5;
  tri_it.b.x = xverts[v1] - 0.5; tri_it.b.y = yverts[v1] - 0.5;
  tri_it.c.x = xverts[v2] - 0.5; tri_it.c.y = yverts[v2] - 0.5;
}
#endif
